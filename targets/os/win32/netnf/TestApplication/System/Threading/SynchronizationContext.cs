using System;

namespace System.Threading
{
    public class SynchronizationContext
    {
        public SynchronizationContext()
        {

        }

        static readonly SynchronizationContext defaultSyncronizationContext = new SynchronizationContext();
        //
        // Summary:
        //     Gets the synchronization context for the current thread.
        //
        // Returns:
        //     A System.Threading.SynchronizationContext object representing the current synchronization
        //     context.
        public static SynchronizationContext Current
        {
            get
            {
                var context = ThreadPool.Current?.SynchronizationContext;
                if (context == null)
                    context = defaultSyncronizationContext;
                return context;
            }
        }

        //
        // Summary:
        //     Sets the current synchronization context.
        //
        // Parameters:
        //   syncContext:
        //     The System.Threading.SynchronizationContext object to be set.
        public static void SetSynchronizationContext(SynchronizationContext syncContext)
        {
            throw new Exception("Can't set SynchronizationContext on this platform");
        }
        //
        // Summary:
        //     When overridden in a derived class, creates a copy of the synchronization context.
        //
        // Returns:
        //     A new System.Threading.SynchronizationContext object.
        public virtual SynchronizationContext CreateCopy() { return new SynchronizationContext(); }
        //
        // Summary:
        //     Determines if wait notification is required.
        //
        // Returns:
        //     true if wait notification is required; otherwise, false.
        bool waitNotification;
        public bool IsWaitNotificationRequired() { return waitNotification; }
        //
        // Summary:
        //     When overridden in a derived class, responds to the notification that an operation
        //     has completed.
        public virtual void OperationCompleted() { }
        //
        // Summary:
        //     When overridden in a derived class, responds to the notification that an operation
        //     has started.
        public virtual void OperationStarted() { }
        //
        // Summary:
        //     When overridden in a derived class, dispatches an asynchronous message to a synchronization
        //     context.
        //
        // Parameters:
        //   d:
        //     The System.Threading.SendOrPostCallback delegate to call.
        //
        //   state:
        //     The object passed to the delegate.
        public virtual void Post(SendOrPostCallback d, object state)
        {
            ThreadPool.QueueUserWorkItem(new WaitCallback(d), state);
        }
        //
        // Summary:
        //     When overridden in a derived class, dispatches a synchronous message to a synchronization
        //     context.
        //
        // Parameters:
        //   d:
        //     The System.Threading.SendOrPostCallback delegate to call.
        //
        //   state:
        //     The object passed to the delegate.
        //
        public virtual void Send(SendOrPostCallback d, object state) { d(state); }
        //
        // Summary:
        //     Sets notification that wait notification is required and prepares the callback
        //     method so it can be called more reliably when a wait occurs.
        protected void SetWaitNotificationRequired() { waitNotification = true; }
    }
}
