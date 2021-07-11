using System;

namespace nanoFramework.nanoCLR.Host
{
    public static class ThreadExtensions
    {
        public static T Sync<T>(this object syncRoot, Func<T> func)
        {
            lock (syncRoot)
            {
                return func();
            }
        }

        public static void Sync(this object syncRoot, Action action)
        {
            lock (syncRoot)
            {
                action();
            }
        }
    }
}
