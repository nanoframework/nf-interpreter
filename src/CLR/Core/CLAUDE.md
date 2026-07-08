# Core runtime — generics implementation notes

Companion to the source in this directory. Captures the architectural
knowledge, gotchas, and debugging context for generics support in the CLR core
(type system, interpreter, execution engine) so the source files can stay
focused on the algorithms.

If you are about to read or modify any of these and the comment looks terse,
the rationale is in here:

- `TypeSystem.cpp` — `SignatureParser`, `FindVirtualMethodDef`,
  `MatchSignatureForVirtualDispatch`, `TypeDef_Instance::InitializeFromToken`,
  `MethodRef_Instance::InitializeFromToken`, `ResolveMethodRef`.
- `Interpreter.cpp` — `CEE_CALLVIRT`, `CEE_NEWOBJ`, `CEE_BOX`,
  `CEE_UNBOX_ANY`, `CEE_LDOBJ`, `CEE_STELEM_*`, `CEE_LDSFLD`/`CEE_STSFLD`/
  `CEE_LDSFLDA`, and the `CEE_CONSTRAINED` prefix handler.
- `Execution.cpp` — `ExecutionEngine_RunGenericStaticConstructors`,
  `SpawnStaticConstructor`, `ResolveGenericTypeParameter`,
  `InitializeLocals`.
- `CLR_RT_HeapBlock.cpp` — `SetGenericInstanceType`, `TypeDescriptorsMatch`,
  the `CLASS → GENERICINST` promotion path.

Generics in this CLR are an active preview. Expect gaps. When something
doesn't work, the first question is almost always *which generic context is
this code path looking at?* — open vs closed TypeSpec, MethodSpec presence,
arrayElementType fallback, `this`-object's stored TypeSpec.

---

## 1. Mental model

**TypeDef vs TypeSpec.** A `TypeDef` is a class/struct declaration
(`Dictionary\`2`). A `TypeSpec` is an *instantiation* — either open
(`Dictionary<TKey,TValue>`, parameters still symbolic) or closed
(`Dictionary<int,string>`, all parameters concrete). Generic dispatch is
fundamentally about choosing the right closed TypeSpec for a given call site.

**VAR / MVAR / GENERICINST.**
- `DATATYPE_VAR` (`!N`) is a type-level generic parameter — `T` in
  `class Foo<T>`.
- `DATATYPE_MVAR` (`!!N`) is a method-level generic parameter — `T` in
  `void Bar<T>()`.
- `DATATYPE_GENERICINST` is the marker introducing a generic instantiation
  in a signature. The element stream is: `GENERICINST` marker → CLASS or
  VALUETYPE element carrying the *generic typedef* + arg count → that many
  argument elements (each may itself be a nested `GENERICINST`).

**How a closed TypeSpec is selected at a call site.** Roughly:

1. If the IL has an explicit MethodSpec, use it (and its instantiation).
2. Otherwise, if the MethodRef owner is itself a closed TypeSpec, use that.
3. Otherwise, look at the caller's generic context (`stack->m_call.genericType`).
4. Otherwise, recover a closed TypeSpec from the `this` object's stored
   `HB_GenericInstance` TypeSpec.
5. Otherwise, fall back to `arrayElementType` (runtime-inferred element type
   from operations like `newarr T[]` or `ldelem.ref`).
6. If everything fails, the call lands in a slot that can't resolve `VAR` and
   typically surfaces as `CLR_E_WRONG_TYPE`.

The priority order matters because some intermediates are *open* and would
silently corrupt VAR resolution if used as if they were closed. See §4.

---

## 2. Signature parsing (`SignatureParser`)

`Advance(Element &res)` walks one element of a signature at a time and
*overwrites* `res` with the parsed element. Two consequences that have
already bitten us:

- **Never cache a field from `res` and then call `Advance(res)` again** —
  the field is gone. The drain loop in `MatchSignatureForVirtualDispatch`
  was buggy because it re-read `inner.GenParamCount` after `Advance(inner)`
  overwrote `inner`. Drain by converging `Available()`, not by counting
  arguments.
- **`ParamCount` is the parser's remaining-element budget**, decremented by
  every `Advance` call. It correctly accounts for nested generic args. Code
  that drains a generic sub-tree should rely on `Available()` rather than
  walking arg counts by hand — the parser already does the bookkeeping.

`m_pendingGenericInst` is a one-shot flag. When `Advance` sees a
`GENERICINST` marker it sets the flag; the next `CLASS`/`VALUETYPE` element
then consumes its arg-count byte *unconditionally* — even for nested types
whose own `genericParamCount` is zero (their TypeSpec still carries the
enclosing-type args). The flag is cleared once consumed. Skipping this
breaks nested generic enumerators (e.g. `Dictionary<,>.Enumerator`).

---

## 3. Virtual method dispatch on generics

Implemented in `CLR_RT_TypeSystem::FindVirtualMethodDef` and the cache in
front of it (`CLR_RT_EventCache::FindVirtualMethod`).

### Explicit interface implementation name mangling

C# compilers emit explicit interface implementations under the *expanded*
qualified name, e.g.

```
System.Collections.Generic.ICollection<KeyValuePair<TKey,TValue>>.Remove
```

But `BuildTypeName` (used internally by the runtime to construct interface
qualifiers for lookup) produces the *backtick* form:

```
System.Collections.Generic.ICollection`1.Remove
```

These never match by string equality. That's why `FindVirtualMethodDef`
does three passes:

1. **Backtick exact name** (e.g. `ICollection\`1.Remove`) — catches explicit
   impls whose stored name happens to use the backtick form.
2. **Suffix-only** match on `.<calleeName>` — catches explicit impls stored
   under the expanded name. `suffixMatchOnly=true` rejects exact short-name
   matches so that, e.g., `Dictionary.Remove(TKey)` does not shadow the
   explicit `ICollection<KVP<,>>.Remove(KVP<,>)`.
3. **Exact short name** — last-resort match for ordinary virtual methods.

Each pass also has to pass the signature matcher (§3 below). Pass 3
matching the *wrong* method is one of the most common failure modes: an
explicit impl whose signature matcher silently fails falls through to a
same-named public method with a compatible signature.

### `MatchSignatureForVirtualDispatch` — VAR ↔ GENERICINST drain

The interface side of an explicit impl uses type parameters (`VAR N` in the
signature), while the concrete implementation side uses the expanded
`GENERICINST` form. The matcher must accept the mismatch and drain the
`GENERICINST` sub-tree on the expanded side so both parsers stay aligned.

**The drain MUST work by convergence**, not by counting. The element stream
for a generic instance is variable-depth (any argument can itself be a
nested `GENERICINST`) and the per-element `GenParamCount` is only valid on
the element that introduced the generic, not on its drained children.
Correct pattern:

```cpp
int targetAvail = iAvailLeft - 1;     // simple-VAR side already advanced once
while (parserRight.Available() > targetAvail) {
    CLR_RT_SignatureParser::Element drained{};
    if (FAILED(parserRight.Advance(drained))) return false;
}
```

This consumes exactly one full parameter regardless of arity or nesting
depth, because `ParamCount` already accounts for nested args. The previous
arg-counting implementation only drained the first argument of
`KeyValuePair<TKey,TValue>` and corrupted alignment for every multi-arg
generic.

### TypeSpec recovery from `this`

When `CALLVIRT` resolves a `MethodRef` whose owner is an interface
`TypeSpec` (e.g. `ICollection<KVP<int,string>>`), `ResolveToken` stores
that interface TypeSpec on `calleeInst.genericType`. After virtual
dispatch lands on the concrete implementation in `Dictionary<TKey,TValue>`,
the stored TypeSpec is *for the wrong TypeDef* and would corrupt
VAR resolution in the callee.

Recovery: detect the mismatch (TypeSpec's `genericTypeDef` differs from the
callee's declaring `TypeDef`), and try to pull the correct closed TypeSpec
from the `this` heap-block's `HB_GenericInstance` slot
(`thisHeap->ObjectGenericType()`). If recovery fails, **null is safer than
a mismatched TypeSpec** — the callee will then fall back through the
priority chain in §4 rather than confidently using the wrong context.

---

## 4. Generic context propagation (CALLVIRT / CALL)

`CEE_CALLVIRT` and `CEE_CALL` set up the callee's `m_call.genericType`
using a four-tier priority:

1. **`effectiveCallerGeneric` if its TypeDef matches the callee's owner.**
   Set by the upstream TypeSpec search that walks the caller's assembly to
   find a closed TypeSpec whose `genericTypeDef` matches the `this`
   object's `TypeDef`. This is the interface-callvirt case where the
   compile-time MethodRef points at an interface but the runtime dispatch
   lands on the concrete generic class.
2. **`calleeInst.genericType` if it is closed.** Set by `ResolveToken`
   from the MethodRef's owner TypeSpec, refined by MVAR resolution against
   the caller's MethodSpec. The closed-only gate matters: an open TypeSpec
   like `KeyValuePair<TKey,TValue>` cannot resolve VAR indices and causes
   `CLR_E_WRONG_TYPE` deep inside the callee.
3. **`effectiveCallerGeneric` as a fallback** when the TypeDef doesn't
   match (covers cases where the search above found a TypeSpec but it
   isn't a fit for the callee).
4. **Caller's inherited `genericType`** — last resort.

### PushInline vs Push

The same priority logic is duplicated in the inline path and the regular
push path. `PushInline` switches `stack->m_call` to the callee but does
not apply generic-context inheritance on its own, so the CALLVIRT handler
has to mirror the priority selection in both branches. **If you change
the priority order, change it in both places** or generic dispatch will
silently regress on whichever path you missed.

### MethodSpec inheritance for open callee + MVAR

When the callee's `genericType` is open (still contains MVAR), inherit the
caller's `methodSpec` so the callee can close MVAR slots from the call
site. Without this, generic-method calls like `Array.Empty<T>` from inside
another generic method lose their MVAR binding.

---

## 5. VAR / MVAR resolution

Lives in `CLR_RT_TypeDef_Instance::InitializeFromToken` (in
`TypeSystem.cpp`). For both `DATATYPE_VAR` and `DATATYPE_MVAR` the chain
is, in order:

1. **Explicit `contextTypeSpec`** parameter — caller passes this when it
   knows the closed TypeSpec to use (e.g. CALLVIRT passes
   `stack->m_call.genericType`).
2. **`caller->genericType`** — the inferred context from the caller's
   stack frame.
3. **`caller->arrayElementType`** — runtime-inferred element type set by
   operations like `newarr T[]`, `ldelem.ref`, SZArrayHelper dispatch.
   Used to close the first VAR slot on helper objects whose closed
   context is inferred from a single runtime type.

For MVAR specifically there is an additional **MVAR → VAR → concrete**
two-level case used by the `constrained.` prefix on a generic method
inside a generic class (e.g. `constrained.!!T` inside `Equals<TValue>`
where `TValue` is itself a `VAR` in the enclosing `Dictionary<TKey,TValue>`).
The MVAR resolves through the caller's MethodSpec, lands on a VAR, and
then resolves through the caller's TypeSpec to the concrete type. **Nested
MVAR is not implemented** (only one level of MVAR chaining is supported);
an `_ASSERT` enforces this.

### Null fallback

For pre-allocation paths (e.g. `InitializeLocals` filling a struct array
for an open generic type) a VAR that cannot resolve falls back to a null
object reference. The reasoning: subsequent `stfld` instructions will
overwrite with the correct type once the slot is actually written. Value
types cannot be stubbed this way and fail outright.

---

## 6. `constrained.` prefix

ECMA-335 §III.2.1: when `constrained. T` precedes `callvirt` and `T` is
a *reference type*, the managed pointer on the stack must be dereferenced
to the actual object reference. For *value types* the BYREF is kept — the
callee receives it as `this`.

The implementation resolves the `constrained` token via
`CLR_RT_TypeDef_Instance::ResolveToken` and passes
`stack->m_call.genericType` as `contextTypeSpec`. This enables the
two-level MVAR → VAR → concrete resolution described in §5; without it,
`constrained.!!T` inside a generic method on a generic class will resolve
incorrectly.

---

## 7. `NEWOBJ` on generic types

Constructors are special: the callee's `genericType` from `ResolveToken`
often points at the *open* MethodRef TypeSpec (because the MethodRef
itself was emitted against the open generic). The caller's
`stack->m_call.genericType` is usually the *closed* TypeSpec we actually
want.

So `CEE_NEWOBJ` prefers the caller's context over the callee's own
`genericType`, opposite to the §4 priority for `CALLVIRT`. The callee
also inherits the caller's `methodSpec` when the constructor itself has
an open generic signature, for the same reason as §4.

---

## 8. BOX / UNBOX.ANY / LDOBJ / STELEM on generics

These all share a common pattern: when the opcode operates on a VAR (`!0`,
`!1`, etc.) inside an interface adapter (e.g. `IList<T>.set_Item`), the
adapter's `stack->m_call.genericType` may be *open*. To close the VAR slot
we use the runtime type of the value (for `BOX`/`UNBOX.ANY`) or the
runtime element type of the array (for `LDOBJ`/`STELEM_*`) to populate
`arrayElementType`, which then flows into the §5 resolution chain.

This mirrors what virtual dispatch does for `SZArrayHelper` calls
(`Array.get_Item<T>` and friends), where the array's reflection data
provides the element type. Same fix in three places; if you change one,
audit the others.

---

## 9. Static fields on generic types

`LDSFLD`/`STSFLD`/`LDSFLDA` on a generic type need both:
- **TypeSpec context** for VAR resolution (the closed `T` of `Foo<T>`).
- **MethodDef context** for MVAR resolution (the closed `!!U` of the
  enclosing generic method, if any).

Both are passed to the field-resolution helper. Missing either causes
field-typed `VAR`/`MVAR` slots to resolve as the open parameter instead
of the concrete type, which then breaks every subsequent read of that
field.

Field access on a generic type is also entangled with `.cctor` scheduling
(§10). If the field-resolution helper reports "not found" but the type has
a pending generic `.cctor`, the instruction reschedules and lets the
`.cctor` complete the field-table initialization first.

---

## 10. Generic `.cctor` lifecycle

Implemented in `ExecutionEngine_RunGenericStaticConstructors`
(`Execution.cpp`) and `SpawnStaticConstructor`.

- The crawler walks every assembly's `TypeSpec` table, filters to **closed**
  generic instantiations (open TypeSpecs have no observable static state),
  and schedules a `.cctor` for each one that has one.
- Each closed type is hashed (via `FindOrCreateGenericStaticFields`); the
  hash deduplicates so the same closed type is not initialized twice across
  multiple TypeSpec rows that happen to encode the same closed
  instantiation.
- The crawler is **resumable**: after a `.cctor` completes, the helper picks
  up at the next TypeSpec index. The current crawl position is stored on
  the spawned delegate via field repurposing — `m_genericTypeSpec.data != 0`
  on the delegate discriminates a generic-type `.cctor` from an ordinary
  one, and the field carries the TypeSpec index for resumption.
- The crawler runs lazily — generic `.cctor`s only fire when something
  forces them (typically the first field access via `LDSFLD`/`STSFLD`).
  This is why the field-access opcodes reschedule themselves when they
  observe a pending `.cctor` (§9).

---

## 11. Cross-assembly generic method resolution

`MethodRef_Instance::InitializeFromToken` handles the case where a generic
method's MethodRef points into an assembly other than the calling assembly.
The path:

1. Find a MethodRef in the target assembly matching the method by name.
2. Verify the MethodRef cross-reference points back to the original method
   (signature match).
3. If verification fails, keep the original method reference as a
   conservative fallback.

Same-assembly resolution is a fast path that doesn't need the rebind step.

### `m_typeSpecStorage` lifetime

`CLR_RT_MethodDef_Instance::genericType` is a *pointer*. If it pointed at
the `typeSpec` parameter on the caller's stack, that pointer would dangle
the moment `InitializeFromToken` returned. So the resolver copies the
TypeSpec into `m_typeSpecStorage` (member field of the instance) and points
`genericType` at the copy. **If you add a code path that sets `genericType`,
make sure the value it points at outlives the instance.**

---

## 12. AppDomain bootstrap

Mostly orthogonal to generics. Two facts worth knowing if you touch this:

- `AppDomainAssembly` is linked to the `AppDomain` *before* its static
  field allocation and `.cctor` scheduling complete. This is GC safety,
  not performance: a GC during allocation has to find the partially-built
  assembly via the AppDomain.
- AppDomain zombie cleanup is intentionally deferred. Full cleanup would
  require verifying that no managed driver, finalizable object, transparent
  proxy, or timer still references the dead domain. None of that
  infrastructure exists yet, so dead AppDomains are kept around as
  zombies. This is a known limitation.

The verbose explanations of these are still in `TypeSystem.cpp` because
they're not generics-related.

---

## 13. Build + debug workflow

### Build the netcore CLR DLL

```
MSBuild targets/netcore/nanoFramework.nanoCLR/nanoFramework.nanoCLR.vcxproj \
  -p:Configuration=Debug -p:Platform=x64
```

Produces `build/bin/Debug/nanoFramework.nanoCLR.dll`. The Win32 CLI
(`build/bin/Debug/net8.0/nanoFramework.nanoCLR.CLI.exe`) loads the native
DLL from its `NanoCLR/` subdirectory, so after rebuilding you must
**copy**:

```
cp build/bin/Debug/nanoFramework.nanoCLR.dll \
   build/bin/Debug/net8.0/NanoCLR/nanoFramework.nanoCLR.dll
```

(MSBuild does not auto-deploy. If the CLI exhibits stale behavior after a
rebuild, you forgot the copy.)

### Run the generics unit tests

The "run unit tests" launch profile of the CLI project (see
`targets/netcore/nanoFramework.nanoCLR.CLI/Properties/launchSettings.json`)
loads the test assemblies from
`E:/GitHub/nf-System.Collections/Tests/GenericCollections/bin/Debug/`:

```
mscorlib.pe
nanoFramework.System.Collections.pe
nanoFramework.TestFramework.pe
nanoFramework.UnitTestLauncher.pe
NFUnitTest.pe
```

The order matters — assemblies referenced later in the list see earlier
ones already loaded. Output lines starting with `Test passed,` or
`Test failed,` indicate per-test outcomes.

### Diagnostic instrumentation pattern

Use `CLR_Debug::Printf` gated on `VIRTUAL_DEVICE` for temporary trace
output. Pattern:

```cpp
#if defined(VIRTUAL_DEVICE)
    CLR_Debug::Printf("[TAG] field=%08x other=%d\r\n", a, b);
#endif
```

Choose a stable prefix tag (`[VMDDIAG]`, `[SIGDIAG]`) so you can filter
the noisy CLI output via grep. Strip the instrumentation before commit —
the codebase deliberately keeps verbose tracing out of release builds.

The full IL execution trace that some sessions reference comes from the
interpreter's tracing macros (already in `Interpreter.cpp` — search for
the `Execute_IL` tracing block). It's expensive and only useful when you
need step-by-step visibility into opcode dispatch.

---

## 14. Known failure-mode catalog

When you see one of these symptoms, this is where to look first.

**`CLR_E_WRONG_TYPE` on `callvirt`.**
- Walk the `FindVirtualMethodDef` 3-pass chain (§3). Most commonly the
  signature matcher silently fails on an explicit interface impl and the
  call falls through to an unrelated public method or to "not found."
- Check whether the call site is on a generic interface (`ICollection<KVP>`)
  whose concrete impl is the explicit-interface form on the closed
  generic class.

**Explicit interface impl shadowed by same-named public method.**
- The suffix-only pass in `FindVirtualMethodDef` exists exactly to prevent
  this. If a public method like `Dictionary.Remove(TKey)` is being called
  when `ICollection<KVP<,>>.Remove(KVP<,>)` should fire, the suffix-only
  match did not find the explicit impl. Almost always a signature-matcher
  bug. See §3.

**VAR unresolved at runtime.**
- The `arrayElementType` propagation chain (§5/§8) is broken somewhere
  upstream. Check whether the opcode that introduced the value into the
  current frame set `arrayElementType` correctly — usually `newarr`,
  `ldelem.ref`, `box`, or an SZArrayHelper dispatch.

**Open TypeSpec reaching a callee.**
- The §4 priority-2 closed-only gate is the first place to look. If a
  priority-2 result is being picked up when it shouldn't (because the
  callee's `genericType` is open), the gate is missing or wrong.

**Generic `.cctor` not firing or firing twice.**
- `FindOrCreateGenericStaticFields` hash mismatch — two TypeSpec rows
  encode the same closed type but hash differently, or vice versa. See §10.
- Alternatively, the resumption logic on `SpawnStaticConstructor` lost the
  TypeSpec index (`m_genericTypeSpec.data` zeroed prematurely).

**`Dictionary<TKey,TValue>` constructor from an `IDictionary<,>` crashes.**
- Specific instance of the suffix-only / signature-matcher bug. The
  `IEnumerable<KVP<,>>.GetEnumerator` explicit impl must be found by
  pass 2 of `FindVirtualMethodDef`. If `MatchSignatureForVirtualDispatch`
  mishandles the VAR↔GENERICINST drain (specifically for multi-argument
  generics like `KeyValuePair<,>`), this is the symptom.
