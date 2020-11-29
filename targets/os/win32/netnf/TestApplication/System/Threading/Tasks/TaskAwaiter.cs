using System;
using System.Runtime.CompilerServices;

namespace System.Threading.Tasks
{
    public struct TaskAwaiter : INotifyCompletion
    {
        readonly bool isCompleted;
        public bool IsCompleted => isCompleted;
        Thread getResultThread;
        public void GetResult()
        {
            //if (isCompleted)
            //    return;
            //getResultThread = Thread.CurrentThread;
            //getResultThread.Suspend();
        }
        Action _continuation;
        public void OnCompleted(Action continuation)
        {
            _continuation = continuation;
        }
        public void UnsafeOnCompleted(Action continuation)
        {
            _continuation = continuation;
        }
    }

    public readonly struct TaskAwaiter<TResult> : ICriticalNotifyCompletion, INotifyCompletion
    {
        public bool IsCompleted { get; }
        public TResult GetResult()
        {
            return default;
        }
        public void OnCompleted(Action continuation)
        {

        }
        public void UnsafeOnCompleted(Action continuation)
        {

        }
    }

}
