// GetComputerInfoDlg.h : header file
//

#if !defined(AFX_GETCOMPUTERINFODLG_H__BD09D1BA_B92E_4764_BE6F_C00C667E9212__INCLUDED_)
#define AFX_GETCOMPUTERINFODLG_H__BD09D1BA_B92E_4764_BE6F_C00C667E9212__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGetComputerInfoDlg dialog

class CGetComputerInfoDlg : public CDialog
{
// Construction
public:
	CGetComputerInfoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGetComputerInfoDlg)
	enum { IDD = IDD_GETCOMPUTERINFO_DIALOG };
	CString	m_computerInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetComputerInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGetComputerInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETCOMPUTERINFODLG_H__BD09D1BA_B92E_4764_BE6F_C00C667E9212__INCLUDED_)
