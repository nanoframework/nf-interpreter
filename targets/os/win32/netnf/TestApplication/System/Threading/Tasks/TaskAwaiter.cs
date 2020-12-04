using System;
using System.Runtime.CompilerServices;

namespace System.Threading.Tasks
{
    public readonly struct TaskAwaiter : INotifyCompletion
    {
        //readonly bool _isCompleted;
        //readonly Exception _exception;
        //readonly Action _continuation;
        readonly Task _task;

        public TaskAwaiter(Task task)
        {
            _task = task;
        }

        public bool IsCompleted => _task.IsCompleted;

        public void GetResult()
        {
            _task.GetResult();
        }

        public void OnCompleted(Action continuation)
        {
            _task.OnCompleted(continuation);
        }

        public void UnsafeOnCompleted(Action continuation)
        {
            _task.OnCompleted(continuation);
        }

        //public void CompleteWithException(Exception e)
        //{
        //    _exception = e;
        //    _isCompleted = true;
        //    _continuation?.Invoke();
        //}

        //public void Complete()
        //{
        //    _isCompleted = true;
        //    _continuation?.Invoke();
        //}
    }

    public readonly struct TaskAwaiter<TResult> : ICriticalNotifyCompletion, INotifyCompletion
    {
        readonly Task<TResult> _task;

        public TaskAwaiter(Task<TResult> task)
        {
            _task = task;

        }
        public bool IsCompleted => _task.IsCompleted;

        public TResult GetResult()
        {
            return _task.GetResult();
        }

        public void OnCompleted(Action continuation)
        {
            _task.OnCompleted(continuation);
        }

        public void UnsafeOnCompleted(Action continuation)
        {
            _task.OnCompleted(continuation);
        }

        //public void CompleteWithException(Exception e)
        //{
        //    _exception = e;
        //    _isCompleted = true;
        //    _continuation?.Invoke();
        //}

        //public void Complete(TResult result)
        //{
        //    _result = result;
        //    _isCompleted = true;
        //    _continuation?.Invoke();
        //}
    }

}
