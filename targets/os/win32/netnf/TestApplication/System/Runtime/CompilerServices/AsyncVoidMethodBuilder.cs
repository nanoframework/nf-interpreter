
namespace System.Runtime.CompilerServices
{
    public struct AsyncVoidMethodBuilder
    {
        public static AsyncVoidMethodBuilder Create()
        {
            return new AsyncVoidMethodBuilder();
        }

        void AwaitOnCompletedInternal(object awaiter, object stateMachine)
        {
            //awaiter.OnCompleted(() =>
            //{
            //    stateMachine.MoveNext();
            //});
            ////GetResult will suspend the calling thread, start a new one
            ///*new Threading.Thread(() =>*/ stateMachine.MoveNext()/*)*/;
        }

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
            stateMachine.MoveNext();
        }
    }

}
