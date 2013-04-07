// license.h : main header file for the LICENSE application
//

#if !defined(AFX_LICENSE_H__2B986148_BBF9_4BEC_89FB_84F8F1B6D755__INCLUDED_)
#define AFX_LICENSE_H__2B986148_BBF9_4BEC_89FB_84F8F1B6D755__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLicenseApp:
// See license.cpp for the implementation of this class
//

class CLicenseApp : public CWinApp
{
public:
	CLicenseApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLicenseApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSE_H__2B986148_BBF9_4BEC_89FB_84F8F1B6D755__INCLUDED_)
