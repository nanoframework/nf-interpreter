using System;

namespace System.Threading.Tasks
{
    public class TaskCompletionSource<TResult>
    {
        public TaskCompletionSource()
        {
            Task = new Task<TResult>();
        }

        public void SetResult(TResult result)
        {
            Task.Complete(result);
        }

        public void SetCanceled()
        {
            Task.CompleteWithException(new TaskCanceledException());
        }

        public void SetException(Exception e)
        {
            Task.CompleteWithException(e);
        }

        public Task<TResult> Task { get; }
    }
}
