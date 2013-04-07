// licenseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "YoLicense.h"
#include "license.h"
#include "licenseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CYoLicense m_license("temp");


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog

CLicenseDlg::CLicenseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLicenseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicenseDlg)
	m_initializationCode = _T("");
	m_staticDate = _T("");
	m_temporaryKey = _T("");
	m_hardwareCodes = _T("");
	m_permanentLicense = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseDlg)
	DDX_Text(pDX, IDC_INITIALIZATION_CODE, m_initializationCode);
	DDX_Text(pDX, IDC_STATIC_DATE, m_staticDate);
	DDX_Text(pDX, IDC_TEMPORARY_KEY, m_temporaryKey);
	DDX_Text(pDX, IDC_HARDWARE_CODES, m_hardwareCodes);
	DDX_Text(pDX, IDC_PERMANENT_LICENSE, m_permanentLicense);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLicenseDlg, CDialog)
	//{{AFX_MSG_MAP(CLicenseDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_INITIALIZATION_CODE, OnChangeInitializationCode)
	ON_BN_CLICKED(IDC_EXP_1, OnExp1)
	ON_BN_CLICKED(IDC_EXP_180, OnExp180)
	ON_BN_CLICKED(IDC_EXP_30, OnExp30)
	ON_BN_CLICKED(IDC_EXP_360, OnExp360)
	ON_BN_CLICKED(IDC_EXP_60, OnExp60)
	ON_BN_CLICKED(IDC_EXP_7, OnExp7)
	ON_BN_CLICKED(IDC_EXP_90, OnExp90)
	ON_EN_CHANGE(IDC_HARDWARE_CODES, OnChangeHardwareCodes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg message handlers

BOOL CLicenseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CWnd *pCBox;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	pCBox = (CComboBox*)GetDlgItem(IDC_EXP_1);
	((CButton *)pCBox)->SetCheck(TRUE);

	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLicenseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLicenseDlg::OnPaint() 
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
HCURSOR CLicenseDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLicenseDlg::OnChangeInitializationCode() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	char s[64];
	char *t;
	int i;

	UpdateData(TRUE);
	sprintf(s, "%s", m_initializationCode);

	// remove spaces
	i = 0;
	for (t = s; *t != '\0'; t++)
	{
		if (*t != ' ')
		{
			s[i] = *t;
			i++;
		}
	}
	s[i] = '\0';
	TRACE1("m_initializationCode = %s\n", s);

	if (strlen(s) == 16)
	{
		m_initializationCode = s;
		rpTemporaryKey();
	}
	else
	{
		m_temporaryKey = "";
		m_staticDate = "";
		UpdateData(FALSE);
	}


	return;
}


BOOL CLicenseDlg::rpTemporaryKey()
// generate temporary key based on initialization code, verified date and time, and
// number of days to expiration
{
	char dateTime[64];
	//char temporaryKey[17];
	//char transCode[17];
	//double a[17];
	//double b;
	//double c;
	//double r;
	int i;
	int j;
	//char code[17];
	char s[48];
	char u;
	CWnd *pCBox;
	//int year;
	//int month;
	//int day;
	//int hour;
	//int min;
	//int sec;
	//int ms;


	sprintf(m_license.m_initializationCode, "%s", m_initializationCode);
	m_license.yoGetDateTime(dateTime);
	m_staticDate = dateTime;

	m_license.yoTemporaryKey();

	/***
	sprintf(code, "%s", m_initializationCode);

	// set initial conditions
	for (i = 0; i < 16; i++)
	{
		if (code[i] >= '0' && code[i] <= '9')
		{
			a[i] = (double)((int)code[i] - 48);
		}
		else if (toupper(code[i]) >= 'A' && toupper(code[i]) <= 'F')
		{
			a[i] = (double)((int)toupper(code[i]) - 55);
		}
	}

	for (i = 0; i < 16; i++)
	{
		// set initial condition between 0.0 and 1.0
		a[i] = 0.03125 + (0.0625 * a[i]);
	}

	for (i = 0; i < 16; i++)
	{
		
		// set r for each iteration
		r = 3.8791 + (0.1 * a[15 - i]);

		for (j = 0; j < 37 - ((i * 4) % 15); j++)
		{
			b = r * a[i];
			c = 1.0 - a[i];
			a[i] = b * c;
			//a[i] = r * a[i] * (1.0 - a[i]);
			//TRACE3("%d: %d: %lf\n", i, j, a[i]);
		}
		
		transCode[i] = (char)((int)(a[i] * 26.0) + 97);
		TRACE3("transCode[%d] = %c   %lf  ", i, transCode[i], a[i] * 26.0);
		TRACE1("r = %lf\n", r);
	}

	// shuffle entries
	transCode[15] = transCode[0];
	transCode[0] = transCode[9];
	transCode[9] = transCode[11];
	transCode[11] = transCode[2];
	transCode[2] = transCode[8];
	transCode[8] = transCode[14];
	transCode[14] = transCode[3];
	transCode[3] = transCode[5];
	transCode[5] = transCode[6];
	transCode[6] = transCode[1];
	transCode[1] = transCode[12];
	transCode[12] = transCode[10];
	transCode[10] = transCode[13];
	transCode[13] = transCode[7];
	transCode[7] = transCode[4];
	transCode[4] = transCode[15];
	transCode[16] = '\0';

	// manipulate entries dependent on other values in translation code
	// change first w to an h
	for (i = 0; i < 16; i++)
	{
		if (transCode[i] == 'w')
		{
			transCode[i] = 'h';
			break;
		}
	}
	// change first x to a g
	for (i = 0; i < 16; i++)
	{
		if (transCode[i] == 'x')
		{
			transCode[i] = 'g';
			break;
		}
	}
	// change first y to a k
	for (i = 0; i < 16; i++)
	{
		if (transCode[i] == 'y')
		{
			transCode[i] = 'k';
			break;
		}
	}
	// change first z to an r
	for (i = 0; i < 16; i++)
	{
		if (transCode[i] == 'z')
		{
			transCode[i] = 'r';
			break;
		}
	}

	// change i + 8th entry based on i and i + 8
	for (i = 0; i < 16; i++)
	{
		temporaryKey[(i + 8) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 8) % 16] - 97 + (i % 3)) % 26) + 97);
	}
	for (i = 0; i < 16; i++)
	{
		transCode[i] = temporaryKey[i];
	}

	// change first a to an s
	for (i = 0; i < 16; i++)
	{
		if (transCode[i] == 'a')
		{
			transCode[i] = 's';
			break;
		}
	}

	// change i + 3rd entry based on i and i + 3
	for (i = 0; i < 16; i++)
	{
		temporaryKey[(i + 3) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 3) % 16] - 97 + (i % 7)) % 26) + 97);
	}
	for (i = 0; i < 16; i++)
	{
		transCode[i] = temporaryKey[i];
	}

	// change first b to an m
	for (i = 0; i < 16; i++)
	{
		if (transCode[i] == 'b')
		{
			transCode[i] = 'm';
			break;
		}
	}

	// change i + 11th entry based on i and i + 11
	for (i = 0; i < 16; i++)
	{
		if (((int)transCode[i] - 97) % 3 != 1)
		{
			temporaryKey[(i + 11) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 11) % 16] - 97 + (i % 1)) % 26) + 97);
		}
	}
	for (i = 0; i < 16; i++)
	{
		transCode[i] = temporaryKey[i];
	}

	// change first c to a q
	for (i = 0; i < 16; i++)
	{
		if (transCode[i] == 'c')
		{
			transCode[i] = 'q';
			break;
		}
	}

	// change i + 7th entry based on i and i + 7
	for (i = 0; i < 16; i++)
	{
		if (((int)transCode[i] - 97) % 5 != 3)
		{
			temporaryKey[(i + 7) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 7) % 16] - 97) % 26) + 97);
		}
	}

	temporaryKey[16] = '\0';
***/

	// insert expiration key

	// find placeholder for expiration code
	// expiration codes
	// a = 360 days
	// b = 180 days
	// c = 90 days
	// d = 60 days
	// e = 30 days
	// f = 7 days
	// g = 1 day

	u = 'g';
	pCBox = (CComboBox*)GetDlgItem(IDC_EXP_360);
	if (((CButton *)pCBox)->GetCheck())
	{
		u = 'a';
	}
	pCBox = (CComboBox*)GetDlgItem(IDC_EXP_180);
	if (((CButton *)pCBox)->GetCheck())
	{
		u = 'b';
	}
	pCBox = (CComboBox*)GetDlgItem(IDC_EXP_90);
	if (((CButton *)pCBox)->GetCheck())
	{
		u = 'c';
	}
	pCBox = (CComboBox*)GetDlgItem(IDC_EXP_60);
	if (((CButton *)pCBox)->GetCheck())
	{
		u = 'd';
	}
	pCBox = (CComboBox*)GetDlgItem(IDC_EXP_30);
	if (((CButton *)pCBox)->GetCheck())
	{
		u = 'e';
	}
	pCBox = (CComboBox*)GetDlgItem(IDC_EXP_7);
	if (((CButton *)pCBox)->GetCheck())
	{
		u = 'f';
	}

	TRACE1("temporaryKey = %s\n", m_license.m_temporaryKey);
	i = (((int)m_license.m_temporaryKey[0] - 97) % 14) + 1;
	m_license.m_temporaryKey[i] = u;
	TRACE1("temporaryKey = %s (with exp)\n", m_license.m_temporaryKey);

	// insert spaces for m_temporaryKey
	j = 0;
	for (i = 0; i < 16; i++)
	{
		s[j] = m_license.m_temporaryKey[i];
		j++;
		if (i < 15 && (i % 4) == 3)
		{
			s[j] = ' ';
			j++;
		}
	}
	s[j] = '\0';
	m_temporaryKey = s;

	/***
	// decode date and time
	// year = 9783 - year  (hex) (4 chars) pos 13 8 15 3
	// month = month (hex) (1 char) pos 7
	// day = 89 - day (hex) (2 chars) pos 2 9
	// hour = 55 - hour (dec) (2 chars) pos 14 12
	// min = 69 - min (hex) (2 chars) pos 4 11
	// sec = 91 - sec (dec) (2 chars) pos 10 6
	// ms = ms (hex) (3 chars) pos 1 0 5
	s[0] = code[13];
	s[1] = code[8];
	s[2] = code[15];
	s[3] = code[3];
	s[4] = '\0';
	sscanf(s, "%x", &year);
	year = 9783 - year;
	
	s[0] = code[7];
	s[1] = '\0';
	sscanf(s, "%x", &month);

	s[0] = code[2];
	s[1] = code[9];
	s[2] = '\0';
	sscanf(s, "%x", &day);
	day = 89 - day;

	s[0] = code[14];
	s[1] = code[12];
	s[2] = '\0';
	sscanf(s, "%d", &hour);
	hour = 55 - hour;

	s[0] = code[4];
	s[1] = code[11];
	s[2] = '\0';
	sscanf(s, "%x", &min);
	min = 69 - min;

	s[0] = code[10];
	s[1] = code[6];
	s[2] = '\0';
	sscanf(s, "%d", &sec);
	sec = 91 - sec;

	s[0] = code[1];
	s[1] = code[0];
	s[2] = code[5];
	s[3] = '\0';
	sscanf(s, "%x", &ms);

	sprintf(s, "%04d.%02d.%02d  (%02d:%02d:%02d.%03d)",
		year, month, day, hour, min, sec, ms);
	**/


	// insert spaces for m_initializationCode
	j = 0;
	for (i = 0; i < 16; i++)
	{
		s[j] = m_license.m_initializationCode[i];
		j++;
		if (i < 15 && (i % 4) == 3)
		{
			s[j] = ' ';
			j++;
		}
	}
	s[j] = '\0';
	m_initializationCode = s;


	UpdateData(FALSE);

	return TRUE;
}



void CLicenseDlg::OnExp1() 
{
	// TODO: Add your control notification handler code here
	OnChangeInitializationCode();
	return;	
}

void CLicenseDlg::OnExp180() 
{
	// TODO: Add your control notification handler code here
	OnChangeInitializationCode();
	return;	
}

void CLicenseDlg::OnExp30() 
{
	// TODO: Add your control notification handler code here
	OnChangeInitializationCode();
	return;	
}

void CLicenseDlg::OnExp360() 
{
	// TODO: Add your control notification handler code here
	OnChangeInitializationCode();
	return;
}

void CLicenseDlg::OnExp60() 
{
	// TODO: Add your control notification handler code here
	OnChangeInitializationCode();
	return;
}

void CLicenseDlg::OnExp7() 
{
	// TODO: Add your control notification handler code here
	OnChangeInitializationCode();
	return;	
}

void CLicenseDlg::OnExp90() 
{
	// TODO: Add your control notification handler code here
	OnChangeInitializationCode();
	return;	
}

void CLicenseDlg::OnChangeHardwareCodes() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	char s[64];
	int i;
	int j;

	UpdateData(TRUE);
	sprintf(s, "%s", m_hardwareCodes);

	for (i = 0; i < 3; i++)
	{
		m_license.m_code[i][0] = '\0';
	}
	m_license.m_license[0] = '\0';

	sscanf(s, "%s %s %s", m_license.m_code[0], m_license.m_code[1], m_license.m_code[2]);


	if (strlen(m_license.m_code[0]) != 17)
	{
		m_license.m_code[0][0] = '\0';
	}
	if (strlen(m_license.m_code[1]) != 17)
	{
		m_license.m_code[1][0] = '\0';
	}
	if (strlen(m_license.m_code[2]) != 17)
	{
		m_license.m_code[2][0] = '\0';
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 17 && m_license.m_code[i][j] != '\0'; j++)
		{
			if ((toupper(m_license.m_code[i][j]) >= 'A' && toupper(m_license.m_code[i][j]) <= 'F')
				|| (m_license.m_code[i][j] >= '0' && m_license.m_code[i][j] <= '9')
				|| m_license.m_code[i][j] == '-')
			{
				// valid
			}
			else
			{
				m_license.m_code[i][0] = '\0';
				break;
			}
		}
	}

	TRACE3("m_license.code = %s   %s   %s\n", 
		m_license.m_code[0], m_license.m_code[1], m_license.m_code[2]);

	if (m_license.m_code[0][0] != '\0' || m_license.m_code[1][0] != '\0'
		|| m_license.m_code[2][0] != '\0')
	{
		m_license.yoEncode("");
		m_permanentLicense = m_license.m_license;
	}
	else
	{
		m_permanentLicense = "";
	}

	UpdateData(FALSE);

	return;

}
