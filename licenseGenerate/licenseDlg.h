// licenseDlg.h : header file
//

#if !defined(AFX_LICENSEDLG_H__D6BC6238_1ADB_49AE_84A9_7ACCCB60C13A__INCLUDED_)
#define AFX_LICENSEDLG_H__D6BC6238_1ADB_49AE_84A9_7ACCCB60C13A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog

class CLicenseDlg : public CDialog
{
// Construction
public:
	CLicenseDlg(CWnd* pParent = NULL);	// standard constructor


// Dialog Data
	//{{AFX_DATA(CLicenseDlg)
	enum { IDD = IDD_LICENSE_DIALOG };
	CString	m_initializationCode;
	CString	m_staticDate;
	CString	m_temporaryKey;
	CString	m_hardwareCodes;
	CString	m_permanentLicense;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	BOOL rpTemporaryKey();

	// Generated message map functions
	//{{AFX_MSG(CLicenseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeInitializationCode();
	afx_msg void OnExp1();
	afx_msg void OnExp180();
	afx_msg void OnExp30();
	afx_msg void OnExp360();
	afx_msg void OnExp60();
	afx_msg void OnExp7();
	afx_msg void OnExp90();
	afx_msg void OnChangeHardwareCodes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSEDLG_H__D6BC6238_1ADB_49AE_84A9_7ACCCB60C13A__INCLUDED_)
