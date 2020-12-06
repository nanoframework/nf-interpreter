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

        public static Task<TResult> FromResult<TResult>(TResult result)
        {
            var task = new Task<TResult>();
            task.Complete(result);
            Debug.WriteLine("FromResult: Complete");
            return task;
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
        public bool IsCompletedSuccessfully => IsCompleted && Exception == null;

        public Task()
        {
            Debug.WriteLine("Task:ctor()");
            awaiter = new TaskAwaiter(this);
        }

        public Task(Action action):this()
        {
            var syncContext = SynchronizationContext.Current;
            ThreadPool.QueueUserWorkItem((_) =>
            {
                try
                {
                    action();
                    Debug.WriteLine("Calling Complete");
                    syncContext.Post((__) => Complete(), null);
                    Debug.WriteLine("Called Complete");
                }
                catch (Exception e)
                {
                    syncContext.Post((ex) => CompleteWithException((Exception)ex), e);
                }
            });
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
            _continuations.Clear();
            Debug.WriteLine("Continuations OK");
        }

        public TaskAwaiter GetAwaiter()
        {
            return awaiter;
        }

        public Task ContinueWith(Action continuation)
        {
            Task task = new Task();
            task.OnCompleted(continuation);
            OnCompleted(() =>
            {
                task.Complete();
            });
            return task;
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
            Debug.WriteLine("Task<T>:ctor()");
            awaiter = new TaskAwaiter<TResult>(this);
        }

        public Task(Func<TResult> action):this()
        {
            Debug.WriteLine("Task<T>:ctor(action)");
            var syncContext = SynchronizationContext.Current;
            ThreadPool.QueueUserWorkItem((_) =>
            {
                Debug.WriteLine("Task<T>:Start");
                try
                {
                    var result = action();
                    syncContext.Post((r) => Complete((TResult)r), result);
                }
                catch (Exception e)
                {
                    syncContext.Post((ex) => CompleteWithException((Exception)ex), e);
                }
                Debug.WriteLine("Task<T>:End");
            });
            Debug.WriteLine("Task<T>:ctor(action):ends");
        }

        public Task<TNewResult> ContinueWith<TNewResult>(Func<TResult, TNewResult> continuation)
        {
            Task<TNewResult> task = new Task<TNewResult>();
            OnCompleted((result) =>
            {
                task.Complete(continuation(result));
            });
            return task;
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
            Debug.WriteLine("Complete Dome");
        }

        public new TaskAwaiter<TResult> GetAwaiter()
        {
            return awaiter;
        }
    }
}
