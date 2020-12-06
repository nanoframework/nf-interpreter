using System;
using System.Diagnostics;

namespace System.Threading.Internal
{
    internal class ThreadWorker// : IDisposable
    {
        Thread thread;
        WaitCallback callback;
        object state;
        internal bool IsFree => thread == null || callback == null || thread.ThreadState == ThreadState.Suspended;
        internal int Id => thread != null ? thread.ManagedThreadId : -1;
        internal SynchronizationContext SynchronizationContext { get; private set; }
        static bool cantSuspend = true;//false;

        void Start()
        {
            thread = new Thread(() =>
            {
                while (true)
                {
                    Debug.WriteLine($"Thread {Id} started");
                    try
                    {
                        callback(state);
                        callback = null;
                        state = null;
                    }
                    catch { }
                    ThreadPool.RunPendingWorkItems(this);
                    //if more job was posted to this pool as a result of call to RunPendingWorkItems, continue the work immediately
                    if (callback != null)
                        continue;
                    Debug.WriteLine($"Thread {Id} exited");
                    if (cantSuspend)//if platform doesnt support suspend, use polling
                    {
                        while (callback == null)
                        {
                            Thread.Sleep(100);
                        }
                    }
                    else
                    {
                        try
                        {
                            Thread.CurrentThread.Suspend(); //if platform doesnt support suspend, use polling
                        }
                        catch
                        {
                            cantSuspend = true;
                            while (callback == null)
                            {
                                Thread.Sleep(100);
                            }
                        }
                    }
                }
            });
            SynchronizationContext = new NanoFrameworkSynchronizationContext(thread.ManagedThreadId);
            thread.Start();
        }

        public void Post(WaitCallback callback, object state)
        {
            this.state = state;
            this.callback = callback;
            if (thread == null)
            {
                Start();
            }
            else
            {
                if (!cantSuspend)
                    thread.Resume();
            }
        }

        //public void Dispose()
        //{
        //    thread?.Abort();
        //}
    }

}
