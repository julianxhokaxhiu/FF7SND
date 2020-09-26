using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace FF7SND.Core
{
    public class WinMM
    {
        public const UInt32 WINMM_SND_SYNC = 0;
        public const UInt32 WINMM_SND_MEMORY = 4;

        [DllImport("Winmm.dll")]
        public static extern bool PlaySound(byte[] data, IntPtr hMod, UInt32 dwFlags);
    }
}
