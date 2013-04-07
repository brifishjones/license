// YoLicense.h: interface for the CYoLicense class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YOLICENSE_H__33D4C647_DA97_46B0_B689_3756EEAB924D__INCLUDED_)
#define AFX_YOLICENSE_H__33D4C647_DA97_46B0_B689_3756EEAB924D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MyReusableStaticSingleThreadReleaseLibrary.h


class CYoLicense  
{
public:
	CYoLicense(char *);     // pass in the application name
	virtual ~CYoLicense();
	BOOL yoInitializeTemporaryLicense(char, char message[256]);  // initialize registry for temporary licensing
	BOOL yoValidateTemporaryLicense(char message[256]);  // call from main application
	BOOL yoValidateLicense(char progID[48], char message[256]);  // call from main application
	BOOL yoGetLicense(char license[64]);   // obtains license based on HDSN and MAC addresses for the computer running this application
	BOOL yoGetComputerInfo(char info[64]);   // obtains HDSN and MAC addresses for the computer running this application
	BOOL yoGenerateInitializationCode(void);  // generates code needed for the temporary licensing
	BOOL yoTemporaryKey(void);  // generate a temporary key based on the value of m_initializationCode
	BOOL CYoLicense::yoGetDateTime(char *dateTime);   // extracts the current date and time from m_initializationCode
	BOOL yoEncode(char *);   // generates codes based on HDSN and MAC addresses and compares to license passed in

	char m_code[3][18];    // HDSN [0] and MAC addresses [1 and 2]
	char m_license[64];     // holds permanent license if it is valid	
	char m_initializationCode[17];  // code generated for temporary licensing.
	char m_temporaryKey[17];  	// contains the value of a temporary key for a given initialization code
	                            // once temporary key is validated the value is the expiration date
	                            // the empty string signifies an invalid temporaryKey

private:
	// BOOL yoMessage(char sMessage[256]);  // sample message box -- put in main application
	BOOL yoLicenseHDSN(void);   // finds hard disk serial number
	BOOL yoLicenseMACFile(void);  // finds MAC address and writes it to a temporary file
	BOOL yoLicenseMACPipe(void);  // finds MAC address using pipe
	BOOL yoReadRegistry(void);  // reads registry values
	BOOL yoWriteRegistry(void);  // writes registry values
	BOOL yoValidCode(char *);   // checks code passed in to see if it is a valid HDSN or MAC address
	BOOL yoEncodeLicenses(char *);   // encodes generated licenses for unique license code in license.dll
	BOOL yoDecodeLicenses(char *, char license[3][17]);   // extracts licenses from license code
	void yoDateCode(void);   // generate a code based on the current date and time
	//BOOL yoDecode(char code[17]);
	char m_applicationName[32];
	char m_dateCode[128];

	CTime m_time[4];  // four time variables used for temporary licensing

	int m_sessions;
};

#endif // !defined(AFX_YOLICENSE_H__33D4C647_DA97_46B0_B689_3756EEAB924D__INCLUDED_)
