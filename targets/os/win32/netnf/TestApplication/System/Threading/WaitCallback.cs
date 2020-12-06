using System;

namespace System.Threading
{
    //
    // Summary:
    //     Represents a callback method to be executed by a thread pool thread.
    //
    // Parameters:
    //   state:
    //     An object containing information to be used by the callback method.
    public delegate void WaitCallback(object state);
}
