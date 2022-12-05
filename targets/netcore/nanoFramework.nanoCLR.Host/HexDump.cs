////
// Copyright (c) Drew Noakes. All Rights Reserved. Licensed under the Apache License, Version 2.0. See LICENSE in the project root for license information.
////

using System;
using System.Diagnostics.Contracts;
using System.Text;

namespace HexDump
{
    internal static class HexDump
    {
        [Pure]
        public static string Format(byte[] bytes, int columnWidth = 8, int columnCount = 2, bool includeOffset = true, bool includeAscii = true)
        {
            if (bytes == null)
            {
                throw new ArgumentNullException(nameof(bytes));
            }

            if (bytes.Length == 0)
            {
                return string.Empty;
            }

            var str = new StringBuilder();

            var i = 0;

            while (i < bytes.Length)
            {
                var baseIndex = i;

                if (i != 0)
                {
                    str.AppendLine();
                }

                // OFFSET
                if (includeOffset)
                {
                    str.Append($"{baseIndex:X4}    ");
                }

                // BYTES
                for (var columnIndex = 0; columnIndex < columnCount; columnIndex++)
                {
                    // space between columns
                    if (columnIndex != 0)
                    {
                        str.Append("  ");
                    }

                    for (var j = 0; j < columnWidth; j++)
                    {
                        if (j != 0)
                        {
                            str.Append(' ');
                        }

                        str.Append(i < bytes.Length
                            ? bytes[i].ToString("X2")
                            : "  ");
                        i++;
                    }
                }

                // ASCII
                if (includeAscii)
                {
                    str.Append("    ");

                    i = baseIndex;
                    for (var columnIndex = 0; columnIndex < columnCount; columnIndex++)
                    {
                        // space between columns
                        if (columnIndex != 0)
                        {
                            str.Append(' ');
                        }

                        for (var j = 0; j < columnWidth; j++)
                        {
                            if (i < bytes.Length)
                            {
                                var c = (char)bytes[i];
                                str.Append(char.IsControl(c) ? '.' : c);
                            }
                            else
                            {
                                str.Append(' ');
                            }

                            i++;
                        }
                    }
                }
            }

            return str.ToString();
        }
    }
}
