using System;
using System.Collections;
using System.Threading.Internal;

namespace System.Threading
{
    public static class ThreadPool
    {
        const int Workers = 12;
        const int WorkItems = 13;
        //using fixed array for performance
        static CircularQueue<ThreadWorker> workers = new CircularQueue<ThreadWorker>(Workers);
        static CircularQueue<WorkItem> pendingWorks = new CircularQueue<WorkItem>(WorkItems);

        static object mlock = new object();
        static internal void RunPendingWorkItems(ThreadWorker callingWorker)
        {
            lock (mlock)
            {
                //first find the first workitem that was requested to be run on this calling pool and start it
                for (int i = 0; i < pendingWorks.Count; i++)
                {
                    WorkItem work = pendingWorks[i]; //TODO: remove this workitem from queue
                    if (work.workerId == callingWorker.Id) //if the work must be run on this thread
                    {
                        callingWorker.Post(work.callBack, work.state);
                    }
                }
                //now run all other pending works on any available pool
                while (pendingWorks.Count > 0)
                {
                    //get an available pool
                    ThreadWorker pool = GetOrCreateFreeWorker();
                    if (pool != null)
                    {
                        do
                        {
                            WorkItem work = default;
                            if (pendingWorks.Get(ref work))
                            {
                                if (work.workerId < 0) //if the work can be run on any thread
                                {
                                    pool.Post(work.callBack, work.state);
                                }
                                else
                                {
                                    continue;
                                }
                            }
                        } while (false);
                    }
                }
            }
        }

        static ThreadWorker GetWorkerById(int id)
        {
            ThreadWorker pool = null;
            for (int i = 0; i < workers.Count; i++)
            {
                pool = workers[i];
                if (pool.Id == id)
                    return pool;
            }
            return null;
        }

        static ThreadWorker GetFreeWorker()
        {
            ThreadWorker pool = null;
            for (int i = 0; i < workers.Count; i++)
            {
                pool = workers[i];
                if (pool.IsFree)
                    return pool;
            }
            return null;
        }

        static ThreadWorker GetOrCreateFreeWorker()
        {
            ThreadWorker pool = GetFreeWorker();
            if (pool == null)
            {
                pool = new ThreadWorker();
                workers.Put(pool);
            }
            return pool;
        }

        //
        // Summary:
        //     Gets the number of thread pool threads that currently exist.
        //
        // Returns:
        //     The number of thread pool threads that currently exist.
        public static int ThreadCount => workers.Count;
        //
        // Summary:
        //     Gets the number of work items that have been processed so far.
        //
        // Returns:
        //     The number of work items that have been processed so far.
        public static long CompletedWorkItemCount { get; private set; }
        //
        // Summary:
        //     Gets the number of work items that are currently queued to be processed.
        //
        // Returns:
        //     The number of work items that are currently queued to be processed.
        public static long PendingWorkItemCount => pendingWorks.Count;
        //
        // Summary:
        //     Retrieves the difference between the maximum number of thread pool threads returned
        //     by the System.Threading.ThreadPool.GetMaxThreads(System.Int32@,System.Int32@)
        //     method, and the number currently active.
        //
        // Parameters:
        //   workerThreads:
        //     The number of available worker threads.
        //
        //   completionPortThreads:
        //     The number of available asynchronous I/O threads.
        public static void GetAvailableThreads(out int workerThreads, out int completionPortThreads)
        {
            workerThreads = workers.Space;
            completionPortThreads = -1;
        }
        //
        // Summary:
        //     Retrieves the number of requests to the thread pool that can be active concurrently.
        //     All requests above that number remain queued until thread pool threads become
        //     available.
        //
        // Parameters:
        //   workerThreads:
        //     The maximum number of worker threads in the thread pool.
        //
        //   completionPortThreads:
        //     The maximum number of asynchronous I/O threads in the thread pool.
        public static void GetMaxThreads(out int workerThreads, out int completionPortThreads)
        {
            workerThreads = Workers;
            completionPortThreads = -1;
        }
        //
        // Summary:
        //     Retrieves the minimum number of threads the thread pool creates on demand, as
        //     new requests are made, before switching to an algorithm for managing thread creation
        //     and destruction.
        //
        // Parameters:
        //   workerThreads:
        //     When this method returns, contains the minimum number of worker threads that
        //     the thread pool creates on demand.
        //
        //   completionPortThreads:
        //     When this method returns, contains the minimum number of asynchronous I/O threads
        //     that the thread pool creates on demand.
        public static void GetMinThreads(out int workerThreads, out int completionPortThreads)
        {
            workerThreads = 0;
            completionPortThreads = -1;
        }
        //
        // Summary:
        //     Queues a method for execution. The method executes when a thread pool thread
        //     becomes available.
        //
        // Parameters:
        //   callBack:
        //     A System.Threading.WaitCallback that represents the method to be executed.
        //
        // Returns:
        //     true if the method is successfully queued; System.NotSupportedException is thrown
        //     if the work item could not be queued.
        //
        // Exceptions:
        //   T:System.ArgumentNullException:
        //     callBack is null.
        //
        //   T:System.NotSupportedException:
        //     The common language runtime (CLR) is hosted, and the host does not support this
        //     action.
        public static bool QueueUserWorkItem(WaitCallback callBack)
        {
            ThreadWorker pool = GetOrCreateFreeWorker();
            if (pool != null)
            {
                pool.Post(callBack, null);
                return true;
            }
            return pendingWorks.Put(new WorkItem(callBack, null));
        }
        //
        // Summary:
        //     Queues a method for execution, and specifies an object containing data to be
        //     used by the method. The method executes when a thread pool thread becomes available.
        //
        // Parameters:
        //   callBack:
        //     A System.Threading.WaitCallback representing the method to execute.
        //
        //   state:
        //     An object containing data to be used by the method.
        //
        // Returns:
        //     true if the method is successfully queued; System.NotSupportedException is thrown
        //     if the work item could not be queued.
        //
        // Exceptions:
        //   T:System.NotSupportedException:
        //     The common language runtime (CLR) is hosted, and the host does not support this
        //     action.
        //
        //   T:System.ArgumentNullException:
        //     callBack is null.
        public static bool QueueUserWorkItem(WaitCallback callBack, object state)
        {
            ThreadWorker pool = GetOrCreateFreeWorker();
            if (pool != null)
            {
                pool.Post(callBack, state);
                return true;
            }
            return pendingWorks.Put(new WorkItem(callBack, state));
        }
        //
        // Summary:
        //     Queues a method specified by an System.Action`1 delegate for execution, and provides
        //     data to be used by the method. The method executes when a thread pool thread
        //     becomes available.
        //
        // Parameters:
        //   callBack:
        //     An System.Action`1 representing the method to execute.
        //
        //   state:
        //     An object containing data to be used by the method.
        //
        //   preferLocal:
        //     true to prefer queueing the work item in a queue close to the current thread;
        //     false to prefer queueing the work item to the thread pool's shared queue.
        //
        // Type parameters:
        //   TState:
        //     The type of elements of state.
        //
        // Returns:
        //     true if the method is successfully queued; System.NotSupportedException is thrown
        //     if the work item could not be queued. .
        public static bool QueueUserWorkItem<TState>(Action<TState> callBack, TState state, bool preferLocal)
        {
            if (preferLocal)
                throw new Exception("PreferLocal:true not supported");
            ThreadWorker pool = GetOrCreateFreeWorker();
            if (pool != null)
            {
                pool.Post((_) => callBack(state), null);
                return true;
            }
            return pendingWorks.Put(new WorkItem((_) => callBack(state), state));
        }

        internal static bool QueueUserWorkItemOnSpecificWorker(int threadId, WaitCallback callBack, object state)
        {
            ThreadWorker pool = GetWorkerById(threadId);
            if (pool == null)
                throw new Exception($"No such worker with id {threadId}");
            if (pool.IsFree)
            {
                pool.Post(callBack, state);
                return true;
            }
            else
            {
                return pendingWorks.Put(new WorkItem((_) => callBack(state), state, threadId));
            }
        }

        internal static ThreadWorker Current => GetWorkerById(Thread.CurrentThread.ManagedThreadId);

        //
        // Summary:
        //     Sets the number of requests to the thread pool that can be active concurrently.
        //     All requests above that number remain queued until thread pool threads become
        //     available.
        //
        // Parameters:
        //   workerThreads:
        //     The maximum number of worker threads in the thread pool.
        //
        //   completionPortThreads:
        //     The maximum number of asynchronous I/O threads in the thread pool.
        //
        // Returns:
        //     true if the change is successful; otherwise, false.
        public static bool SetMaxThreads(int workerThreads, int completionPortThreads)
        {
            return false;
        }
        //
        // Summary:
        //     Sets the minimum number of threads the thread pool creates on demand, as new
        //     requests are made, before switching to an algorithm for managing thread creation
        //     and destruction.
        //
        // Parameters:
        //   workerThreads:
        //     The minimum number of worker threads that the thread pool creates on demand.
        //
        //   completionPortThreads:
        //     The minimum number of asynchronous I/O threads that the thread pool creates on
        //     demand.
        //
        // Returns:
        //     true if the change is successful; otherwise, false.
        public static bool SetMinThreads(int workerThreads, int completionPortThreads)
        {
            return false;
        }
    }
}
