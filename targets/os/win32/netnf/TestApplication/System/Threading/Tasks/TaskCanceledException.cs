using System;

namespace System.Threading.Tasks
{
    public class TaskCanceledException : Exception
    {
        public TaskCanceledException()
        {
        }

        public TaskCanceledException(string message) : base(message)
        {
        }

        public TaskCanceledException(string message, Exception innerException) : base(message, innerException)
        {
        }
    }
}
