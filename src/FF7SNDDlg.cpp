/*
// FF7SND is a Final Fantasy VII Audio Extraction app
// Copyright (C) 2003 Qhimm.com
// Copyright (C) 2020 Julian Xhokaxhiu

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// FF7SNDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FF7SND.h"
#include "FF7SNDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFF7SNDDlg dialog

CFF7SNDDlg::CFF7SNDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFF7SNDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFF7SNDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hdrmem=NULL;
	hdroff=NULL;
	nHeaders=0;
}

CFF7SNDDlg::~CFF7SNDDlg() {
	if (hdrmem) free(hdrmem);
	if (hdroff) free(hdroff);
}

void CFF7SNDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFF7SNDDlg)
	DDX_Control(pDX, IDC_CLIPS, m_Clips);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFF7SNDDlg, CDialog)
	//{{AFX_MSG_MAP(CFF7SNDDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_LOAD, OnLoad)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(ID_PLAY, OnPlay)
	ON_BN_CLICKED(ID_EXTRACT, OnExtract)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFF7SNDDlg message handlers

BOOL CFF7SNDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	
	// TODO: Add extra initialization here
	HKEY keyReg;
	strcpy(FF7Dir, ""); // Set the dir to nothing

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, R"(Software\Square Soft, Inc.\Final Fantasy VII)", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &keyReg))
	{
		char buf[MAX_PATH];
		unsigned long pulLen = (ULONG)MAX_PATH;

		unsigned long lType = REG_SZ;
		long lReturn = 0;

		lReturn = RegQueryValueEx(
			keyReg,
			"AppPath",
			NULL,
			&lType,
			(UCHAR*)&buf,
			&pulLen );

		if (ERROR_SUCCESS == lReturn)
		{
			strcpy(FF7Dir, buf);
			if (strlen(FF7Dir) != 0)
			{
				if (FF7Dir[strlen(FF7Dir)-1] != 92) strcat(FF7Dir, "\\");
				strcat(FF7Dir, "Data\\Sound");
			}
		}
			
		RegCloseKey(keyReg);
	}

	if (strlen(FF7Dir) == 0) {
		MessageBox("Could not locate Final Fantasy VII. You will have to select the directory manually.", "Error", MB_ICONEXCLAMATION);
		strcpy(FF7Dir, "C:\\");
	}

	CRect rc;
	m_Clips.GetWindowRect(rc);
	m_Clips.SetWindowPos(NULL, 0,0, rc.Width(),164, SWP_NOZORDER | SWP_NOMOVE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFF7SNDDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFF7SNDDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFF7SNDDlg::OnLoad() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgFile(TRUE,
		"dat",
		"audio.dat",
		OFN_HIDEREADONLY,
		"FF7 Sound file|audio.dat|All files (*.*)|*.*|",
		this);

	dlgFile.m_ofn.lpstrInitialDir = FF7Dir;
	if (IDCANCEL == dlgFile.DoModal()) return;
	
	strcpy(FF7Dir, dlgFile.GetPathName());
	FF7Dir[strlen(FF7Dir)-dlgFile.GetFileName().GetLength()-1] = 0;

	char szFile[MAX_PATH];
	strcpy(szDat, dlgFile.GetPathName());
	szDat[strlen(szDat)-dlgFile.GetFileExt().GetLength()-1] = 0;
	sprintf(szFile, "%s.fmt", szDat);
	strcat(szDat, ".dat");

	CFile fileHdr;
	if (!fileHdr.Open(szFile, CFile::modeRead | CFile::shareDenyNone)) return;

	size_t len = (size_t)fileHdr.GetLength();
//	fileHdr.Read(&nHeaders, 4);
	if (hdrmem) free(hdrmem);
	if (hdroff) free(hdroff);
	hdrmem = (char*)malloc(len);
	hdroff = (int*)malloc(800*sizeof(int));
	if (!hdrmem || !hdroff) {
		if (hdrmem) free(hdrmem);
		if (hdroff) free(hdroff);
		MessageBox("Not enough memory.", "Error", MB_ICONHAND);
		return;
	}

	fileHdr.Seek(0, CFile::begin);
	fileHdr.Read(hdrmem, len);

	fileHdr.Close();

	FF7SNDHEADER *hdr;
	m_Clips.SetItemHeight(0,16);
	m_Clips.ResetContent();
	size_t n=0, i=0;
	char buf[32];
	while (n<len) {
		hdr = (FF7SNDHEADER*)(hdrmem+n);
		if (hdr->length == 0) {
			n += 42;
			continue;
		}
		hdroff[i] = n;
		sprintf(buf, "%i", hdr->wfex.nSamplesPerSec);
		m_Clips.AddString((LPCTSTR)i);
		n += 46 + hdr->wNumCoef * 4;
		i++;
	}
	nHeaders = i;
	
}

void CFF7SNDDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDCtl == IDC_CLIPS) {
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);
		CRect rc(lpDrawItemStruct->rcItem);

		if (lpDrawItemStruct->itemState & ODS_SELECTED) {
			dc.FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT));
			dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		} else {
			dc.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));
			dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		}
		
		int c = lpDrawItemStruct->itemData;

		FF7SNDHEADER* hdr = (FF7SNDHEADER*)(hdrmem+hdroff[c]);
		
		char buf[100];
		sprintf(buf, "%05i", c+1);
		dc.DrawText(CString(buf), CRect(rc.left+5,rc.top+1,rc.left+100,rc.bottom), 0);
		
		sprintf(buf, "%i kB", hdr->length >> 10);
		dc.DrawText(CString(buf), CRect(rc.left+5,rc.top+1,rc.left+160,rc.bottom), DT_RIGHT);

		sprintf(buf, "%i", hdr->wfex.nSamplesPerSec);
		dc.DrawText(CString(buf), CRect(rc.left+200,rc.top,rc.right,rc.bottom), 0);

		dc.Detach();

	} else
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CFF7SNDDlg::OnPlay() 
{
	// TODO: Add your control notification handler code here
	int c = m_Clips.GetCurSel();

	CFile fileIn, fileOut;
	fileIn.Open(szDat, CFile::modeRead | CFile::shareDenyNone);

	FF7SNDHEADER *hdr = (FF7SNDHEADER*)(hdrmem+hdroff[c]);
	int len = hdr->length + 46 + (hdr->wfex.cbSize == 0 ? 0 : (4+hdr->wNumCoef * 4));
	char *snd = (char*)malloc(len);
	char *p = snd;

	p = 4 + (char*)memcpy(p, "RIFF", 4);
	ULONG temp = hdr->length + 36 + (hdr->wfex.cbSize == 0 ? 0 : (4+hdr->wNumCoef * 4));
	p = 4 + (char*)memcpy(p, &temp, 4);
	p = 8 + (char*)memcpy(p, "WAVEfmt ", 8);
	temp = 18 + (hdr->wfex.cbSize == 0 ? 0 : (4+hdr->wNumCoef*4));
	p = 4 + (char*)memcpy(p, &temp, 4);
	p = temp + (char*)memcpy(p, &hdr->wfex, temp);
	p = 4 + (char*)memcpy(p, "data", 4);
	temp = hdr->length;
	p = 4 + (char*)memcpy(p, &temp, 4);
	
	fileIn.Seek(hdr->offset, CFile::begin);
	fileIn.Read(p, hdr->length);

	fileIn.Close();

	GetDlgItem(ID_PLAY)->EnableWindow(FALSE);
	UpdateWindow();

	sndPlaySound(snd, SND_MEMORY | SND_SYNC);

	GetDlgItem(ID_PLAY)->EnableWindow(TRUE);
	UpdateWindow();

	free(snd);

}

void CFF7SNDDlg::OnExtract() 
{
	CFolderPickerDialog dlgFolder(
		"",
		0,
		this
	);

	dlgFolder.m_ofn.lpstrInitialDir = FF7Dir;
	if (IDCANCEL == dlgFolder.DoModal()) return;

	for (int idx = 0; idx < m_Clips.GetCount(); idx++)
	{
		if (m_Clips.GetSel(idx) > 0)
		{
			char outPath[MAX_PATH]{ 0 };

			CT2A chosenPath(dlgFolder.GetPathName());

			sprintf(outPath, "%s/%d.wav", chosenPath.m_psz, idx + 1);

			CFile fileIn, fileOut;
			fileIn.Open(szDat, CFile::modeRead | CFile::shareDenyNone);
			fileOut.Open(outPath, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyWrite);

			FF7SNDHEADER* hdr = (FF7SNDHEADER*)(hdrmem + hdroff[idx]);
			ULONG temp = MAKEFOURCC('R', 'I', 'F', 'F');
			fileOut.Write(&temp, 4);
			temp = hdr->length + 36 + (hdr->wfex.cbSize == 0 ? 0 : (4 + hdr->wNumCoef * 4));
			fileOut.Write(&temp, 4);
			temp = MAKEFOURCC('W', 'A', 'V', 'E');
			fileOut.Write(&temp, 4);
			temp = MAKEFOURCC('f', 'm', 't', ' ');
			fileOut.Write(&temp, 4);
			temp = 18 + (hdr->wfex.cbSize == 0 ? 0 : (4 + hdr->wNumCoef * 4));
			fileOut.Write(&temp, 4);
			fileOut.Write(&hdr->wfex, 18 + (hdr->wfex.cbSize == 0 ? 0 : (4 + hdr->wNumCoef * 4)));
			temp = MAKEFOURCC('d', 'a', 't', 'a');
			fileOut.Write(&temp, 4);
			temp = hdr->length;
			fileOut.Write(&temp, 4);

			char* buf = (char*)malloc(hdr->length);
			fileIn.Seek(hdr->offset, CFile::begin);
			fileIn.Read(buf, hdr->length);
			fileOut.Write(buf, hdr->length);
			free(buf);

			fileIn.Close();
			fileOut.Close();
		}
	}

	MessageBox("All files extracted successfully.", "", MB_ICONINFORMATION);
}

BOOL CFF7SNDDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((::GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			switch (pMsg->wParam)
			{
			case 'A':
				m_Clips.SelItemRange(TRUE, 0, m_Clips.GetCount() - 1);
				break;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
