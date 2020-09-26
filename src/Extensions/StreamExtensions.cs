using System;
using System.IO;
using System.Runtime.InteropServices;

namespace FF7SND
{
    public static class StreamExtensions
    {
        public static T ReadStruct<T>(this Stream stream) where T : struct
        {
            var sz = Marshal.SizeOf(typeof(T));
            var buffer = new byte[sz];
            stream.Read(buffer, 0, sz);
            var pinnedBuffer = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            var structure = (T)Marshal.PtrToStructure(
                pinnedBuffer.AddrOfPinnedObject(), typeof(T));
            pinnedBuffer.Free();
            return structure;
        }

        public static void WriteStruct<T>(this Stream stream, T data) where T : struct
        {
            var sz = Marshal.SizeOf(data);
            IntPtr pStruct = Marshal.AllocHGlobal(sz);
            Marshal.StructureToPtr(data, pStruct, false);
            var buffer = Array.CreateInstance(typeof(byte), sz) as byte[];
            Marshal.Copy(pStruct, buffer, 0, buffer.Length);
            stream.Write(buffer, 0, sz);
            Marshal.Release(pStruct);
        }

        public static long SeekStruct<T>(this Stream stream) where T : struct
        {
            var sz = Marshal.SizeOf(typeof(T));
            return stream.Seek(sz, SeekOrigin.Current);
        }
    }
}
