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

// FF7SND.h : main header file for the FF7SND application
//

#if !defined(AFX_FF7SND_H__0B59B403_301D_11D4_815E_0020183A5DA9__INCLUDED_)
#define AFX_FF7SND_H__0B59B403_301D_11D4_815E_0020183A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFF7SNDApp:
// See FF7SND.cpp for the implementation of this class
//

class CFF7SNDApp : public CWinApp
{
public:
	CFF7SNDApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFF7SNDApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFF7SNDApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FF7SND_H__0B59B403_301D_11D4_815E_0020183A5DA9__INCLUDED_)
