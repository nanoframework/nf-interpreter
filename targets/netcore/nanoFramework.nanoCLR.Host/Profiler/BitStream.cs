// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.IO;

namespace nanoFramework.Tools.NanoProfiler.Packets
{
    // developer note: this class was taken from the nanoFramework.Tools.Debugger project and simplified
    internal class BitStream
    {
        class Buffer
        {
            const int c_BufferSize = 2048;

            internal Buffer _next = null;
            internal byte[] _data;
            internal int _length;
            internal int _avail;   //Bits available at last position.

            internal Buffer()
            {
                _data = new byte[c_BufferSize];
                _length = 0;
            }

            internal Buffer(
                byte[] data,
                int pos,
                int len,
                int bitsInLastPos)
            {
                if (bitsInLastPos < 1 || bitsInLastPos > 8)
                {
                    throw new ArgumentException("bits");
                }

                _data = new byte[len];
                _avail = bitsInLastPos;

                // can't copy more than what's available in the source array!
                if (len > data.Length - pos)
                {
                    // NOT enough elements, so adjust length to copy
                    _length = data.Length - pos;
                }
                else
                {
                    // all good
                    _length = len;
                }

                Array.Copy(
                    data,
                    pos,
                    _data,
                    0,
                    _length);
            }
        }

        Buffer _first;
        Buffer _current;
        Buffer _last;
        int _pos;
        int _avail;

        bool _streamEnded;

        public BitStream()
        {
            _first = new Buffer();
            _last = _first;
            _streamEnded = false;

            Rewind();
        }

        public BitStream(
            byte[] data,
            int pos,
            int len) : this()
        {
            if (data == null)
            {
                throw new ArgumentNullException(nameof(data));
            }
            AppendChunk(data, pos, len * 8);
        }

        public void MarkStreamEnd()
        {
            _streamEnded = true;
        }

        public void AppendChunk(
            byte[] data,
            int pos,
            int bitlen)
        {
            if (bitlen > 0)
            {
                int len = bitlen / 8;
                int bitsInLast = bitlen % 8;
                if (bitsInLast == 0)
                {
                    bitsInLast = 8;
                }
                else
                {
                    len++;
                }

                Buffer next = new Buffer(data, pos, len, bitsInLast);

                if (_last == null)
                {
                    _first = _last = _current = next;
                    Rewind();
                }
                else
                {
                    _last._next = next;
                    _last = next;
                }
            }
        }

        public void Rewind()
        {
            _current = _first;
            _pos = -1;
            _avail = 0;
        }

        public int BitsAvailable
        {
            get
            {
                int val;

                Buffer ptr = _current;
                val = 8 * (ptr._length - _pos) + _avail - 8;

                while (ptr._next != null)
                {
                    ptr = ptr._next;

                    val += 8 * (ptr._length - 1) + ptr._avail;
                }

                return val;
            }
        }

        public uint ReadBits(int bits)
        {
            if (bits > 32) throw new ArgumentException("Max number of bits per read is 32");

            uint val = 0;
            int pos = bits;
            int bitsOrig = bits;

            while (bits > 0)
            {
                while (_avail == 0)
                {
                    _pos++;

                    while (_pos >= _current._length)
                    {
                        if (_current._next == null)
                        {
                                throw new EndOfStreamException();
                        }
                        else
                        {
                            _current = _current._next;
                            _pos = 0;
                        }
                    }

                    if (_pos < _current._length - 1)
                    {
                        _avail = 8;
                    }
                    else
                    {
                        _avail = _current._avail;
                    }
                }

                int insert = Math.Min(bits, _avail);
                uint mask = ((1U << insert) - 1U);
                int shift = _avail - insert;

                if (_pos == _current._length - 1)
                {
                    shift += 8 - _current._avail;
                }

                pos -= insert; val |= (((uint)_current._data[_pos] >> shift) & mask) << pos;
                bits -= insert;
                _avail -= insert;
            }

            return val;
        }
    }
}
