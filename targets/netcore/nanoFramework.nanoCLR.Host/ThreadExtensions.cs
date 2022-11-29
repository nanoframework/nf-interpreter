//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;

namespace nanoFramework.nanoCLR.Host
{
    internal static class ThreadExtensions
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
