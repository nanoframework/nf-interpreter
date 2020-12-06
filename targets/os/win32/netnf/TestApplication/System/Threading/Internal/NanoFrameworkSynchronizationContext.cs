using System;

namespace System.Threading.Internal
{
    public class NanoFrameworkSynchronizationContext:SynchronizationContext
    {
        int workerId;

        public NanoFrameworkSynchronizationContext(int workerId)
        {
            this.workerId = workerId;
        }

        public override void Post(SendOrPostCallback d, object state)
        {
            ThreadPool.QueueUserWorkItemOnSpecificWorker(workerId, (s) => d(s), state);
        }

        //public override void Send(SendOrPostCallback d, object state)
        //{
        //    ThreadPool.QueueUserWorkItemOnSpecificThread(threadId, (s) => d(s), state);
        //}
    }
}
