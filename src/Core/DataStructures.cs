using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace FF7SND
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct FmtFileHeader
    {
        public uint Length;
        public uint Offset;
        public uint Loop;
        public uint Count;
        public uint Start;
        public uint End;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct ADPCMCOEFSET
    {
        public ushort Coef1;
        public ushort Coef2;

        public void Serialize(BinaryWriter writer)
        {
            writer.Write(BitConverter.GetBytes(Coef1));
            writer.Write(BitConverter.GetBytes(Coef2));
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct WAVEFORMATEX
    {
        public ushort FormatTag;
        public ushort Channels;
        public uint SamplesPerSec;
        public uint AvgBytesPerSec;
        public ushort BlockAlign;
        public ushort BitsPerSample;
        public ushort cbSize;

        public void Serialize(BinaryWriter writer)
        {
            writer.Write(BitConverter.GetBytes(FormatTag));
            writer.Write(BitConverter.GetBytes(Channels));
            writer.Write(BitConverter.GetBytes(SamplesPerSec));
            writer.Write(BitConverter.GetBytes(AvgBytesPerSec));
            writer.Write(BitConverter.GetBytes(BlockAlign));
            writer.Write(BitConverter.GetBytes(BitsPerSample));
            writer.Write(BitConverter.GetBytes(cbSize));
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct ADPCMWAVEFORMAT
    {
        public WAVEFORMATEX waveFormatEx;
        public ushort SamplesPerBlock;
        public ushort NumCoef;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 7)]
        public ADPCMCOEFSET[] Coef;

        public void Serialize(BinaryWriter writer)
        {
            waveFormatEx.Serialize(writer);
            writer.Write(BitConverter.GetBytes(SamplesPerBlock));
            writer.Write(BitConverter.GetBytes(NumCoef));
            for (ushort idx = 0; idx < 7; idx++) Coef[idx].Serialize(writer);
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct RiffChunk
    {
        public string Id;
        public uint Size;
        public string Format;

        public void Serialize(BinaryWriter writer)
        {
            writer.Write(Encoding.ASCII.GetBytes(Id));
            writer.Write(BitConverter.GetBytes(Size));
            writer.Write(Encoding.ASCII.GetBytes(Format));
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct FormatChunk
    {
        public string Id;
        public uint Size;
        public ADPCMWAVEFORMAT ADPCM;

        public void Serialize(BinaryWriter writer)
        {
            writer.Write(Encoding.ASCII.GetBytes(Id));
            writer.Write(BitConverter.GetBytes(Size));
            ADPCM.Serialize(writer);
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct DataChunk
    {
        public string Id;
        public uint Size;
        public byte[] Data;

        public void Serialize(BinaryWriter writer)
        {
            writer.Write(Encoding.ASCII.GetBytes(Id));
            writer.Write(BitConverter.GetBytes(Size));
            writer.Write(Data, 0, Data.Length);
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct LoopChunk
    {
        public string Id;
        public uint Size;
        public uint Start;
        public uint End;

        public void Serialize(BinaryWriter writer)
        {
            writer.Write(Encoding.ASCII.GetBytes(Id));
            writer.Write(BitConverter.GetBytes(Size));
            writer.Write(BitConverter.GetBytes(Start));
            writer.Write(BitConverter.GetBytes(End));
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct AudioFile
    {
        public FmtFileHeader fmtHeader;
        public RiffChunk riffChunk;
        public FormatChunk formatChunk;
        public LoopChunk loopChunk;
        public DataChunk dataChunk;
    }
}
