
using System.Diagnostics;
using System.Threading;

namespace System.Runtime.CompilerServices
{
    public struct AsyncVoidMethodBuilder
    {
        public static AsyncVoidMethodBuilder Create()
        {
            return new AsyncVoidMethodBuilder();
        }

        public void AwaitOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
            where TAwaiter : INotifyCompletion
            where TStateMachine : IAsyncStateMachine
        {
            Debug.WriteLine($"AwaitOnCompleted1");
            var _stateMachine = stateMachine;
            Debug.WriteLine("AwaitOnCompleted2");
            awaiter.OnCompleted(() =>
            {
                Debug.WriteLine("On completed");
                _stateMachine.MoveNext();
                Debug.WriteLine("On completed end");
            });
        }

        public void AwaitUnsafeOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
            where TAwaiter : ICriticalNotifyCompletion
            where TStateMachine : IAsyncStateMachine
        {
            var _stateMachine = stateMachine;
            awaiter.OnCompleted(() =>
            {
                _stateMachine.MoveNext();
            });
        }

        public void SetException(Exception exception)
        {

        }
        public void SetResult()
        {

        }
        public void SetStateMachine(IAsyncStateMachine stateMachine)
        {

        }
        public void Start<TStateMachine>(ref TStateMachine stateMachine) where TStateMachine : IAsyncStateMachine
        {
            Debug.WriteLine($"AsynVoid:Start:{stateMachine.GetHashCode()}");
            stateMachine.MoveNext();
        }
    }

}
