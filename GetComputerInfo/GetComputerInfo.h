// GetComputerInfo.h : main header file for the GETCOMPUTERINFO application
//

#if !defined(AFX_GETCOMPUTERINFO_H__CDD29D5F_5E8E_4F7C_98A4_CD4A85115648__INCLUDED_)
#define AFX_GETCOMPUTERINFO_H__CDD29D5F_5E8E_4F7C_98A4_CD4A85115648__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGetComputerInfoApp:
// See GetComputerInfo.cpp for the implementation of this class
//

class CGetComputerInfoApp : public CWinApp
{
public:
	CGetComputerInfoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetComputerInfoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGetComputerInfoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETCOMPUTERINFO_H__CDD29D5F_5E8E_4F7C_98A4_CD4A85115648__INCLUDED_)
