.code

; Export the symbol
PUBLIC port_switch

; Function to perform a context switch on the two threads
; passed in as parameters. 
port_switch PROC

    push rbp
    push rsi
    push rdi
    push rbx
    push r12
    push r13
    push r14
    push r15
    mov rbx, rsp
    mov qword ptr [rdx + 24], rbx
    mov rsp, qword ptr [rcx + 24]
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rdi
    pop rsi
    pop rbp

    ret

port_switch ENDP
END