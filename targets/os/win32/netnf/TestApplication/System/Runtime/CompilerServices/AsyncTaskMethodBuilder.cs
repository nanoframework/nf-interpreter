
using System.Threading.Tasks;

namespace System.Runtime.CompilerServices
{
    public struct AsyncTaskMethodBuilder<TResult>
    {
        public static AsyncTaskMethodBuilder<TResult> Create()
        {
            return new AsyncTaskMethodBuilder<TResult>();
        }

        Task<TResult> task;
        public Task<TResult> Task => task;
        public void AwaitOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
            where TAwaiter : INotifyCompletion
            where TStateMachine : IAsyncStateMachine
        {
            var _stateMachine = stateMachine;
            awaiter.OnCompleted(() =>
            {
                _stateMachine.MoveNext();
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
            task?.CompleteWithException(exception);
        }
        public void SetResult(TResult result)
        {
            task?.Complete(result);
        }

        public void SetStateMachine(IAsyncStateMachine stateMachine)
        {

        }
        public void Start<TStateMachine>(ref TStateMachine stateMachine) where TStateMachine : IAsyncStateMachine
        {
            task = new Task<TResult>();
            stateMachine.MoveNext();
        }
    }
}
