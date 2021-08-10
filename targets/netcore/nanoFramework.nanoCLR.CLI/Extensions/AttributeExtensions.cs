//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Linq;
using System.Reflection;

namespace nanoFramework.nanoCLR.CLI
{
    public static class AttributeExtensions
    {
        /// <summary>
        /// A generic extension method to retrieve any attribute that is applied to an <see cref="Enum"/>.
        /// </summary>
        public static TAttribute GetAttribute<TAttribute>(this Enum enumValue)
                where TAttribute : Attribute
        {
            return enumValue.GetType()
                            .GetMember(enumValue.ToString())
                            .First()
                            .GetCustomAttribute<TAttribute>();
        }
    }
}
