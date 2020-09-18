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

// FF7SNDDlg.h : header file
//

#if !defined(AFX_FF7SNDDLG_H__0B59B405_301D_11D4_815E_0020183A5DA9__INCLUDED_)
#define AFX_FF7SNDDLG_H__0B59B405_301D_11D4_815E_0020183A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "mmsystem.h"
#include "mmreg.h"

struct FF7SNDHEADER {
	ULONG length;
	ULONG offset;
	char zz1[16];
	WAVEFORMATEX wfex;
	WORD wSamplesPerBlock;
	WORD wNumCoef;
	ADPCMCOEFSET aCoef[8];
};

/////////////////////////////////////////////////////////////////////////////
// CFF7SNDDlg dialog

class CFF7SNDDlg : public CDialog
{
// Construction
public:
	int nHeaders;
	char szDat[MAX_PATH]{ 0 };
	char FF7Dir[MAX_PATH]{ 0 };
	CFF7SNDDlg(CWnd* pParent = NULL);	// standard constructor
	~CFF7SNDDlg();

	char *hdrmem;
	int *hdroff;

// Dialog Data
	//{{AFX_DATA(CFF7SNDDlg)
	enum { IDD = IDD_FF7SND_DIALOG };
	CListBox	m_Clips;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFF7SNDDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFF7SNDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoad();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnPlay();
	afx_msg void OnExtract();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FF7SNDDLG_H__0B59B405_301D_11D4_815E_0020183A5DA9__INCLUDED_)
