using System;

namespace System.Threading.Tasks
{
    public class Task
    {
        public TaskAwaiter GetAwaiter()
        {
            return new TaskAwaiter();
        }
    }

    public class Task<TResult> : Task
    {

    }
}
