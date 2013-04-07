// YoLicense.cpp: implementation of the CYoLicense class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "YoLicense.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CYoLicense::CYoLicense(char *appName)
{
	int i;
	for (i = 0; i < 31; i++)
	{
		m_applicationName[i] = *(appName + i);
	}
	for (i = 0; i < 3; i++)
	{
		m_code[i][0] = '\0';
	}
	m_license[0] = '\0';
	m_dateCode[0] = '\0';
	m_initializationCode[0] = '\0';
	m_temporaryKey[0] = '\0';

}

CYoLicense::~CYoLicense()
{
	// if temporary licensing is being used write registry with current date and time
	// before exiting
	if (m_temporaryKey[0] != '\0')
	{
		yoWriteRegistry();
	}
}


BOOL CYoLicense::yoValidateLicense(char progID[48], char message[256])
// Main entry point into license class.  Called from main application to verify a
// permanent license.  The program license, which is derived from either the hard disk
// serial number or the MAC address of an ethernet card installed on the user's computer,
// must be defined in the programLic.dll file.  The DLL must be included in the program
// distribution and copied to the same directory as the main application when the program
// files are installed.
// Pass in the unique program identifier:
//    progID is usually the CLSID of the program without braces and dashes.
//    Each license DLL will have the same program ID as the program ID defined
//    in the main application.  [progID passed in = progID defined in programLic.DLL]
//    Example: "F3CAEE0F4D244800A7469B7080362802"
// The message returned is formatted to be displayed in a dialog box if an error is encountered.
// The generated license is returned if successful.
//
{
//#define ADDRESS_ERROR_LICENSE_PIPE_FILE -100  // also declared in programLic.dll
//#define ADDRESS_ERROR_NO_ETHERNET_CARD -101
//#define ADDRESS_ERROR_INVALID_LICENSE -102
//#define ADDRESS_VERSION "2004.02.18"  // change version constant when creating updated release

//#ifdef _DEBUG
//#define ADDRESS_CODE "00-50-BF-75-01-CF" // comment out when creating release version  
//#endif

	int i;
	int j;
	int k;
	int n;
	double a;
	double a0;
	double b;
	double c;
	double r;
	char license[128];
	//char sLicense[24];
	char s[16];
	char DLLname[64];
	//CAddressLicenseDlg licenseDlg;
	HINSTANCE hDLL;               // Handle to DLL
	typedef void (* PFNLIC)(char *);
	PFNLIC pfnLic = NULL;

	sprintf(DLLname, "%sLic.dll", m_applicationName);

	hDLL = LoadLibrary(DLLname);
	if (hDLL == NULL)
	{
		sprintf(message, "%s%c%c%s%c%c%s %s",
			"Unable to run application!", 0x0D, 0x0A,
			"During the licensing process the required", 0x0D, 0x0A,
			DLLname, "was not found.");
		//return rpMessage(sMessage);
		//AfxMessageBox("Unable to run address application!     \nDuring the licensing process the required     \nprogramLic.dll was not found.     ");
		return FALSE;
	}

	pfnLic = (PFNLIC)GetProcAddress(hDLL, "yoValidLicense");
	if (!pfnLic)
	{
		FreeLibrary(hDLL);
		sprintf(message, "%s%c%c%s%c%c%s %s",
			"Unable to run application!", 0x0D, 0x0A,
			"During the licensing process the routine yoValidLicense() in", 0x0D, 0x0A,
			DLLname, "could not be properly called.");	
		//return rpMessage(sMessage);
		//AfxMessageBox("Unable to run address application!     \nDuring the licensing process an error     \nin programLic.dll was encountered.     ");
		return FALSE;
	}

	yoDateCode();
	sprintf(license, "%s", m_dateCode);
	// license sent to pfnLic is arbitrary string based on current date and time
	pfnLic(license);
	// license returned is one to three 16 char strings separated by a single space based
	// on the hard disk serial number and ethernet MAC address(es) defined in programLic.dll

	// initialize license
	sprintf(m_license, "0123456789ABCDEF");

	// check for valid license

	yoLicenseHDSN();
	if (yoLicenseMACPipe() == FALSE)
	{
		yoLicenseMACFile();
	}

	if (m_code[0][0] == '\0')
	//if (yoLicenseHDSN() == FALSE && yoLicenseMACPipe() == FALSE && yoLicenseMACFile() == FALSE)
	{
		FreeLibrary(hDLL);
		sprintf(message, "%s%c%c%s",
			"Cannot run application!", 0x0D, 0x0A,
			"Unable to generate proper licensing information.");	
		//return rpMessage(sMessage);
		//AfxMessageBox("Unable to run address application!     \nAn error occured when initiating licensing     \nroutines on this computer.     ");
		return FALSE;
	}

	
	if (yoEncode(license) == FALSE)
	{
		FreeLibrary(hDLL);
		sprintf(message, "%s%c%c%s%c%c%s",
			"Unable to run application due to licensing error.", 0x0D, 0x0A,
			"Valid license for this computer is:", 0x0D, 0x0A,
			m_license);	
		return FALSE;
	}

	// validation of licensing process with programLic.DLL
	// both this program and programLic.DLL will run the following loop
	// the code must match exactly in the DLL

	for (n = 0; n < 8; n++)
	{
		m_dateCode[0] = 'A'; // tell DLL to return a 126 digit code instead of a license
		sprintf(license, "%s", m_dateCode);
		pfnLic(license);

		k = (int)m_dateCode[93] - 48;
		r = 3.9 + (double)((int)progID[n] % 10) * 0.01
			+ (double)((int)m_dateCode[k * 7 + 5] - 48) * 0.001
			+ (double)((int)progID[16 + n] % 10) * 0.0001
			+ (double)((int)m_dateCode[k * 7 + 13] - 48) * 0.00001
			+ (double)((int)progID[24 + n] % 10) * 0.000001;
		a0 = (double)((int)progID[8 + n] % 10) * 0.1
			+ (double)((int)m_dateCode[k * 7 + 8] - 48) * 0.01
			+ (double)((int)progID[n] % 10) * 0.001
			+ (double)((int)m_dateCode[k * 7 + 3] - 48) * 0.0001
			+ (double)((int)progID[16 + n] % 10) * 0.00001
			+ (double)((int)m_dateCode[k * 7 + 15] - 48) * 0.000001;

		//k = (int)m_dateCode[93] - 48;
		//r = 3.9 + (double)((int)m_dateCode[k * 7 + 3] - 48) * 0.01
		//	+ (double)((int)m_dateCode[k * 7 + 5] - 48) * 0.001
		//	+ (double)((int)m_dateCode[k * 7 + 8] - 48) * 0.0001
		//	+ (double)((int)m_dateCode[k * 7 + 13] - 48) * 0.00001
		//	+ (double)((int)m_dateCode[k * 7 + 15] - 48) * 0.000001;
		//a0 = (double)((int)m_dateCode[k * 8 + 3] - 48) * 0.1
		//	+ (double)((int)m_dateCode[k * 8 + 5] - 48) * 0.01
		//	+ (double)((int)m_dateCode[k * 8 + 8] - 48) * 0.001
		//	+ (double)((int)m_dateCode[k * 8 + 13] - 48) * 0.0001;

		if (a0 < 0.0001)
		{
			a0 = 0.0001;
		}

		for (i = 0; i < 21; i++)
		{
			a = a0; // always reset a to prevent rounding error
			for (j = 0; j < 21 - ((i * 4) % 15); j++)
			{
				b = r * a;
				c = 1.0 - a;
				a = b * c;
			}
			//k = (int)(a * 1000000.0);
			sprintf(s, "%lf", a);
			m_dateCode[i * 6] = s[2];
			m_dateCode[i * 6 + 1] = s[3];
			m_dateCode[i * 6 + 2] = s[4];
			m_dateCode[i * 6 + 3] = s[5];
			m_dateCode[i * 6 + 4] = s[6];
			m_dateCode[i * 6 + 5] = s[7];
			//sprintf(&m_dateCode[i * 6], "%06d", k);
		}
		m_dateCode[126] = '\0';

		if (strcmp(m_dateCode, license) != 0)
		{
			FreeLibrary(hDLL);
			sprintf(message, "%s%c%c%s%c%c%s",
				"Unable to run application!", 0x0D, 0x0A,
				"An error occured when validating licensing.", 0x0D, 0x0A,
				"License may be for a different application");	
			//return rpMessage(sMessage);
			//AfxMessageBox("Unable to run address application!     \nAn error occured when validating licensing     \nrequirements on this computer.     ");
			return FALSE;
		}
		
	}

	FreeLibrary(hDLL);

	sprintf(message, "%s", m_license);
	return TRUE;

}

BOOL CYoLicense::yoGetDateTime(char *dateTime)
// extracts the current date and time from m_initializationCode
// used to verify that initialization code contains proper date and time
// so that key can be assigned for temporary licensing.
{
	char s[48];
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
	int ms;

	dateTime[0] = '\0';

	if (strlen(m_initializationCode) != 16)
	{
		return FALSE;
	}

	// decode date and time
	// year = 9783 - year  (hex) (4 chars) pos 13 8 15 3
	// month = month (hex) (1 char) pos 7
	// day = 89 - day (hex) (2 chars) pos 2 9
	// hour = 55 - hour (dec) (2 chars) pos 14 12
	// min = 69 - min (hex) (2 chars) pos 4 11
	// sec = 91 - sec (dec) (2 chars) pos 10 6
	// ms = ms (hex) (3 chars) pos 1 0 5
	s[0] = m_initializationCode[13];
	s[1] = m_initializationCode[8];
	s[2] = m_initializationCode[15];
	s[3] = m_initializationCode[3];
	s[4] = '\0';
	sscanf(s, "%x", &year);
	year = 9783 - year;
	
	s[0] = m_initializationCode[7];
	s[1] = '\0';
	sscanf(s, "%x", &month);

	s[0] = m_initializationCode[2];
	s[1] = m_initializationCode[9];
	s[2] = '\0';
	sscanf(s, "%x", &day);
	day = 89 - day;

	s[0] = m_initializationCode[14];
	s[1] = m_initializationCode[12];
	s[2] = '\0';
	sscanf(s, "%d", &hour);
	hour = 55 - hour;

	s[0] = m_initializationCode[4];
	s[1] = m_initializationCode[11];
	s[2] = '\0';
	sscanf(s, "%x", &min);
	min = 69 - min;

	s[0] = m_initializationCode[10];
	s[1] = m_initializationCode[6];
	s[2] = '\0';
	sscanf(s, "%d", &sec);
	sec = 91 - sec;

	s[0] = m_initializationCode[1];
	s[1] = m_initializationCode[0];
	s[2] = m_initializationCode[5];
	s[3] = '\0';
	sscanf(s, "%x", &ms);

	sprintf(dateTime, "%04d.%02d.%02d  (%02d:%02d:%02d.%03d)",
		year, month, day, hour, min, sec, ms);

	return TRUE;

}


void CYoLicense::yoDateCode()
// generate a code based on the current date and time
// and write that code to m_dateCode
{

	SYSTEMTIME gmSystemTime;
	CTime timeCurrent;
	int i;
	int j;
	int ms;
	double r;
	double a;
	double b;
	double c;

	::GetSystemTime(&gmSystemTime);
	timeCurrent = gmSystemTime;

	ms = (int)gmSystemTime.wMilliseconds;
	r = 3.89 + (double)ms * 0.0001;
	a = 0.37;
	for (i = 0; i < 21; i++)
	{
		for (j = 0; j < 31 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(&m_dateCode[i * 6], "%06ld", (long)(a * 1000000.0));

		// set r and a
		if (i % 6 == 0)
		{
			r = 3.8839 + (a * 0.1 + (double)(int)timeCurrent.GetSecond() * 0.0001);
		}
		else if (i % 6 == 1)
		{
			r = 3.8752 + (a * 0.1 + (double)(int)timeCurrent.GetMinute() * 0.0001);
		}
		else if (i % 6 == 2)
		{
			r = 3.8911 + (a * 0.1 + (double)(int)timeCurrent.GetHour() * 0.0001);
		}
		else if (i % 6 == 3)
		{
			r = 3.8594 + (a * 0.1 + (double)(int)timeCurrent.GetDay() * 0.0001);
		}
		else if (i % 6 == 4)
		{
			r = 3.9423;
		}
		else if (i % 6 == 5)
		{
			r = 3.8875 + (a * 0.1 + (double)(int)timeCurrent.GetMonth() * 0.0001);
		}
	}
	m_dateCode[126] = '\0';

	return;
	
}

BOOL CYoLicense::yoGetComputerInfo(char info[64])
// Obtains HDSN and MAC addresses for the computer running this application
// If HDSN or MAC addressess can be obtained info is set to m_code values and TRUE is returned
// otherwise FALSE is returned.
{
	int i;
	info[0] = '\0';

	for (i = 0; i < 3; i++)
	{
		m_code[i][0] = '\0';
	}
	
	yoLicenseHDSN();
	if (yoLicenseMACPipe() == FALSE)
	{
		yoLicenseMACFile();
	}

	if (m_code[0][0] == '\0')
	{
		return FALSE;
	}

	sprintf(info, "%s %s %s", m_code[0], m_code[1], m_code[2]);

	return TRUE;
}


BOOL CYoLicense::yoGetLicense(char license[64])
// Obtains license based on HDSN and MAC addresses for computer running this application.
// If a valid license can be generated license variable is set to m_license and TRUE is returned
// otherwise FALSE is returned.
{
	int i;
	license[0] = '\0';

	for (i = 0; i < 3; i++)
	{
		m_code[i][0] = '\0';
	}
	
	yoLicenseHDSN();
	if (yoLicenseMACPipe() == FALSE)
	{
		yoLicenseMACFile();
	}

	if (m_code[0][0] == '\0')
	{
		return FALSE;
	}

	yoEncode(license);  // routine will fail but m_license will be properly set

	sprintf(license, "%s", m_license);

	return TRUE;
}


BOOL CYoLicense::yoValidCode(char *code)
// checks code passed in to see if it is a valid HDSN or MAC address

{
	if (code[0] == '0' && code[1] == '0'
		|| code[0] == '0' && code[1] == '2'
		|| code[0] == '0' && code[1] == '4'
		|| code[0] == '0' && code[1] == '8'
		|| code[0] == '1' && code[1] == '0'
		|| code[0] == '1' && code[1] == '1'
		|| code[0] == '8' && code[1] == '0'
		|| code[0] == 'A' && code[1] == '0'
		|| code[0] == 'A' && code[1] == 'A'
		|| code[0] == 'A' && code[1] == 'C'
		// above are valid MAC addresses must begin with the above prefixes
		// as of Feb 2004
		|| code[0] == 'D' && code[1] == 'D')
		// signifies that code is a hard disk serial number
	{
		return TRUE;
	}

	return FALSE;

}



BOOL CYoLicense::yoEncode(char *license)
// Before this routine is called, m_code must have been set with the
// hard disk serial number and the MAC address of the ethernet card (or cards)
// attached to this computer using yoLicenseHDSN(), yoLicenseMACPipe(), or yoLicenseMACFile().
// Valid values of m_code are encoded and compared against the license variable
// passed in.  The license variable is obtained from decoded information stored in
// programLic.dll.  If a license for the hard disk serial number or MAC address equals
// the encoded value, m_license variable is set to license and yoEncode() returns TRUE
// to indicate success.  A FALSE value is returned if the match fails. 
{
	char transCode[17];
	char lic[3][17];
	char licDLL[3][17];
	BOOL charUsed[26];
	double a[17];
	double b;
	double c;
	double r;
	int deleteEntry;
	int i;
	int j;
	int k;
	int n;
	char code[18];

	if (m_code[0][0] == '\0')
	{
		return FALSE;
	}

	// initialize variables
	lic[0][0] = '\0';
	lic[1][0] = '\0';
	lic[2][0] = '\0';
	licDLL[0][0] = '\0';
	licDLL[1][0] = '\0';
	licDLL[2][0] = '\0';
	m_license[0] = '\0';

	for (n = 0; n < 3; n++)
	{
//#ifdef ADDRESS_CODE  // used for debugging only
//		if (n == 0)
//		{
//			sprintf(m_code[n], ADDRESS_CODE);
//		}
//#endif
		sprintf(code, "%s", m_code[n]);
		TRACE2("yoEncode() %d: %s\n", n, code);

		if (yoValidCode(code))			
		{
			// set initial conditions
			for (i = 0; i < 17; i++)
			{
				if (code[i] >= '0' && code[i] <= '9')
				{
					a[i] = (double)((int)code[i] - 48);
				}
				else if (toupper(code[i]) >= 'A' && toupper(code[i]) <= 'F')
				{
					a[i] = (double)((int)code[i] - 55);
				}
				else if (code[i] == '-' && i > 1)
				{
					a[i] = (double)((int)(a[i - 1] + a[i - 2]) % 16);
				}
			}

			for (i = 0; i < 17; i++)
			{
				// set initial condition between 0.0 and 1.0
				a[i] = 0.03125 + (0.0625 * a[i]);
			}

			for (i = 0; i < 17; i++)
			{
				// pick entry to remove
				if (i == 15)
				{
					deleteEntry = 15 - int(a[i]);
				}
				
				// set r for each iteration
				r = 3.8839 + (0.1 * a[16 - i]);

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
				
				//r = 3.8839 + (0.1 * a[i]);
				//r = 3.8839 + 0.1 * (a[i] / 26.0);

			}

			for (i = deleteEntry; i < 16; i++)
			{
				transCode[i] = transCode[i + 1];
			}

			// shuffle entries
			transCode[16] = transCode[0];
			transCode[0] = transCode[13];
			transCode[13] = transCode[11];
			transCode[11] = transCode[2];
			transCode[2] = transCode[8];
			transCode[8] = transCode[14];
			transCode[14] = transCode[1];
			transCode[1] = transCode[5];
			transCode[5] = transCode[6];
			transCode[6] = transCode[15];
			transCode[15] = transCode[3];
			transCode[3] = transCode[12];
			transCode[12] = transCode[10];
			transCode[10] = transCode[9];
			transCode[9] = transCode[7];
			transCode[7] = transCode[4];
			transCode[4] = transCode[16];
			transCode[16] = '\0';

			// manipulate entries dependent on other values in translation code
			// change first w to a g
			for (i = 0; i < 16; i++)
			{
				if (transCode[i] == 'w')
				{
					transCode[i] = 'g';
					break;
				}
			}
			// change first x to an h
			for (i = 0; i < 16; i++)
			{
				if (transCode[i] == 'x')
				{
					transCode[i] = 'h';
					break;
				}
			}
			// change first y to an r
			for (i = 0; i < 16; i++)
			{
				if (transCode[i] == 'y')
				{
					transCode[i] = 'r';
					break;
				}
			}
			// change first z to a k
			for (i = 0; i < 16; i++)
			{
				if (transCode[i] == 'z')
				{
					transCode[i] = 'k';
					break;
				}
			}

			// change i + 8th entry based on i and i + 8
			for (i = 0; i < 16; i++)
			{
				lic[n][(i + 8) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 8) % 16] - 97 + (i % 3)) % 26) + 97);
			}
			for (i = 0; i < 16; i++)
			{
				transCode[i] = lic[n][i];
			}

			// change first a to an m
			for (i = 0; i < 16; i++)
			{
				if (transCode[i] == 'a')
				{
					transCode[i] = 'm';
					break;
				}
			}

			// change i + 3rd entry based on i and i + 3
			for (i = 0; i < 16; i++)
			{
				lic[n][(i + 3) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 3) % 16] - 97 + (i % 7)) % 26) + 97);
			}
			for (i = 0; i < 16; i++)
			{
				transCode[i] = lic[n][i];
			}

			// change first b to an s
			for (i = 0; i < 16; i++)
			{
				if (transCode[i] == 'b')
				{
					transCode[i] = 's';
					break;
				}
			}

			// change i + 11th entry based on i and i + 11
			for (i = 0; i < 16; i++)
			{
				if (((int)transCode[i] - 97) % 3 != 1)
				{
					lic[n][(i + 11) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 11) % 16] - 97 + (i % 1)) % 26) + 97);
				}
			}
			for (i = 0; i < 16; i++)
			{
				transCode[i] = lic[n][i];
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
					lic[n][(i + 7) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 7) % 16] - 97) % 26) + 97);
				}
			}
			for (i = 0; i < 16; i++)
			{
				transCode[i] = lic[n][i];
			}

			// remove duplicates chars
			for (i = 0; i < 26; i++)
			{
				charUsed[i] = FALSE;
			}
			for (i = 0; i < 16; i++)
			{
				charUsed[(int)transCode[i] - 97] = TRUE;
			}

			k = 0;
			for (i = 0; i < 15; i++)
			{
				for (j = i + 1; j < 16; j++)
				{
					if (transCode[j] == transCode[i])
					{
						for ( ; k < 26 && charUsed[k] == TRUE; k++);
						charUsed[k] = TRUE;
						transCode[j] = (char)(k + 97);
					}
				}
			}

			// set license
			for (i = 0; i < 16; i++)
			{
				lic[n][i] = transCode[i];
			}
			lic[n][16] = '\0';
			TRACE2("lic[%d] = %s\n", n, lic[n]);

			//if (strcmp(m_license, ADDRESS_LICENSE) == 0)
			//if (strcmp(m_license, license) == 0)
			//{
				//sprintf(m_code[0], "%s", code);
			//	return TRUE;
			//}
		}

	}

	sprintf(m_license, "%s %s %s", lic[0], lic[1], lic[2]);

	// see if generated licenses match license values passed in
	sscanf(license, "%s %s %s", licDLL[0], licDLL[1], licDLL[2]);

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (lic[i][0] != '\0' && licDLL[j][0] != '\0'
				&& strcmp(lic[i], licDLL[j]) == 0)
			{
				return TRUE;
			}
		}

	}

	return FALSE;
}


BOOL CYoLicense::yoGenerateInitializationCode()
// generate the initialization code based on the current system time
// and assign the code to m_initializationCode
// needed to obtain the temporary key
// year = 9783 - year  (hex) (4 chars) pos 13 8 15 3
// month = month (hex) (1 char) pos 7
// day = 89 - day (hex) (2 chars) pos 2 9
// hour = 55 - hour (dec) (2 chars) pos 14 12
// min = 69 - min (hex) (2 chars) pos 4 11
// sec = 91 - sec (dec) (2 chars) pos 10 6
// ms = ms (hex) (3 chars) pos 1 0 5
{
	SYSTEMTIME gmSystemTime;
	CTime timeCurrent;
	char s[8];

	::GetSystemTime(&gmSystemTime);
	timeCurrent = gmSystemTime;

	sprintf(s, "%04x", 9783 - timeCurrent.GetYear());
	m_initializationCode[13] = s[0];
	m_initializationCode[8] = s[1];
	m_initializationCode[15] = s[2];
	m_initializationCode[3] = s[3];

	sprintf(s, "%x", timeCurrent.GetMonth());
	m_initializationCode[7] = s[0];

	sprintf(s, "%02x", 89 - timeCurrent.GetDay());
	m_initializationCode[2] = s[0];
	m_initializationCode[9] = s[1];

	sprintf(s, "%02d", 55 - timeCurrent.GetHour());
	m_initializationCode[14] = s[0];
	m_initializationCode[12] = s[1];

	sprintf(s, "%02x", 69 - timeCurrent.GetMinute());
	m_initializationCode[4] = s[0];
	m_initializationCode[11] = s[1];

	sprintf(s, "%02d", 91 - timeCurrent.GetSecond());
	m_initializationCode[10] = s[0];
	m_initializationCode[6] = s[1];

	sprintf(s, "%03x", gmSystemTime.wMilliseconds);
	m_initializationCode[1] = s[0];
	m_initializationCode[0] = s[1];
	m_initializationCode[5] = s[2];

	m_initializationCode[16] = '\0';
	TRACE1("m_initializationCode = %s\n", m_initializationCode);

	return TRUE;
}


BOOL CYoLicense::yoInitializeTemporaryLicense(char expirationCode, char message[256])
// initialize the application's registry entries for temporary licensing
// entries will be labeled "first", "second", "third", "fourth", and "fifth"
// yoValidateTemporaryLicense() will be called after registry is initialized
// expirationCode is generated using a dialog based application called license

{
	HKEY hk;
	DWORD dwDisp;
	DWORD type = REG_SZ;
	DWORD size = 512;
	char s[512];
	char t[512];
	double a;
	double a0;
	double b;
	double c;
	double r;
	int i;
	int j;
	char u[16];
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
	int ms;

	sprintf(s, "Software\\bjones\\%s", m_applicationName);
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, s,
		0, "", REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hk, &dwDisp)
		!= ERROR_SUCCESS)
	{
		sprintf(message, "ERROR: Could not create key in registry: %s", s);
		return FALSE;
	}

	u[0] = m_initializationCode[13];
	u[1] = m_initializationCode[8];
	u[2] = m_initializationCode[15];
	u[3] = m_initializationCode[3];
	u[4] = '\0';
	sscanf(u, "%x", &year);
	
	u[0] = m_initializationCode[7];
	u[1] = '\0';
	sscanf(u, "%x", &month);

	u[0] = m_initializationCode[2];
	u[1] = m_initializationCode[9];
	u[2] = '\0';
	sscanf(u, "%x", &day);

	u[0] = m_initializationCode[14];
	u[1] = m_initializationCode[12];
	u[2] = '\0';
	sscanf(u, "%d", &hour);

	u[0] = m_initializationCode[4];
	u[1] = m_initializationCode[11];
	u[2] = '\0';
	sscanf(u, "%x", &min);

	u[0] = m_initializationCode[10];
	u[1] = m_initializationCode[6];
	u[2] = '\0';
	sscanf(u, "%d", &sec);

	u[0] = m_initializationCode[1];
	u[1] = m_initializationCode[0];
	u[2] = m_initializationCode[5];
	u[3] = '\0';
	sscanf(u, "%x", &ms);

	// first registry (start date)
	r = 3.800825 + (0.001 * (double)((int)m_initializationCode[0] % 100));
	a0 = 0.270021 + (0.0001 * (double)((int)m_initializationCode[6] % 100));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';
	// number of chars to skip starts at s[18]
	// start date begins at s[120]

	// year
	j = 120;
	j += (int)s[18] - 48 + 1;
	sprintf(u, "%04d", year);
	t[j] = u[0];
	j += (int)s[19] - 48 + 1;
	t[j] = u[1];
	j += (int)s[20] - 48 + 1;
	t[j] = u[2];
	j += (int)s[21] - 48 + 1;
	t[j] = u[3];

	// month
	sprintf(u, "%02d", month);
	j += (int)s[22] - 48 + 1;
	t[j] = u[0];
	j += (int)s[23] - 48 + 1;
	t[j] = u[1];

	// day
	sprintf(u, "%02d", day);
	j += (int)s[24] - 48 + 1;
	t[j] = u[0];
	j += (int)s[25] - 48 + 1;
	t[j] = u[1];

	// hour
	sprintf(u, "%02d", hour);
	j += (int)s[26] - 48 + 1;
	t[j] = u[0];
	j += (int)s[27] - 48 + 1;
	t[j] = u[1];

	// min
	sprintf(u, "%02d", min);
	j += (int)s[28] - 48 + 1;
	t[j] = u[0];
	j += (int)s[29] - 48 + 1;
	t[j] = u[1];

	// sec
	sprintf(u, "%02d", sec);
	j += (int)s[30] - 48 + 1;
	t[j] = u[0];
	j += (int)s[31] - 48 + 1;
	t[j] = u[1];

	// ms
	sprintf(u, "%03d", ms);
	j += (int)s[32] - 48 + 1;
	t[j] = u[0];
	j += (int)s[33] - 48 + 1;
	t[j] = u[1];
	j += (int)s[34] - 48 + 1;
	t[j] = u[2];

	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[35] - 48 + 1;
	t[j] = u[2];
	j += (int)s[36] - 48 + 1;
	t[j] = u[3];
	j += (int)s[37] - 48 + 1;
	t[j] = u[4];
	j += (int)s[38] - 48 + 1;
	t[j] = u[5];
	j += (int)s[39] - 48 + 1;
	t[j] = u[6];
	j += (int)s[40] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[41] - 48 + 1;
	t[j] = u[2];
	j += (int)s[42] - 48 + 1;
	t[j] = u[3];
	j += (int)s[43] - 48 + 1;
	t[j] = u[4];
	j += (int)s[44] - 48 + 1;
	t[j] = u[5];
	j += (int)s[45] - 48 + 1;
	t[j] = u[6];
	j += (int)s[46] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "first", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		sprintf(message, "ERROR: Could not set value in registry");
		return FALSE;
	}

	// second registry (days to expiration)
	r = 3.950013 + (0.0001 * (double)((int)m_initializationCode[1] % 100));
	a0 = 0.360092 + (0.0001 * (double)((int)m_initializationCode[10] % 100));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[24]
	// days to expiration begins at s[282]

	// expiration codes
	// a = 360 days
	// b = 180 days
	// c = 90 days
	// d = 60 days
	// e = 30 days
	// f = 7 days
	// g and other chars = 1 day

	if (expirationCode == 'a')
	{
		sprintf(u, "360");
	}
	else if (expirationCode == 'b')
	{
		sprintf(u, "180");
	}
	else if (expirationCode == 'c')
	{
		sprintf(u, "090");
	}
	else if (expirationCode == 'd')
	{
		sprintf(u, "060");
	}
	else if (expirationCode == 'e')
	{
		sprintf(u, "030");
	}
	else if (expirationCode == 'f')
	{
		sprintf(u, "007");
	}
	else
	{
		sprintf(u, "001");
	}


	j = 282;
	j += (int)s[24] - 48 + 1;
	t[j] = u[0];
	j += (int)s[25] - 48 + 1;
	t[j] = u[1];
	j += (int)s[26] - 48 + 1;
	t[j] = u[2];

	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[27] - 48 + 1;
	t[j] = u[2];
	j += (int)s[28] - 48 + 1;
	t[j] = u[3];
	j += (int)s[29] - 48 + 1;
	t[j] = u[4];
	j += (int)s[30] - 48 + 1;
	t[j] = u[5];
	j += (int)s[31] - 48 + 1;
	t[j] = u[6];
	j += (int)s[32] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[33] - 48 + 1;
	t[j] = u[2];
	j += (int)s[34] - 48 + 1;
	t[j] = u[3];
	j += (int)s[35] - 48 + 1;
	t[j] = u[4];
	j += (int)s[36] - 48 + 1;
	t[j] = u[5];
	j += (int)s[37] - 48 + 1;
	t[j] = u[6];
	j += (int)s[38] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "second", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		sprintf(message, "ERROR: Could not set value in registry");
		return FALSE;
	}

	// third registry (session start date)
	r = 3.860025 + (0.0001 * (double)((int)m_initializationCode[6] % 100));
	a0 = 0.400317 + (0.001 * (double)((int)m_initializationCode[5] % 100));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[66]
	// session start date begins at s[186]

	// year
	j = 186;
	j += (int)s[66] - 48 + 1;
	sprintf(u, "%04d", year);
	t[j] = u[0];
	j += (int)s[67] - 48 + 1;
	t[j] = u[1];
	j += (int)s[68] - 48 + 1;
	t[j] = u[2];
	j += (int)s[69] - 48 + 1;
	t[j] = u[3];

	// month
	sprintf(u, "%02d", month);
	j += (int)s[70] - 48 + 1;
	t[j] = u[0];
	j += (int)s[71] - 48 + 1;
	t[j] = u[1];

	// day
	sprintf(u, "%02d", day);
	j += (int)s[72] - 48 + 1;
	t[j] = u[0];
	j += (int)s[73] - 48 + 1;
	t[j] = u[1];

	// hour
	sprintf(u, "%02d", hour);
	j += (int)s[74] - 48 + 1;
	t[j] = u[0];
	j += (int)s[75] - 48 + 1;
	t[j] = u[1];

	// min
	sprintf(u, "%02d", min);
	j += (int)s[76] - 48 + 1;
	t[j] = u[0];
	j += (int)s[77] - 48 + 1;
	t[j] = u[1];

	// sec
	sprintf(u, "%02d", sec);
	j += (int)s[78] - 48 + 1;
	t[j] = u[0];
	j += (int)s[79] - 48 + 1;
	t[j] = u[1];

	// ms
	sprintf(u, "%03d", ms);
	j += (int)s[80] - 48 + 1;
	t[j] = u[0];
	j += (int)s[81] - 48 + 1;
	t[j] = u[1];
	j += (int)s[82] - 48 + 1;
	t[j] = u[2];

	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[83] - 48 + 1;
	t[j] = u[2];
	j += (int)s[84] - 48 + 1;
	t[j] = u[3];
	j += (int)s[85] - 48 + 1;
	t[j] = u[4];
	j += (int)s[86] - 48 + 1;
	t[j] = u[5];
	j += (int)s[87] - 48 + 1;
	t[j] = u[6];
	j += (int)s[88] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[89] - 48 + 1;
	t[j] = u[2];
	j += (int)s[90] - 48 + 1;
	t[j] = u[3];
	j += (int)s[91] - 48 + 1;
	t[j] = u[4];
	j += (int)s[92] - 48 + 1;
	t[j] = u[5];
	j += (int)s[93] - 48 + 1;
	t[j] = u[6];
	j += (int)s[94] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "third", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		sprintf(message, "ERROR: Could not set value in registry");
		return FALSE;
	}

	// fourth registry (session end date)
	r = 3.900761 + (0.001 * (double)((int)m_initializationCode[1] % 100));
	a0 = 0.730021 + (0.0001 * (double)((int)m_initializationCode[11] % 100));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[126]
	// session end date begins at s[234]

	// year
	j = 234;
	j += (int)s[126] - 48 + 1;
	sprintf(u, "%04d", year);
	t[j] = u[0];
	j += (int)s[127] - 48 + 1;
	t[j] = u[1];
	j += (int)s[128] - 48 + 1;
	t[j] = u[2];
	j += (int)s[129] - 48 + 1;
	t[j] = u[3];

	// month
	sprintf(u, "%02d", month);
	j += (int)s[130] - 48 + 1;
	t[j] = u[0];
	j += (int)s[131] - 48 + 1;
	t[j] = u[1];

	// day
	sprintf(u, "%02d", day);
	j += (int)s[132] - 48 + 1;
	t[j] = u[0];
	j += (int)s[133] - 48 + 1;
	t[j] = u[1];

	// hour
	sprintf(u, "%02d", hour);
	j += (int)s[134] - 48 + 1;
	t[j] = u[0];
	j += (int)s[135] - 48 + 1;
	t[j] = u[1];

	// min
	sprintf(u, "%02d", min);
	j += (int)s[136] - 48 + 1;
	t[j] = u[0];
	j += (int)s[137] - 48 + 1;
	t[j] = u[1];

	// sec
	sprintf(u, "%02d", sec);
	j += (int)s[138] - 48 + 1;
	t[j] = u[0];
	j += (int)s[139] - 48 + 1;
	t[j] = u[1];

	// ms
	sprintf(u, "%03d", ms);
	j += (int)s[140] - 48 + 1;
	t[j] = u[0];
	j += (int)s[141] - 48 + 1;
	t[j] = u[1];
	j += (int)s[142] - 48 + 1;
	t[j] = u[2];

	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[143] - 48 + 1;
	t[j] = u[2];
	j += (int)s[144] - 48 + 1;
	t[j] = u[3];
	j += (int)s[145] - 48 + 1;
	t[j] = u[4];
	j += (int)s[146] - 48 + 1;
	t[j] = u[5];
	j += (int)s[147] - 48 + 1;
	t[j] = u[6];
	j += (int)s[148] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[149] - 48 + 1;
	t[j] = u[2];
	j += (int)s[150] - 48 + 1;
	t[j] = u[3];
	j += (int)s[151] - 48 + 1;
	t[j] = u[4];
	j += (int)s[152] - 48 + 1;
	t[j] = u[5];
	j += (int)s[153] - 48 + 1;
	t[j] = u[6];
	j += (int)s[154] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "fourth", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		sprintf(message, "ERROR: Could not set value in registry");
		return FALSE;
	}

	// fifth registry (number of executions)
	r = 3.890052 + (0.0001 * (double)((int)m_initializationCode[5] % 100));
	a0 = 0.400192 + (0.001 * (double)((int)m_initializationCode[6] % 100));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[162]
	// clock sync begins at s[300]
	// 86400 seconds in a day

	sprintf(u, "00000");

	j = 300;
	//j += (int)s[162] - 48 + 1;
	//t[j] = u[0];
	//j += (int)s[163] - 48 + 1;
	//t[j] = u[1];
	//j += (int)s[164] - 48 + 1;
	//t[j] = u[2];
	//j += (int)s[165] - 48 + 1;
	//t[j] = u[3];
	//j += (int)s[166] - 48 + 1;
	//t[j] = u[4];

	// number of sessions (up to 999999)
	sprintf(u, "000000");
	j += (int)s[162] - 48 + 1;
	t[j] = u[0];
	j += (int)s[163] - 48 + 1;
	t[j] = u[1];
	j += (int)s[164] - 48 + 1;
	t[j] = u[2];
	j += (int)s[165] - 48 + 1;
	t[j] = u[3];
	j += (int)s[166] - 48 + 1;
	t[j] = u[4];
	j += (int)s[167] - 48 + 1;
	t[j] = u[5];


	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[168] - 48 + 1;
	t[j] = u[2];
	j += (int)s[169] - 48 + 1;
	t[j] = u[3];
	j += (int)s[170] - 48 + 1;
	t[j] = u[4];
	j += (int)s[171] - 48 + 1;
	t[j] = u[5];
	j += (int)s[172] - 48 + 1;
	t[j] = u[6];
	j += (int)s[173] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[174] - 48 + 1;
	t[j] = u[2];
	j += (int)s[175] - 48 + 1;
	t[j] = u[3];
	j += (int)s[176] - 48 + 1;
	t[j] = u[4];
	j += (int)s[177] - 48 + 1;
	t[j] = u[5];
	j += (int)s[178] - 48 + 1;
	t[j] = u[6];
	j += (int)s[179] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "fifth", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		sprintf(message, "ERROR: Could not set value in registry");
		return FALSE;
	}

	return yoValidateTemporaryLicense(message);

}


BOOL CYoLicense::yoLicenseHDSN()
{
// Finds the hard disk serial number of the C: drive on this computer and assigns
// it to m_code in a format used by the MAC address.  DD-xx-xx-xx-xx-xx
// It is best to call this routine prior to calling yoLicenseMACFile() or yoLicenseMACPipe()
// because it's best that HDSN is assigned to code[0].

	int i;
	int k;
	char hdsn[24];
	DWORD dwSerialNumber;

	// find first vacant m_code[]
	for (k = 0; k < 2 && m_code[k][0] != '\0'; k++);

	// write hard disk serial number into m_code[0]
    if (!::GetVolumeInformation ("C:\\", NULL, 0,
        &dwSerialNumber, NULL, NULL, NULL, 0))
	{
		return FALSE;
	}
	else
	{
		TRACE2("hard disk serial number = %x %d\n", dwSerialNumber, dwSerialNumber);
		sprintf(hdsn, "%x", dwSerialNumber);
		for (i = 0; i < 8; i++)
		{
			// convert a-f to uppercase
			if (hdsn[i] >= 'a' && hdsn[i] <= 'f')
			{
				hdsn[i] = toupper(hdsn[i]);
			}
		}

		m_code[k][0] = 'D';  // signifies that code is a hard disk serial number
		m_code[k][1] = 'D';  // signifies that code is a hard disk serial number
		m_code[k][2] = '-';
		m_code[k][3] = hdsn[0];
		m_code[k][4] = hdsn[1];
		m_code[k][5] = '-';
		m_code[k][6] = hdsn[2];
		m_code[k][7] = hdsn[3];
		m_code[k][8] = '-';
		m_code[k][9] = hdsn[4];
		m_code[k][10] = hdsn[5];
		m_code[k][11] = '-';
		m_code[k][12] = hdsn[6];
		m_code[k][13] = hdsn[7];
		m_code[k][14] = '-';
		m_code[k][15] = hdsn[4];  // repeat 5th entry
		m_code[k][16] = hdsn[7];  // repeat 8th entry
		m_code[k][17] = '\0';
	}
	return TRUE;
}


BOOL CYoLicense::yoLicenseMACFile()
// Finds the MAC address of the ethernet card(s) on this computer and assigns each
// found to m_code.  Runs winipcfg.exe and writes stdout to a license file.
// Needed for Windows 95 OS.
{
	HANDLE hSaveStdout;
	HANDLE hSaveStdin;
	DWORD dwProcessId = DWORD(-1);
	CWinThread *pReadThread = NULL;
	DWORD exitCode = STILL_ACTIVE;
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo; 
	char s[4096];
	char licFile[256];
	char cmd[256];
	BOOL ret;
	int i;
	int j;
	int k;
	int ch;
	char *t;
	FILE *fp;

	if ((fp = fopen("C:\\Temp\\lic1616yoLicX1619.txt", "w")) != NULL)
	{
		fclose(fp);
		sprintf(licFile, "C:\\Temp\\lic1616yoLicX1619.txt");
	}
	else if ((fp = fopen("C:\\Windows\\Temp\\lic1616yoLicX1619.txt", "w")) != NULL)
	{
		fclose(fp);
		sprintf(licFile, "C:\\Temp\\lic1616yoLicX1619.txt");
	}
	else if ((fp = fopen("lic1616yoLicX1619.txt", "w")) != NULL)
	{
		fclose(fp);
		sprintf(licFile, "lic1616yoLicX1619.txt");
	}
	else
	{
		return FALSE;
	}

	hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);
 
	// Set up members of STARTUPINFO structure. 
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);

	sprintf(cmd, "winipcfg.exe /All /Batch %s", licFile);

	// Create the child process. 
	ret = CreateProcess(NULL,
		cmd,				// command line 
		NULL,				// process security attributes 
		NULL,				// primary thread security attributes 
		TRUE,				// handles are inherited 
		DETACHED_PROCESS,	// creation flags--don't show console window
		NULL,				// use parent's environment 
		NULL,				// use parent's current directory 
		&siStartInfo,		// STARTUPINFO pointer 
		&piProcInfo);		// receives PROCESS_INFORMATION 

	if (ret)
	{
		dwProcessId = piProcInfo.dwProcessId;
	}
	else
	{
		return FALSE;
	}

	// Wait for winipcfg.exe to finish before continuing
	i = 0;
	while (exitCode == STILL_ACTIVE)
	{
		Sleep(100);
		TRACE3("%d: %d %d\n", i, dwProcessId, exitCode);
		GetExitCodeProcess(piProcInfo.hProcess, &exitCode);
		i++;
		if (i == 50) // avoid infinite loop--5 sec delay
		{
			return FALSE;
		}
	}

	// After process creation, restore the saved STDIN and STDOUT.
	if( !SetStdHandle(STD_INPUT_HANDLE, hSaveStdin) )
	{
		return FALSE;
	}
	if( !SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout) )
	{
		return FALSE;
	}

	if ((fp = fopen(licFile, "r")) == NULL)
	{
		return FALSE;
	}

	for (i = 0; (i < 4096) && ((ch = fgetc(fp)) != EOF); i++)
	{
		s[i] = (char)ch;
		TRACE2("%d: %c\n", i, s[i]);
	}
	s[i] = (char)ch;
	TRACE2("%d: %c\n", i, s[i]);

	// remove contents of license file in case it can't be deleted
	fclose(fp);
	if ((fp = fopen(licFile, "w")) == NULL)
	{
		return FALSE;
	}
	fprintf(fp, "license file is empty");
	fclose(fp);

	// delete license file
	CFile::Remove(licFile);


	// find first vacant m_code[]
	for (k = 0; k < 2 && m_code[k][0] != '\0'; k++);
	i = 0;

	// write any MAC address found into m_code[k]
	for (t = s; i < 4096 && k < 3; t++)
	{
		if (*t == '-' && *(t + 3) == '-' && *(t + 6) == '-'
			&& *(t + 9) == '-' && *(t + 12) == '-' && i - 2 >= 0
			&& yoValidCode(t - 2))
		{
			for (j = 0; j < 17; j++)
			{
				m_code[k][j] = *(t - 2 + j);
			}
			m_code[k][17] = '\0';
			k++;

		}
		i++;
	}

	return TRUE;
}


BOOL CYoLicense::yoLicenseMACPipe()
// Finds the MAC address of ethernet card(s) on this computer and assigns
// each address found to m_code.  Runs ipconfig.exe and pipes stdout to this
// function to test for valid ethernet card address.
{

	HANDLE hChildStdoutRd;
	HANDLE hChildStdoutWr;
	HANDLE hChildStdoutRdDup;
	HANDLE hSaveStdout;
	HANDLE hStdout;

	DWORD dwProcessId = DWORD(-1);
	DWORD exitCode = STILL_ACTIVE;

	DWORD dwRead;
	char s[4096];
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo; 
	SECURITY_ATTRIBUTES saAttr; 
	BOOL fSuccess; 
	//char license[256];
	int i;
	int j;
	int k;
	char *t;

	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
 
	// The steps for redirecting child process's STDOUT: 
	//     1. Save current STDOUT, to be restored later. 
	//     2. Create anonymous pipe to be STDOUT for child process. 
	//     3. Set STDOUT of the parent process to be write handle to 
	//        the pipe, so it is inherited by the child process. 
	//     4. Create a noninheritable duplicate of the read handle and
	//        close the inheritable read handle. 
 
	// Save the handle to the current STDOUT. 
	if ((hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE)
	{
		//return FALSE;
	}
 
	// Create a pipe for the child process's STDOUT. 
	if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) 
	{
	   return FALSE;
	}
 
	// Set a write handle to the pipe to be STDOUT. 
	if (! SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr))
	{
		return FALSE;
	}
 
	// Create noninheritable read handle and close the
	// inheritable read handle. 
	fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd,
		GetCurrentProcess(), &hChildStdoutRdDup , 0,
		FALSE, DUPLICATE_SAME_ACCESS);
    if(!fSuccess )
	{
		return FALSE;
	}
    CloseHandle(hChildStdoutRd);

	// Create the child process. 
	// Set up members of STARTUPINFO structure. 
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.dwFlags = STARTF_USESTDHANDLES;
	siStartInfo.hStdOutput = hChildStdoutWr;
	siStartInfo.hStdError = hChildStdoutWr;
 
	// Start the child process. 
	fSuccess = CreateProcess( NULL,
		"ipconfig.exe /All", // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		DETACHED_PROCESS, // creation flags--don't show console window
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

	if (fSuccess)
	{
		dwProcessId = piProcInfo.dwProcessId;
	}
	else
	{
		return FALSE;
	}

	// Wait for ipconfig.exe to finish before continuing
	i = 0;
	while (exitCode == STILL_ACTIVE)
	{
		Sleep(100);
		TRACE3("%d: %d %d\n", i, dwProcessId, exitCode);
		GetExitCodeProcess(piProcInfo.hProcess, &exitCode);
		i++;
		if (i == 50) // avoid infinite loop--5 sec delay
		{
			return FALSE;
		}
	}

	// After process creation, restore the saved STDOUT. 
	if (!SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout)) 
	{
		return FALSE;
	}
 
	// Read from pipe that is the standard output for child process. 
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	// Close the write end of the pipe before reading from the 
	// read end of the pipe. 
	if (!CloseHandle(hChildStdoutWr)) 
	{
	   return FALSE;
	}

	// Read output from the child process
	for (i = 0; ; i++) 
	{ 
		if(!ReadFile(hChildStdoutRdDup, s, 4096, &dwRead, NULL)
			|| dwRead == 0)
		{
			break;
		}
		if (i == 100) // avoid infinite loop
		{
			return FALSE;
		}
	} 

	if (!CloseHandle(hChildStdoutRdDup)) 
	{
	   return FALSE;
	}

	// find first vacant m_code[]
	for (k = 0; k < 2 && m_code[k][0] != '\0'; k++);
	i = 0;

	// write any MAC address found into m_code[k]
	for (t = s; i < 4096 && k < 3; t++)
	{
		if (*t == '-' && *(t + 3) == '-' && *(t + 6) == '-'
			&& *(t + 9) == '-' && *(t + 12) == '-' && i - 2 >= 0
			&& yoValidCode(t - 2))
		{
			for (j = 0; j < 17; j++)
			{
				m_code[k][j] = *(t - 2 + j);
			}
			m_code[k][17] = '\0';
			k++;

		}
		i++;
	}
	return TRUE;
}


/***
// Sample message box -- insert in main application
BOOL CYoLicense::yoMessage(char sMessage[256])
// displays message box containing licensing error
// option exists to enter a temporary key based on initialization code
// if temporary key is valid TRUE is returned else FALSE is returned indicating
// licensing failure
{

	CLicenseMessageDlg messageDlg;
	char s[24];
	char *t;
	int i;
	int j;

	// generate initialization code
	yoGenerateInitializationCode();

	//rpTemporaryKey();

	j = 0;
	for (i = 0; i < 16; i++)
	{
		s[j] = m_initializationCode[i];
		j++;
		if (i < 15 && (i % 4) == 3)
		{
			s[j] = ' ';
			j++;
		}
	}
	s[j] = '\0';

	messageDlg.m_staticMessage.Format("%s", (LPCTSTR)sMessage);
	messageDlg.m_initializationCode.Format("%s", (LPCTSTR)s);
	messageDlg.DoModal();

	// generate the temporary key
	yoTemporaryKey();

	sprintf(s, "%s", messageDlg.m_temporaryKey);
	// remove spaces
	i = 0;
	for (t = s; i < 24 && *t != '\0'; t++)
	{
		for (; *t == ' '; t++);
		s[i] = *t;
		i++;
	}
	s[16] = '\0';
	
	// find placeholder for expiration code
	// expiration codes
	// a = 360 days
	// b = 180 days
	// c = 90 days
	// d = 60 days
	// e = 30 days
	// f = 7 days
	// g and other chars = 1 day
	i = (((int)m_temporaryKey[0] - 97) % 14) + 1;
	m_temporaryKey[i] = s[i];

	if (strcmp(s, m_temporaryKey) == 0)
	{
h		return yoInitializeTemporaryLicense(s[i]);
	}
	else
	{
		m_temporaryKey[0] = '\0';
		return FALSE;
	}

}
***/


BOOL CYoLicense::yoTemporaryKey()
// generate a temporary key (m_temporaryKey) based on the value of m_initializationCode
// m_initialization code must first be assigned in yoGenerateInitializationCode()
{
	char transCode[17];
	double a[17];
	double b;
	double c;
	double r;
	int i;
	int j;
	char code[17];

	if (m_initializationCode[0] == '\0')
	{
		return FALSE;
	}

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
		m_temporaryKey[(i + 8) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 8) % 16] - 97 + (i % 3)) % 26) + 97);
	}
	for (i = 0; i < 16; i++)
	{
		transCode[i] = m_temporaryKey[i];
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
		m_temporaryKey[(i + 3) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 3) % 16] - 97 + (i % 7)) % 26) + 97);
	}
	for (i = 0; i < 16; i++)
	{
		transCode[i] = m_temporaryKey[i];
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
			m_temporaryKey[(i + 11) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 11) % 16] - 97 + (i % 1)) % 26) + 97);
		}
	}
	for (i = 0; i < 16; i++)
	{
		transCode[i] = m_temporaryKey[i];
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
			m_temporaryKey[(i + 7) % 16] = (char)((((int)transCode[i] - 97 + (int)transCode[(i + 7) % 16] - 97) % 26) + 97);
		}
	}

	m_temporaryKey[16] = '\0';
	TRACE1("m_temporaryKey = %s\n", m_temporaryKey);

	return TRUE;
}

BOOL CYoLicense::yoValidateTemporaryLicense(char message[256])
// validate temporary licensing with values stored in 5 registry files
// clock may be set back no more than 24 hours from last start time
// on each validation a default of 1 hour is added from the start time to the end time
// when temporary license has expired m_sessions is set to 24 * 360--the program will run one last time
{

	SYSTEMTIME gmSystemTime;
	CTime timeCurrent;
	//CTimeSpan ts;

	m_temporaryKey[0] = '\0';     // temporary license is not yet valid

	if (!yoReadRegistry())
	{
		sprintf(message, "ERROR: cannot read registry");
		return FALSE;
	}

	// determine if values in registry are valid for temporary licensing
	// get current date
	::GetSystemTime(&gmSystemTime);
	timeCurrent = gmSystemTime;

	if (timeCurrent > m_time[3])
	{
		m_time[2] = timeCurrent;
	}
	else
	{
		m_time[2] = m_time[3];
	}

	m_time[3] = m_time[2];

	if (timeCurrent > m_time[1] && m_sessions < 24 * 360)
	{
		//AfxMessageBox("Temporary license has expired.     \nThis is the last time address will run.     ");
		// set sessions so that the next time program is executed temporary licensing will fail
		sprintf(message, "Temporary license has expired.  Last time program will run");
		m_sessions = 24 * 360;
	}

	if (timeCurrent + CTimeSpan(3600 * 24) <= m_time[2]
		|| m_sessions > 24 * 360)
		//|| timeCurrent > m_time[1])
	{
		TRACE0("rpValidateTemporaryLicense() INVALID LICENSE\n");
		TRACE3("  start date:    %04d.%02d.%02d", m_time[0].GetYear(), m_time[0].GetMonth(), m_time[0].GetDay());
		TRACE3("  %02d:%02d:%02d\n", m_time[0].GetHour(), m_time[0].GetMinute(), m_time[0].GetSecond());
		TRACE3("  exp date:      %04d.%02d.%02d", m_time[1].GetYear(), m_time[1].GetMonth(), m_time[1].GetDay());
		TRACE3("  %02d:%02d:%02d\n", m_time[1].GetHour(), m_time[1].GetMinute(), m_time[1].GetSecond());
		TRACE3("  session start: %04d.%02d.%02d", m_time[2].GetYear(), m_time[2].GetMonth(), m_time[2].GetDay());
		TRACE3("  %02d:%02d:%02d\n", m_time[2].GetHour(), m_time[2].GetMinute(), m_time[2].GetSecond());
		TRACE3("  session end:   %04d.%02d.%02d", m_time[3].GetYear(), m_time[3].GetMonth(), m_time[3].GetDay());
		TRACE3("  %02d:%02d:%02d\n", m_time[3].GetHour(), m_time[3].GetMinute(), m_time[3].GetSecond());
		TRACE3("  time current:  %04d.%02d.%02d", timeCurrent.GetYear(), timeCurrent.GetMonth(), timeCurrent.GetDay());
		TRACE3("  %02d:%02d:%02d\n", timeCurrent.GetHour(), timeCurrent.GetMinute(), timeCurrent.GetSecond());
		TRACE1("  m_sessions = %d\n", m_sessions);

		sprintf(message, "ERROR: cannot read registry");
		return FALSE;
	}

	m_sessions++;

	TRACE0("rpValidateTemporaryLicense()\n");
	TRACE3("  start date:    %04d.%02d.%02d", m_time[0].GetYear(), m_time[0].GetMonth(), m_time[0].GetDay());
	TRACE3("  %02d:%02d:%02d\n", m_time[0].GetHour(), m_time[0].GetMinute(), m_time[0].GetSecond());
	TRACE3("  exp date:      %04d.%02d.%02d", m_time[1].GetYear(), m_time[1].GetMonth(), m_time[1].GetDay());
	TRACE3("  %02d:%02d:%02d\n", m_time[1].GetHour(), m_time[1].GetMinute(), m_time[1].GetSecond());
	TRACE3("  session start: %04d.%02d.%02d", m_time[2].GetYear(), m_time[2].GetMonth(), m_time[2].GetDay());
	TRACE3("  %02d:%02d:%02d\n", m_time[2].GetHour(), m_time[2].GetMinute(), m_time[2].GetSecond());
	TRACE3("  session end:   %04d.%02d.%02d", m_time[3].GetYear(), m_time[3].GetMonth(), m_time[3].GetDay());
	TRACE3("  %02d:%02d:%02d\n", m_time[3].GetHour(), m_time[3].GetMinute(), m_time[3].GetSecond());
	TRACE3("  time current:  %04d.%02d.%02d", timeCurrent.GetYear(), timeCurrent.GetMonth(), timeCurrent.GetDay());
	TRACE3("  %02d:%02d:%02d\n", timeCurrent.GetHour(), timeCurrent.GetMinute(), timeCurrent.GetSecond());
	TRACE1("  m_sessions = %d\n", m_sessions);

	// temporaryKey is valid if it is not empty string
	// place expiration date into m_temporaryKey
	sprintf(m_temporaryKey, "%04d %02d %02d",
		m_time[1].GetYear(), m_time[1].GetMonth(), m_time[1].GetDay());

	// touch registry with new values for m_time[]
	if (!yoWriteRegistry())
	{
		sprintf(message, "ERROR: cannot write registry");
		// don't return FALSE
	}
	else
	{
		sprintf(message, "SUCCESS: temporary license expires on: %s", m_temporaryKey);
	}

	return TRUE;
}


BOOL CYoLicense::yoReadRegistry(void)
// reads registry entries for temporary licensing into m_time[] variables
// does not validate the entries
{
	HKEY hk;
	DWORD type = REG_SZ;
	DWORD size = 512;
	char reg[5][512];
	char s[512];
	double a;
	double a0[5];
	double b;
	double c;
	double r[5];
	int i;
	int j;
	int k;
	char u[16];
	int year[4];
	int month[4];
	int day[4];
	int hour[4];
	int min[4];
	int sec[4];
	int ms[4];
	int expDays = 1;
	//int clockSync = 86400;
	//int sessions = 999999;
	//SYSTEMTIME gmSystemTime;
	//CTime timeCurrent;
	CTimeSpan ts;

	sprintf(s, "Software\\bjones\\%s", m_applicationName);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, s,
		0, KEY_READ | KEY_WRITE, &hk)
		!= ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegQueryValueEx(hk, "first", 0, &type, (BYTE *)reg[0], &size) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	if (RegQueryValueEx(hk, "second", 0, &type, (BYTE *)reg[1], &size) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	if (RegQueryValueEx(hk, "third", 0, &type, (BYTE *)reg[2], &size) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	if (RegQueryValueEx(hk, "fourth", 0, &type, (BYTE *)reg[3], &size) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	if (RegQueryValueEx(hk, "fifth", 0, &type, (BYTE *)reg[4], &size) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (strlen(reg[0]) != 510 && strlen(reg[1]) != 510 && strlen(reg[2]) != 510
		&& strlen(reg[3]) != 510 && strlen(reg[4]) != 510)
	{
		return FALSE;
	}


	// first registry (start date)
	// number of chars to skip starts at s[18]
	// start date begins at s[120]
	k = 18;
	j = 120;

	// year
	for (i = 0; i < 4; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[4] = '\0';
	sscanf(u, "%d", &year[0]);
	year[0] = 9783 - year[0];

	// month
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &month[0]);

	// day
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &day[0]);
	day[0] = 89 - day[0];

	// hour
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &hour[0]);
	hour[0] = 55 - hour[0];

	// min
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &min[0]);
	min[0] = 69 - min[0];

	// sec
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &sec[0]);
	sec[0] = 91 - sec[0];

	// ms
	for (i = 0; i < 3; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[3] = '\0';
	sscanf(u, "%d", &ms[0]);

	// a0
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &a0[0]);
	a0[0] *= 0.000001;

	// r
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[0][k++] - 48 + 1;
		u[i] = reg[0][j];
		reg[0][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &r[0]);
	r[0] = 3.0 + (r[0] * 0.000001);

	// validate registry
	for (i = 0; i < 85; i++)
	{
		a = a0[0]; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r[0] * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		if (reg[0][i] == 'x')
		{
			s[i] = 'x';
		}
	}

	if (strcmp(s, reg[0]) != 0)
	{
		return FALSE;
	}


	// second registry (days to expiration)
	// number of chars to skip starts at s[24]
	// days to expiration begins at s[282]
	k = 24;
	j = 282;

	// expDays
	for (i = 0; i < 3; i++)
	{
		j += (int)reg[1][k++] - 48 + 1;
		u[i] = reg[1][j];
		reg[1][j] = 'x';
	}
	u[3] = '\0';
	sscanf(u, "%d", &expDays);

	// a0
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[1][k++] - 48 + 1;
		u[i] = reg[1][j];
		reg[1][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &a0[1]);
	a0[1] *= 0.000001;

	// r
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[1][k++] - 48 + 1;
		u[i] = reg[1][j];
		reg[1][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &r[1]);
	r[1] = 3.0 + (r[1] * 0.000001);

	// validate registry
	for (i = 0; i < 85; i++)
	{
		a = a0[1]; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r[1] * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		if (reg[1][i] == 'x')
		{
			s[i] = 'x';
		}
	}

	if (strcmp(s, reg[1]) != 0)
	{
		return FALSE;
	}

	// third registry (session start date)
	// number of chars to skip starts at s[66]
	// session start date begins at s[186]
	k = 66;
	j = 186;

	// year
	for (i = 0; i < 4; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[4] = '\0';
	sscanf(u, "%d", &year[2]);
	year[2] = 9783 - year[2];

	// month
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &month[2]);

	// day
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &day[2]);
	day[2] = 89 - day[2];

	// hour
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &hour[2]);
	hour[2] = 55 - hour[2];

	// min
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &min[2]);
	min[2] = 69 - min[2];

	// sec
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &sec[2]);
	sec[2] = 91 - sec[2];

	// ms
	for (i = 0; i < 3; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[3] = '\0';
	sscanf(u, "%d", &ms[2]);

	// a0
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &a0[2]);
	a0[2] *= 0.000001;

	// r
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[2][k++] - 48 + 1;
		u[i] = reg[2][j];
		reg[2][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &r[2]);
	r[2] = 3.0 + (r[2] * 0.000001);

	// validate registry
	for (i = 0; i < 85; i++)
	{
		a = a0[2]; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r[2] * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		if (reg[2][i] == 'x')
		{
			s[i] = 'x';
		}
	}

	if (strcmp(s, reg[2]) != 0)
	{
		return FALSE;
	}


	// fourth registry (session end date)
	// number of chars to skip starts at s[126]
	// session end date begins at s[234]
	k = 126;
	j = 234;

	// year
	for (i = 0; i < 4; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[4] = '\0';
	sscanf(u, "%d", &year[3]);
	year[3] = 9783 - year[3];

	// month
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &month[3]);

	// day
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &day[3]);
	day[3] = 89 - day[3];

	// hour
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &hour[3]);
	hour[3] = 55 - hour[3];

	// min
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &min[3]);
	min[3] = 69 - min[3];

	// sec
	for (i = 0; i < 2; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[2] = '\0';
	sscanf(u, "%d", &sec[3]);
	sec[3] = 91 - sec[3];

	// ms
	for (i = 0; i < 3; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[3] = '\0';
	sscanf(u, "%d", &ms[3]);

	// a0
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &a0[3]);
	a0[3] *= 0.000001;

	// r
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[3][k++] - 48 + 1;
		u[i] = reg[3][j];
		reg[3][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &r[3]);
	r[3] = 3.0 + (r[3] * 0.000001);

	// validate registry
	for (i = 0; i < 85; i++)
	{
		a = a0[3]; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r[3] * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		if (reg[3][i] == 'x')
		{
			s[i] = 'x';
		}
	}

	if (strcmp(s, reg[3]) != 0)
	{
		return FALSE;
	}

	// fifth registry (number of sessions)
	// number of chars to skip starts at s[162]
	// clock sync begins at s[300]
	// 86400 seconds in a day
	k = 162;
	j = 300;

	// clock synchronization
	//for (i = 0; i < 5; i++)
	//{
	//	j += (int)reg[4][k++] - 48 + 1;
	//	u[i] = reg[4][j];
	//	reg[4][j] = 'x';
	//}
	//u[5] = '\0';
	//sscanf(u, "%d", &clockSync);

	// total number of sessions
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[4][k++] - 48 + 1;
		u[i] = reg[4][j];
		reg[4][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%d", &m_sessions);

	// a0
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[4][k++] - 48 + 1;
		u[i] = reg[4][j];
		reg[4][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &a0[4]);
	a0[4] *= 0.000001;

	// r
	for (i = 0; i < 6; i++)
	{
		j += (int)reg[4][k++] - 48 + 1;
		u[i] = reg[4][j];
		reg[4][j] = 'x';
	}
	u[6] = '\0';
	sscanf(u, "%lf", &r[4]);
	r[4] = 3.0 + (r[4] * 0.000001);

	// validate registry
	for (i = 0; i < 85; i++)
	{
		a = a0[4]; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r[4] * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		if (reg[4][i] == 'x')
		{
			s[i] = 'x';
		}
	}

	if (strcmp(s, reg[4]) != 0)
	{
		return FALSE;
	}

	// write m_time[] variables
	m_time[0] = CTime(year[0], month[0], day[0], hour[0], min[0], sec[0], -1);
	ts = CTimeSpan((LONG)expDays, 0, 0, 0);
	m_time[1] = m_time[0] + ts;
	m_time[2] = CTime(year[2], month[2], day[2], hour[2], min[2], sec[2], -1);
	m_time[3] = CTime(year[3], month[3], day[3], hour[3], min[3], sec[3], -1);
	

	return TRUE;
}

BOOL CYoLicense::yoDecodeLicenses(char *licenseCode, char license[3][17])
// NOT USED
// takes a 1530 char license code (from license.dll) and extracts three licenses
// from within its chaotic structure
{
	char s[512];
	int i;
	int j;
	int k;
	int n;
	char u[16];

	// first 510 chars (0 - 509) for first license

	// number of chars to skip starts at s[99]
	// license begins at s[210]
	j = 210;
	k = 99;

	for (i = 0; i < 510; i++)
	{
		s[i] = licenseCode[i];
	}

	// 16 char license
	for (i = 0; i < 16; i++)
	{
		j += (int)s[k++] - 48 + 2;
		u[0] = s[j];
		u[1] = s[j + 1];
		u[2] = '\0';
		sscanf(u, "%d", &n);
		n += 55;
		license[0][i] = (char)n;
	}
	license[0][16] = '\0';


	// second 510 chars (510 - 1019) for second license

	// number of chars to skip starts at s[26]
	// license begins at s[148]
	j = 148;
	k = 26;

	for (i = 0; i < 510; i++)
	{
		s[i] = licenseCode[i + 510];
	}

	// 16 char license
	for (i = 0; i < 16; i++)
	{
		j += (int)s[k++] - 48 + 2;
		u[0] = s[j];
		u[1] = s[j + 1];
		u[2] = '\0';
		sscanf(u, "%d", &n);
		n += 72;
		license[1][i] = (char)n;
	}
	license[1][16] = '\0';


	// second 510 chars (510 - 1019) for second license

	// number of chars to skip starts at s[49]
	// license begins at s[95]
	j = 95;
	k = 49;

	for (i = 0; i < 510; i++)
	{
		s[i] = licenseCode[i + 1020];
	}

	// 16 char license
	for (i = 0; i < 16; i++)
	{
		j += (int)s[k++] - 48 + 2;
		u[0] = s[j];
		u[1] = s[j + 1];
		u[2] = '\0';
		sscanf(u, "%d", &n);
		n += 63;
		license[2][i] = (char)n;
	}
	license[2][16] = '\0';

	return TRUE;
}



BOOL CYoLicense::yoEncodeLicenses(char *licenseCode)
// NOT USED
// encodes one to three licenses into a string that is the same as unique license code
// that will match code stored in license.dll file for licensing to be valid.
// m_license must first be set in YoEncode()
{
	char s[512];
	char t[512];
	double a;
	double a0;
	double b;
	double c;
	double r;
	int i;
	int j;
	int k;
	char u[16];
	char license[18];

	// first 510 chars (0 - 509) for first license
	r = 3.800000
		+ (0.01 * (double)((int)license[5] % 10))
		+ (0.001 * (double)((int)license[9] % 10))
		+ (0.0001 * (double)((int)license[2] % 10))
		+ (0.00001 * (double)((int)license[13] % 10))
		+ (0.000001 * (double)((int)license[6] % 10));
	a0 = 0.200000
		+ (0.01 * (double)((int)license[4] % 10))
		+ (0.001 * (double)((int)license[10] % 10))
		+ (0.0001 * (double)((int)license[0] % 10))
		+ (0.00001 * (double)((int)license[14] % 10))
		+ (0.000001 * (double)((int)license[7] % 10));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	//s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	//t[510] = '\0';

	// number of chars to skip starts at s[99]
	// license begins at s[210]

	j = 210;
	k = 99;

	// 16 char license
	for (i = 0; i < 16; i++)
	{
		j += (int)s[k++] - 48 + 2;
		sprintf(&t[j], "%02d", (int)license[i] - 55);
	}

	// a0
	sprintf(u, "%lf", a0);
	for (i = 2; i < 8; i++)
	{
		j += (int)s[k++] - 48 + 1;
		t[j] = u[i];
	}

	// r
	sprintf(u, "%lf", r);
	for (i = 2; i < 8; i++)
	{
		j += (int)s[k++] - 48 + 1;
		t[j] = u[i];
	}

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
		licenseCode[i] = s[i];
	}
	//s[510] = '\0';


	// second 510 chars (510 - 1019) for second license
	r = 3.900000
		+ (0.01 * (double)((int)license[15] % 10))
		+ (0.001 * (double)((int)license[8] % 10))
		+ (0.0001 * (double)((int)license[1] % 10))
		+ (0.00001 * (double)((int)license[11] % 10))
		+ (0.000001 * (double)((int)license[6] % 10));
	a0 = 0.300000
		+ (0.01 * (double)((int)license[3] % 10))
		+ (0.001 * (double)((int)license[12] % 10))
		+ (0.0001 * (double)((int)license[5] % 10))
		+ (0.00001 * (double)((int)license[0] % 10))
		+ (0.000001 * (double)((int)license[13] % 10));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	//s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	//t[510] = '\0';

	// number of chars to skip starts at s[26]
	// license begins at s[148]
	j = 148;
	k = 26;

	// 16 char license
	for (i = 0; i < 16; i++)
	{
		j += (int)s[k++] - 48 + 2;
		sprintf(&t[j], "%02d", (int)license[i] - 72);
	}

	// a0
	sprintf(u, "%lf", a0);
	for (i = 2; i < 8; i++)
	{
		j += (int)s[k++] - 48 + 1;
		t[j] = u[i];
	}

	// r
	sprintf(u, "%lf", r);
	for (i = 2; i < 8; i++)
	{
		j += (int)s[k++] - 48 + 1;
		t[j] = u[i];
	}

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
		licenseCode[i + 510] = s[i];
	}
	//s[510] = '\0';
	

	// third 510 chars (1020 - 1529) for third license
	r = 3.900000
		+ (0.01 * (double)((int)license[8] % 10))
		+ (0.001 * (double)((int)license[14] % 10))
		+ (0.0001 * (double)((int)license[11] % 10))
		+ (0.00001 * (double)((int)license[2] % 10))
		+ (0.000001 * (double)((int)license[3] % 10));
	a0 = 0.100000
		+ (0.01 * (double)((int)license[7] % 10))
		+ (0.001 * (double)((int)license[5] % 10))
		+ (0.0001 * (double)((int)license[12] % 10))
		+ (0.00001 * (double)((int)license[13] % 10))
		+ (0.000001 * (double)((int)license[0] % 10));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	//s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	//t[510] = '\0';

	// number of chars to skip starts at s[49]
	// license begins at s[95]
	j = 95;
	k = 49;

	// 16 char license
	for (i = 0; i < 16; i++)
	{
		j += (int)s[k++] - 48 + 2;
		sprintf(&t[j], "%02d", (int)license[i] - 63);
	}

	// a0
	sprintf(u, "%lf", a0);
	for (i = 2; i < 8; i++)
	{
		j += (int)s[k++] - 48 + 1;
		t[j] = u[i];
	}

	// r
	sprintf(u, "%lf", r);
	for (i = 2; i < 8; i++)
	{
		j += (int)s[k++] - 48 + 1;
		t[j] = u[i];
	}

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
		licenseCode[i + 1020] = s[i];
	}
	licenseCode[1530] = '\0';
	//s[510] = '\0';

	return TRUE;
}


BOOL CYoLicense::yoWriteRegistry(void)
// write updates to registry entries for temporary licensing
// yoReadRegistry() should be called at some point prior to calling this routine

{
	HKEY hk;
	DWORD type = REG_SZ;
	DWORD size = 512;
	char s[512];
	char t[512];
	double a;
	double a0;
	double b;
	double c;
	double r;
	int i;
	int j;
	char u[16];
	int day;
	int min;
	int sec;
	int ms;
	SYSTEMTIME gmSystemTime;
	CTime timeCurrent;
	CTimeSpan ts;

	sprintf(s, "Software\\bjones\\%s", m_applicationName);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, s,
		0, KEY_READ | KEY_WRITE, &hk)
		!= ERROR_SUCCESS)
	{
		return FALSE;
	}

	// get current date
	::GetSystemTime(&gmSystemTime);
	timeCurrent = gmSystemTime;

	if (timeCurrent < m_time[2] + CTimeSpan(3600))
	// clock was set backwards or program was used less than 1 hour
	// no more than 24 runs of program in 24 hour period
	{
		m_time[3] = m_time[2] + CTimeSpan(3600);
	}
	else
	{
		m_time[3] = timeCurrent;
	}

	ms = (int)gmSystemTime.wMilliseconds;
	sec = timeCurrent.GetSecond();
	min = timeCurrent.GetMinute();

	// first registry (start date)
	r = 3.800013 + (0.0001 * (double)(ms % 1000));
	a0 = 0.200694 + (0.001 * (double)(sec % 100));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[18]
	// start date begins at s[120]

	// year
	j = 120;
	j += (int)s[18] - 48 + 1;
	sprintf(u, "%04d", 9783 - m_time[0].GetYear());
	t[j] = u[0];
	j += (int)s[19] - 48 + 1;
	t[j] = u[1];
	j += (int)s[20] - 48 + 1;
	t[j] = u[2];
	j += (int)s[21] - 48 + 1;
	t[j] = u[3];

	// month
	sprintf(u, "%02d", m_time[0].GetMonth());
	j += (int)s[22] - 48 + 1;
	t[j] = u[0];
	j += (int)s[23] - 48 + 1;
	t[j] = u[1];

	// day
	sprintf(u, "%02d", 89 - m_time[0].GetDay());
	j += (int)s[24] - 48 + 1;
	t[j] = u[0];
	j += (int)s[25] - 48 + 1;
	t[j] = u[1];

	// hour
	sprintf(u, "%02d", 55 - m_time[0].GetHour());
	j += (int)s[26] - 48 + 1;
	t[j] = u[0];
	j += (int)s[27] - 48 + 1;
	t[j] = u[1];

	// min
	sprintf(u, "%02d", 69 - m_time[0].GetMinute());
	j += (int)s[28] - 48 + 1;
	t[j] = u[0];
	j += (int)s[29] - 48 + 1;
	t[j] = u[1];

	// sec
	sprintf(u, "%02d", 91 - m_time[0].GetSecond());
	j += (int)s[30] - 48 + 1;
	t[j] = u[0];
	j += (int)s[31] - 48 + 1;
	t[j] = u[1];

	// ms
	sprintf(u, "%03d", ms);
	j += (int)s[32] - 48 + 1;
	t[j] = u[0];
	j += (int)s[33] - 48 + 1;
	t[j] = u[1];
	j += (int)s[34] - 48 + 1;
	t[j] = u[2];

	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[35] - 48 + 1;
	t[j] = u[2];
	j += (int)s[36] - 48 + 1;
	t[j] = u[3];
	j += (int)s[37] - 48 + 1;
	t[j] = u[4];
	j += (int)s[38] - 48 + 1;
	t[j] = u[5];
	j += (int)s[39] - 48 + 1;
	t[j] = u[6];
	j += (int)s[40] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[41] - 48 + 1;
	t[j] = u[2];
	j += (int)s[42] - 48 + 1;
	t[j] = u[3];
	j += (int)s[43] - 48 + 1;
	t[j] = u[4];
	j += (int)s[44] - 48 + 1;
	t[j] = u[5];
	j += (int)s[45] - 48 + 1;
	t[j] = u[6];
	j += (int)s[46] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "first", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	// second registry (days to expiration)
	r = 3.900029 + (0.0001 * (double)(498 + (ms % 500)));
	a0 = 0.300692 + (0.001 * (double)(30 + (min % 60)));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[24]
	// days to expiration begins at s[282]

	ts = m_time[1] - m_time[0];
	day = (int)ts.GetDays();

	sprintf(u, "%03d", day);

	j = 282;
	j += (int)s[24] - 48 + 1;
	t[j] = u[0];
	j += (int)s[25] - 48 + 1;
	t[j] = u[1];
	j += (int)s[26] - 48 + 1;
	t[j] = u[2];

	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[27] - 48 + 1;
	t[j] = u[2];
	j += (int)s[28] - 48 + 1;
	t[j] = u[3];
	j += (int)s[29] - 48 + 1;
	t[j] = u[4];
	j += (int)s[30] - 48 + 1;
	t[j] = u[5];
	j += (int)s[31] - 48 + 1;
	t[j] = u[6];
	j += (int)s[32] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[33] - 48 + 1;
	t[j] = u[2];
	j += (int)s[34] - 48 + 1;
	t[j] = u[3];
	j += (int)s[35] - 48 + 1;
	t[j] = u[4];
	j += (int)s[36] - 48 + 1;
	t[j] = u[5];
	j += (int)s[37] - 48 + 1;
	t[j] = u[6];
	j += (int)s[38] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "second", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	// third registry (session start date)
	r = 3.870029 + (0.0001 * (double)(10 + (sec % 60)));
	a0 = 0.100092 + (0.0001 * (double)(202 + (ms % 500)));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[66]
	// session start date begins at s[186]

	// year
	j = 186;
	j += (int)s[66] - 48 + 1;
	sprintf(u, "%04d", 9783 - m_time[2].GetYear());
	t[j] = u[0];
	j += (int)s[67] - 48 + 1;
	t[j] = u[1];
	j += (int)s[68] - 48 + 1;
	t[j] = u[2];
	j += (int)s[69] - 48 + 1;
	t[j] = u[3];

	// month
	sprintf(u, "%02d", m_time[2].GetMonth());
	j += (int)s[70] - 48 + 1;
	t[j] = u[0];
	j += (int)s[71] - 48 + 1;
	t[j] = u[1];

	// day
	sprintf(u, "%02d", 89 - m_time[2].GetDay());
	j += (int)s[72] - 48 + 1;
	t[j] = u[0];
	j += (int)s[73] - 48 + 1;
	t[j] = u[1];

	// hour
	sprintf(u, "%02d", 55 - m_time[2].GetHour());
	j += (int)s[74] - 48 + 1;
	t[j] = u[0];
	j += (int)s[75] - 48 + 1;
	t[j] = u[1];

	// min
	sprintf(u, "%02d", 69 - m_time[2].GetMinute());
	j += (int)s[76] - 48 + 1;
	t[j] = u[0];
	j += (int)s[77] - 48 + 1;
	t[j] = u[1];

	// sec
	sprintf(u, "%02d", 91 - m_time[2].GetSecond());
	j += (int)s[78] - 48 + 1;
	t[j] = u[0];
	j += (int)s[79] - 48 + 1;
	t[j] = u[1];

	// ms
	sprintf(u, "%03d", (ms + 328) % 1000);
	j += (int)s[80] - 48 + 1;
	t[j] = u[0];
	j += (int)s[81] - 48 + 1;
	t[j] = u[1];
	j += (int)s[82] - 48 + 1;
	t[j] = u[2];

	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[83] - 48 + 1;
	t[j] = u[2];
	j += (int)s[84] - 48 + 1;
	t[j] = u[3];
	j += (int)s[85] - 48 + 1;
	t[j] = u[4];
	j += (int)s[86] - 48 + 1;
	t[j] = u[5];
	j += (int)s[87] - 48 + 1;
	t[j] = u[6];
	j += (int)s[88] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[89] - 48 + 1;
	t[j] = u[2];
	j += (int)s[90] - 48 + 1;
	t[j] = u[3];
	j += (int)s[91] - 48 + 1;
	t[j] = u[4];
	j += (int)s[92] - 48 + 1;
	t[j] = u[5];
	j += (int)s[93] - 48 + 1;
	t[j] = u[6];
	j += (int)s[94] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "third", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	// fourth registry (session end date)
	r = 3.900061 + (0.0001 * (double)(147 + (ms % 500)));
	a0 = 0.700392 + (0.001 * (double)(32 + (sec % 60)));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[126]
	// session end date begins at s[234]

	// year
	j = 234;
	j += (int)s[126] - 48 + 1;
	sprintf(u, "%04d", 9783 - m_time[3].GetYear());
	t[j] = u[0];
	j += (int)s[127] - 48 + 1;
	t[j] = u[1];
	j += (int)s[128] - 48 + 1;
	t[j] = u[2];
	j += (int)s[129] - 48 + 1;
	t[j] = u[3];

	// month
	sprintf(u, "%02d", m_time[3].GetMonth());
	j += (int)s[130] - 48 + 1;
	t[j] = u[0];
	j += (int)s[131] - 48 + 1;
	t[j] = u[1];

	// day
	sprintf(u, "%02d", 89 - m_time[3].GetDay());
	j += (int)s[132] - 48 + 1;
	t[j] = u[0];
	j += (int)s[133] - 48 + 1;
	t[j] = u[1];

	// hour
	sprintf(u, "%02d", 55 - m_time[3].GetHour());
	j += (int)s[134] - 48 + 1;
	t[j] = u[0];
	j += (int)s[135] - 48 + 1;
	t[j] = u[1];

	// min
	sprintf(u, "%02d", 69 - m_time[3].GetMinute());
	j += (int)s[136] - 48 + 1;
	t[j] = u[0];
	j += (int)s[137] - 48 + 1;
	t[j] = u[1];

	// sec
	sprintf(u, "%02d", 91 - m_time[3].GetSecond());
	j += (int)s[138] - 48 + 1;
	t[j] = u[0];
	j += (int)s[139] - 48 + 1;
	t[j] = u[1];

	// ms
	sprintf(u, "%03d", (ms + 466) % 1000);
	j += (int)s[140] - 48 + 1;
	t[j] = u[0];
	j += (int)s[141] - 48 + 1;
	t[j] = u[1];
	j += (int)s[142] - 48 + 1;
	t[j] = u[2];

	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[143] - 48 + 1;
	t[j] = u[2];
	j += (int)s[144] - 48 + 1;
	t[j] = u[3];
	j += (int)s[145] - 48 + 1;
	t[j] = u[4];
	j += (int)s[146] - 48 + 1;
	t[j] = u[5];
	j += (int)s[147] - 48 + 1;
	t[j] = u[6];
	j += (int)s[148] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[149] - 48 + 1;
	t[j] = u[2];
	j += (int)s[150] - 48 + 1;
	t[j] = u[3];
	j += (int)s[151] - 48 + 1;
	t[j] = u[4];
	j += (int)s[152] - 48 + 1;
	t[j] = u[5];
	j += (int)s[153] - 48 + 1;
	t[j] = u[6];
	j += (int)s[154] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "fourth", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	// fifth registry (clock synchronization, number of executions)
	r = 3.800052 + (0.0001 * (double)(23 + (ms % 500)));
	a0 = 0.700392 + (0.001 * (double)(39 + (min % 60)));

	for (i = 0; i < 85; i++)
	{
		a = a0; // always reset a to prevent rounding error
		for (j = 0; j < 21 - ((i * 4) % 15); j++)
		{
			b = r * a;
			c = 1.0 - a;
			a = b * c;
		}
		sprintf(u, "%lf", a);
		s[i * 6] = u[2];
		s[i * 6 + 1] = u[3];
		s[i * 6 + 2] = u[4];
		s[i * 6 + 3] = u[5];
		s[i * 6 + 4] = u[6];
		s[i * 6 + 5] = u[7];
	}
	s[510] = '\0';
	
	for (i = 0; i < 510; i++)
	{
		t[i] = 'x';
	}
	t[510] = '\0';

	// number of chars to skip starts at s[162]
	// clock sync begins at s[300]
	// 86400 seconds in a day

	//sprintf(u, "%05d", m_clockSync);

	j = 300;
	//j += (int)s[162] - 48 + 1;
	//t[j] = u[0];
	//j += (int)s[163] - 48 + 1;
	//t[j] = u[1];
	//j += (int)s[164] - 48 + 1;
	//t[j] = u[2];
	//j += (int)s[165] - 48 + 1;
	//t[j] = u[3];
	//j += (int)s[166] - 48 + 1;
	//t[j] = u[4];

	// number of sessions (up to 999999)
	sprintf(u, "%06d", m_sessions);
	j += (int)s[162] - 48 + 1;
	t[j] = u[0];
	j += (int)s[163] - 48 + 1;
	t[j] = u[1];
	j += (int)s[164] - 48 + 1;
	t[j] = u[2];
	j += (int)s[165] - 48 + 1;
	t[j] = u[3];
	j += (int)s[166] - 48 + 1;
	t[j] = u[4];
	j += (int)s[167] - 48 + 1;
	t[j] = u[5];


	// a0
	sprintf(u, "%lf", a0);
	j += (int)s[168] - 48 + 1;
	t[j] = u[2];
	j += (int)s[169] - 48 + 1;
	t[j] = u[3];
	j += (int)s[170] - 48 + 1;
	t[j] = u[4];
	j += (int)s[171] - 48 + 1;
	t[j] = u[5];
	j += (int)s[172] - 48 + 1;
	t[j] = u[6];
	j += (int)s[173] - 48 + 1;
	t[j] = u[7];

	// r
	sprintf(u, "%lf", r);
	j += (int)s[174] - 48 + 1;
	t[j] = u[2];
	j += (int)s[175] - 48 + 1;
	t[j] = u[3];
	j += (int)s[176] - 48 + 1;
	t[j] = u[4];
	j += (int)s[177] - 48 + 1;
	t[j] = u[5];
	j += (int)s[178] - 48 + 1;
	t[j] = u[6];
	j += (int)s[179] - 48 + 1;
	t[j] = u[7];

	for (i = 0; i < 510; i++)
	{
		if (t[i] >= '0' && t[i] <= '9')
		{
			s[i] = t[i];
		}
	}
	s[510] = '\0';

	if (RegSetValueEx(hk, "fifth", 0, type, (BYTE *)s, size) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}


/***
BOOL CAddressApp::rpDecode(char code[17])
// not used
{
	double a[17];
	double x1;
	double x2;
	double r = 3.90102;
	int i;
	int j;

	for (i = 0; i < 17; i++)
	{
		a[i] = (double)((int)code[i] - 97) / 26.0;

		for (j = 0; j < 76 - i; j++)
		{
			x1 = (-r + sqrt(r * r - 4.0 * -r * -a[i])) / (2.0 * -r);
			x2 = (-r - sqrt(r * r - 4.0 * -r * -a[i])) / (2.0 * -r);

			if (r * x1 * (1.0 - x1) == a[i])
			{
				a[i] = x1;
			}
			else if (r * x2 * (1.0 - x2) == a[i])
			{
				a[i] = x2;
			}
			else
			{
				TRACE1("ERROR x1 x2 %d\n", j);
			}
		}
		code[i] = (char)((int)((-0.03125 + a[i]) * 16.0) + 48);
		if (i % 3 == 2)
		{
			code[i] = '-';
		}

	}

	return TRUE;
}
***/


/***
BOOL CAddressApp::rpMessage(char sMessage[256])
// insert in main application
// displays message box containing licensing error
// option exists to enter a temporary key based on initialization code
// if temporary key is valid TRUE is returned else FALSE is returned indicating
// licensing failure
{
	CAddressMessageDlg messageDlg;
	char s[24];
	char *t;
	int i;
	int j;

	// generate initialization code
	rpInitializationCode();

	//rpTemporaryKey();

	j = 0;
	for (i = 0; i < 16; i++)
	{
		s[j] = m_initializationCode[i];
		j++;
		if (i < 15 && (i % 4) == 3)
		{
			s[j] = ' ';
			j++;
		}
	}
	s[j] = '\0';

	messageDlg.m_staticMessage.Format("%s", (LPCTSTR)sMessage);
	messageDlg.m_initializationCode.Format("%s", (LPCTSTR)s);
	messageDlg.DoModal();

	// generate the temporary key
	rpTemporaryKey();

	sprintf(s, "%s", messageDlg.m_temporaryKey);
	// remove spaces
	i = 0;
	for (t = s; i < 24 && *t != '\0'; t++)
	{
		for (; *t == ' '; t++);
		s[i] = *t;
		i++;
	}
	s[16] = '\0';
	
	// find placeholder for expiration code
	// expiration codes
	// a = 360 days
	// b = 180 days
	// c = 90 days
	// d = 60 days
	// e = 30 days
	// f = 7 days
	// g and other chars = 1 day
	i = (((int)m_temporaryKey[0] - 97) % 14) + 1;
	m_temporaryKey[i] = s[i];

	if (strcmp(s, m_temporaryKey) == 0)
	{
		return rpInitializeRegistry(s[i]);
	}
	else
	{
		m_temporaryKey[0] = '\0';
		return FALSE;
	}
}
***/