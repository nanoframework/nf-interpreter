using System;
using System.Collections;
using System.Diagnostics;

namespace System.Threading.Tasks
{
    public class Task
    {
        public static Task<TResult> FromEvent<TResult>(EventHandler<TResult> callback)
        {
            var tcs = new TaskCompletionSource<TResult>();
            callback += (s, e) => tcs.SetResult(e);
            return tcs.Task;
        }

        public static Task Run(Action action)
        {
            return new Task(action);
        }

        public static Task<TResult> Run<TResult>(Func<TResult> action)
        {
            return new Task<TResult>(action);
        }

        public static Task Delay(int millSeconds)
        {
            return new Task(() => Thread.Sleep(millSeconds));
        }

        public static Task CompletedTask
        {
            get
            {
                var task = new Task();
                task.Complete();
                return task;
            }
        }

        public static Task FromException(Exception e)
        {
            var task = new Task();
            task.CompleteWithException(e);
            return task;
        }

        public static Task<TResult> FromException<TResult>(Exception e)
        {
            var task = new Task<TResult>();
            task.CompleteWithException(e);
            return task;
        }

        public static Task FromCanceled()
        {
            var task = new Task();
            task.CompleteWithException(new TaskCanceledException());
            return task;
        }

        public static Task<TResult> FromCanceled<TResult>()
        {
            var task = new Task<TResult>();
            task.CompleteWithException(new TaskCanceledException());
            return task;
        }

        TaskAwaiter awaiter;
        ArrayList _continuations = new ArrayList();
        public bool IsCompleted { get; protected set; }
        public Exception Exception { get; protected set; }

        public Task()
        {
            awaiter = new TaskAwaiter(this);
        }

        public Task(Action action):this()
        {
            new Thread(() =>
            {
                //try
                {
                    action();
                    Debug.WriteLine("Calling Complete");
                    Complete();
                    Debug.WriteLine("Called Complete");
                }
                //catch (Exception e)
                //{
                //    awaiter.CompleteWithException(e);
                //}
            }).Start();
        }

        protected void RunContinuations()
        {
            Debug.WriteLine("Continuations");
            foreach (var c in _continuations)
            {
                var _continue = (Action)c;
                Debug.WriteLine("Continue");
                _continue();
            }
            Debug.WriteLine("Continuations OK");
            _continuations.Clear();
        }

        public TaskAwaiter GetAwaiter()
        {
            return awaiter;
        }

        internal void OnCompleted(Action continuation)
        {
            if (IsCompleted)
                continuation();
            else
                _continuations.Add(continuation);
        }

        //public Task ContinueWith(Action continuation)
        //{
        //    var task = new Task(continuation);
        //    return task;
        //}

        internal void Complete()
        {
            Debug.WriteLine("Completing");
            IsCompleted = true;
            RunContinuations();
        }

        protected internal void CompleteWithException(Exception e)
        {
            Exception = e;
            IsCompleted = true;
            RunContinuations();
        }

        public void GetResult()
        {
            while (!IsCompleted) ;
            if (Exception != null)
                throw Exception;
        }
    }

    public class Task<TResult> : Task
    {
        TaskAwaiter<TResult> awaiter;
        TResult result;
        public TResult Result
        {
            get
            {
                while (!IsCompleted) ;
                if (Exception != null)
                    throw Exception;
                return result;
            }
            private set
            {
                IsCompleted = true;
                result = value;
            }
        }

        public Task()
        {
            awaiter = new TaskAwaiter<TResult>(this);
        }

        public Task(Func<TResult> action):this()
        {
            new Thread(() =>
            {
                try
                {
                    var result = action();
                    Complete(result);
                }catch(Exception e)
                {
                    CompleteWithException(e);
                }
            }).Start();
        }

        internal void OnCompleted(Action<TResult> continuation)
        {
            OnCompleted(() => continuation(GetResult()));
        }

        public new TResult GetResult()
        {
            return Result;
        }

        internal void Complete(TResult result)
        {
            IsCompleted = true;
            Result = result;
            RunContinuations();
        }

        public new TaskAwaiter<TResult> GetAwaiter()
        {
            return awaiter;
        }
    }
}
