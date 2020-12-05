
using System.Diagnostics;
using System.Threading;

namespace System.Runtime.CompilerServices
{
    public struct AsyncVoidMethodBuilder
    {
        public static AsyncVoidMethodBuilder Create()
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:Create");
            return new AsyncVoidMethodBuilder();
        }

        public void AwaitOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
            where TAwaiter : INotifyCompletion
            where TStateMachine : IAsyncStateMachine
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:AwaitOnCompleted");
            var _stateMachine = stateMachine;
            awaiter.OnCompleted(() =>
            {
                Debug.WriteLine($"AsyncVoidMethodBuilder:OnCompleted");
                _stateMachine.MoveNext();
            });
        }

        public void AwaitUnsafeOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
            where TAwaiter : ICriticalNotifyCompletion
            where TStateMachine : IAsyncStateMachine
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:AwaitUnsafeOnCompleted");
            var _stateMachine = stateMachine;
            awaiter.OnCompleted(() =>
            {
                Debug.WriteLine($"AsyncVoidMethodBuilder:OnCompleted");
                _stateMachine.MoveNext();
            });
        }

        public void SetException(Exception exception)
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:SetException");
        }
        public void SetResult()
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:SetResult");
        }
        public void SetStateMachine(IAsyncStateMachine stateMachine)
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:SetStateMachine");
        }
        public void Start<TStateMachine>(ref TStateMachine stateMachine) where TStateMachine : IAsyncStateMachine
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:Start");
            stateMachine.MoveNext();
        }
    }

}
