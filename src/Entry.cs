using FF7SND.Core;
using Microsoft.Win32;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Linq;

namespace FF7SND
{
    public partial class Entry : Form
    {
        private string FF7Dir = string.Empty;
        private FmtFileHeader fmtHeader;
        private AudioFile[] audioList;

        public Entry()
        {
            InitializeComponent();

            RegistryKey ret = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32).OpenSubKey(@"Software\Square Soft, Inc.\Final Fantasy VII");
            if (ret != null)
            {
                FF7Dir = ret.GetValue("AppPath") + @"Data\Sound";
            }
            ret.Close();
        }

        private bool parseAudioFile(string audioDat, string audioFmt)
        {
            audioList = new AudioFile[750];

            FileStream fileFmt = File.OpenRead(audioFmt);
            FileStream fileDat = File.OpenRead(audioDat);

            for (int idx = 0; idx < audioList.Length; ++idx)
            {
                fmtHeader = fileFmt.ReadStruct<FmtFileHeader>();
                if (fmtHeader.Length == 0)
                {
                    fileFmt.SeekStruct<WAVEFORMATEX>();
                    continue;
                }

                // Save fmt Header info for later
                audioList[idx].fmtHeader = fmtHeader;

                // Set Header Info
                audioList[idx].riffChunk.Id = "RIFF".ToArray();
                audioList[idx].riffChunk.Size = 0;
                audioList[idx].riffChunk.Format = "WAVE".ToArray();

                // Set Format Info
                audioList[idx].formatChunk.Id = "fmt ".ToArray();
                audioList[idx].formatChunk.Size = (uint)Marshal.SizeOf(typeof(ADPCMWAVEFORMAT));
                audioList[idx].formatChunk.ADPCM = fileFmt.ReadStruct<ADPCMWAVEFORMAT>();

                // Set Loop Info
                audioList[idx].loopChunk.Id = "fflp".ToArray();
                audioList[idx].loopChunk.Size = (uint)Marshal.SizeOf(typeof(uint)) * 2;
                audioList[idx].loopChunk.Start = fmtHeader.Start;
                audioList[idx].loopChunk.End = fmtHeader.End;

                // Set Data Info
                audioList[idx].dataChunk.Id = "data".ToArray();
                audioList[idx].dataChunk.Size = fmtHeader.Length;
                audioList[idx].Data = new byte[audioList[idx].dataChunk.Size];
                fileDat.Read(audioList[idx].Data, 0, audioList[idx].Data.Length);

                // Finish saving some last info
                audioList[idx].riffChunk.Size = (uint)(audioList[idx].riffChunk.Id.Length + Marshal.SizeOf(typeof(FormatChunk)) + Marshal.SizeOf(typeof(DataChunk)) + audioList[idx].dataChunk.Size);
                if (fmtHeader.Loop > 0) audioList[idx].riffChunk.Size += (uint)(Marshal.SizeOf(typeof(LoopChunk)));
            }

            fileFmt.Close();
            fileDat.Close();

            return true;
        }

        private bool dumpAudioFile(string audioDat, string audioFmt)
        {
            FileStream fileFmt = File.OpenWrite(audioFmt);
            FileStream fileDat = File.OpenWrite(audioDat);

            for (int idx = 0; idx < audioList.Length; ++idx)
            {
                AudioFile audioFile = audioList[idx];

                // Write fmt header
                fileFmt.WriteStruct<FmtFileHeader>(audioFile.fmtHeader);

                // Write ADPCM data
                fileFmt.WriteStruct<ADPCMWAVEFORMAT>(audioFile.formatChunk.ADPCM);

                // Write audio data
                if (audioFile.Data != null) fileDat.Write(audioFile.Data, 0, audioFile.Data.Length);
            }

            fileFmt.Close();
            fileDat.Close();

            return true;
        }

        private void getWaveStream(Stream stream, int idx)
        {
            AudioFile audioFile = audioList[idx];

            stream.WriteStruct<RiffChunk>(audioFile.riffChunk);
            stream.WriteStruct<FormatChunk>(audioFile.formatChunk);
            stream.WriteStruct<DataChunk>(audioFile.dataChunk);

            if (audioFile.Data != null) stream.Write(audioFile.Data, 0, audioFile.Data.Length);

            if (audioFile.fmtHeader.Loop > 0) stream.WriteStruct<LoopChunk>(audioFile.loopChunk);
        }

        private void renderList()
        {
            for (int idx = 0; idx < audioList.Length; ++idx)
            {
                string[] item =
                {
                    (idx + 1).ToString(),
                    audioList[idx].dataChunk.Size.ToString(),
                    audioList[idx].formatChunk.ADPCM.waveFormatEx.SamplesPerSec.ToString(),
                    audioList[idx].fmtHeader.Loop.ToString()
                };

                lstView.Items.Add(new ListViewItem(item));
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();

            openFileDialog.Filter = "FF7 Sound file (audio.dat)|audio.dat|All files (*.*)|*.*";
            openFileDialog.DefaultExt = "dat";
            openFileDialog.FileName = "audio.dat";
            if (FF7Dir != string.Empty) openFileDialog.InitialDirectory = FF7Dir;

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string audioDat = openFileDialog.FileName;
                string audioFmt = Path.ChangeExtension(audioDat, "fmt");

                parseAudioFile(audioDat, audioFmt);
                renderList();
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("https://github.com/julianxhokaxhiu/FF7SND");
        }

        private void btnPlay_Click(object sender, EventArgs e)
        {
            MemoryStream ms = new MemoryStream();
            getWaveStream(ms, lstView.SelectedItems[0].Index);

            WinMM.PlaySound(ms.ToArray(), IntPtr.Zero, WinMM.WINMM_SND_SYNC | WinMM.WINMM_SND_MEMORY);
        }

        private void lstView_SelectedIndexChanged(object sender, EventArgs e)
        {
            btnExtract.Enabled = lstView.SelectedItems.Count > 0;
            btnPlay.Enabled = lstView.SelectedItems.Count == 1;
        }

        private void btnExtract_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderDialog = new FolderBrowserDialog();

            if (folderDialog.ShowDialog() == DialogResult.OK)
            {
                foreach(ListViewItem item in lstView.SelectedItems)
                {
                    FileStream fileOut = File.OpenWrite(folderDialog.SelectedPath + @"\" + (item.Index + 1).ToString() + @".wav");
                    getWaveStream(fileOut, item.Index);
                    fileOut.Close();
                }

                MessageBox.Show("Successfully exported the selected items.", "", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

        }

        private void Entry_KeyUp(object sender, KeyEventArgs e)
        {
            if ((Control.ModifierKeys & Keys.Control) == Keys.Control)
            {
                switch (e.KeyCode)
                {
                    case Keys.A:
                        lstView.Items.OfType<ListViewItem>().ToList().ForEach(item => item.Selected = true);
                        break;
                }
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();

            saveFileDialog.Filter = "FF7 Sound file (audio.dat)|audio.dat|All files (*.*)|*.*";
            saveFileDialog.DefaultExt = "dat";
            saveFileDialog.FileName = "audio.dat";
            if (FF7Dir != string.Empty) saveFileDialog.InitialDirectory = FF7Dir;

            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                string audioDat = saveFileDialog.FileName;
                string audioFmt = Path.ChangeExtension(audioDat, "fmt");

                if (dumpAudioFile(audioDat, audioFmt))
                    MessageBox.Show("Audio files were successfully saved in:\n\n" + audioFmt + "\n" + audioDat, "", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}
