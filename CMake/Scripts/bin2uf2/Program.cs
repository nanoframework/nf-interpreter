//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
// Converts binary firmware images to UF2 format for RP2040/RP2350 targets.
// UF2 specification: https://github.com/microsoft/uf2
//
// Usage:
//   Single file:  bin2uf2 <input.bin> <base_address> <family_id> <output.uf2>
//   Merge files:  bin2uf2 --merge <output.uf2> <family_id> <file1> <addr1> [<file2> <addr2> ...]
//
// Family IDs:
//   RP2040:          0xe48bff56
//   RP2350 (ARM):    0xe48bff59
//   RP2350 (RISC-V): 0xe48bff5a

using System.Buffers.Binary;

const uint UF2_MAGIC_START0 = 0x0A324655; // "UF2\n"
const uint UF2_MAGIC_START1 = 0x9E5D5157;
const uint UF2_MAGIC_END    = 0x0AB16F30;
const uint UF2_FLAG_FAMILY  = 0x00002000;
const int  UF2_DATA_SIZE    = 256;

if (args.Length >= 5 && args[0] == "--merge")
{
    // --merge <output.uf2> <family_id> <file1> <addr1> [<file2> <addr2> ...]
    string outputPath = args[1];
    uint familyId = ParseHex(args[2]);
    var pairs = args.AsSpan(3);

    if (pairs.Length % 2 != 0)
    {
        Console.Error.WriteLine("Error: --merge requires pairs of <file> <address>");
        return 1;
    }

    var allBlocks = new List<(uint addr, byte[] data, int len)>();

    for (int i = 0; i < pairs.Length; i += 2)
    {
        byte[] bin = File.ReadAllBytes(pairs[i]);
        uint baseAddr = ParseHex(pairs[i + 1]);

        // Pad gap between previous segment and this one with 0xFF
        if (allBlocks.Count > 0)
        {
            var last = allBlocks[^1];
            uint nextExpected = last.addr + UF2_DATA_SIZE;

            if (baseAddr > nextExpected)
            {
                uint gapSize = baseAddr - nextExpected;
                byte[] pad = new byte[UF2_DATA_SIZE];
                Array.Fill(pad, (byte)0xFF);

                for (uint off = 0; off < gapSize; off += UF2_DATA_SIZE)
                {
                    allBlocks.Add((nextExpected + off, (byte[])pad.Clone(), UF2_DATA_SIZE));
                }

                Console.WriteLine($"  Padded {gapSize} byte gap (0x{nextExpected:X8} -> 0x{baseAddr:X8})");
            }
        }

        AddBlocks(allBlocks, bin, baseAddr);
    }

    WriteUf2(outputPath, allBlocks, familyId);
    Console.WriteLine($"Merged {pairs.Length / 2} binaries -> {outputPath} ({allBlocks.Count} blocks)");
}
else if (args.Length == 4)
{
    // <input.bin> <base_address> <family_id> <output.uf2>
    string inputPath = args[0];
    uint baseAddr = ParseHex(args[1]);
    uint familyId = ParseHex(args[2]);
    string outputPath = args[3];

    byte[] bin = File.ReadAllBytes(inputPath);
    var blocks = new List<(uint addr, byte[] data, int len)>();
    AddBlocks(blocks, bin, baseAddr);
    WriteUf2(outputPath, blocks, familyId);
    Console.WriteLine($"Converted {inputPath} -> {outputPath} ({blocks.Count} blocks)");
}
else
{
    Console.Error.WriteLine("Usage: bin2uf2 <input.bin> <base_address> <family_id> <output.uf2>");
    Console.Error.WriteLine("       bin2uf2 --merge <output.uf2> <family_id> <file1> <addr1> [<file2> <addr2> ...]");
    return 1;
}

return 0;

static void AddBlocks(List<(uint addr, byte[] data, int len)> blocks, byte[] bin, uint baseAddr)
{
    int offset = 0;

    while (offset < bin.Length)
    {
        int chunkLen = Math.Min(UF2_DATA_SIZE, bin.Length - offset);
        byte[] chunk = new byte[UF2_DATA_SIZE]; // zero-padded to 256
        Buffer.BlockCopy(bin, offset, chunk, 0, chunkLen);
        blocks.Add((baseAddr + (uint)offset, chunk, UF2_DATA_SIZE));
        offset += UF2_DATA_SIZE;
    }
}

static void WriteUf2(string path, List<(uint addr, byte[] data, int len)> blocks, uint familyId)
{
    uint totalBlocks = (uint)blocks.Count;

    using var fs = File.Create(path);

    Span<byte> block = stackalloc byte[512];

    for (int i = 0; i < blocks.Count; i++)
    {
        var (addr, data, len) = blocks[i];

        block.Clear();

        // Header (32 bytes)
        BinaryPrimitives.WriteUInt32LittleEndian(block[0..],  UF2_MAGIC_START0);
        BinaryPrimitives.WriteUInt32LittleEndian(block[4..],  UF2_MAGIC_START1);
        BinaryPrimitives.WriteUInt32LittleEndian(block[8..],  UF2_FLAG_FAMILY);
        BinaryPrimitives.WriteUInt32LittleEndian(block[12..], addr);
        BinaryPrimitives.WriteUInt32LittleEndian(block[16..], (uint)len);
        BinaryPrimitives.WriteUInt32LittleEndian(block[20..], (uint)i);
        BinaryPrimitives.WriteUInt32LittleEndian(block[24..], totalBlocks);
        BinaryPrimitives.WriteUInt32LittleEndian(block[28..], familyId);

        // Data (256 bytes at offset 32)
        data.AsSpan(0, UF2_DATA_SIZE).CopyTo(block[32..]);

        // Footer (4 bytes at offset 508)
        BinaryPrimitives.WriteUInt32LittleEndian(block[508..], UF2_MAGIC_END);

        fs.Write(block);
    }
}

static uint ParseHex(string s)
{
    if (s.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
        s = s[2..];

    return uint.Parse(s, System.Globalization.NumberStyles.HexNumber);
}
