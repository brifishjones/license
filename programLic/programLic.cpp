// programLic.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "programLic.h"
//#include <stdio.h>

// progID is the usually the CLSID of the program without braces and dashes
// It must match exactly the CLSID of the program using this license DLL
char progID[48] = "F3CAEE0F4D244800A7469B7080362802";   // for address.exe

// program license must contain at least one 16 char license and can have up to 3 licenses
// (16 chars each, separated by a space) for hard disk serial number, and two MAC addresses.
// MUST MODIFY THE FOLLOWING LINE FOR EACH LICENSE ISSUED
#define PROGRAM_LICENSE "sgwfilhkzarxonqy hyolvtcrjniadezb btdarxchgslyefwj"

// Brian Jones			DD-32-7B-12-FF-1F	pboxihygfclzedaj
// Brian Jones (luther)	DD-DC-B9-9F-57-97 00-0B-DB-DF-5B-F0 00-04-23-4C-BF-BC sgwfilhkzarxonqy hyolvtcrjniadezb btdarxchgslyefwj
// Russ Leggett			DD-64-32-8E-25-85 00-04-5A-66-F0-99	cbmefvidarupkhng bquxdtwczjrfpkea 
// Joanna Sinnwell		00-00-86-46-D1-79	jyin bsgk tcmo awde
// Bill Fisher			00-50-BF-75-01-CF	ibcf jamh glqs kdne
// Jane Hayes			DD-2E-3A-0A-D5-05	psarmefcnodlkigb  
 
//prevent function name from being mangled
extern "C" 
void yoValidLicense(char *license)
// license will either contain the license or verification code
{
	//char code[128];
	int i;
	int j;
	int k;
	double a;
	double a0;
	double b;
	double c;
	double r;
	char s[16];
	static int vCount = 0;

	if (license[0] != 'A')
	// return license
	{
		sprintf(license, PROGRAM_LICENSE);
		return;
	}

	//sprintf(code, "%s", license);
	
	// perform license validation using the same encoding routine found in
	// program.exe [BOOL CProgramApp::rpLicense()]

	// return verification code
	// will be called 8 times by program.exe
	// part of the program ID (usually the CLSID) is used for r and a0
	if (vCount >= 8)
	// vCount should never reach 8
	{
		for (i = 0; i < 126; i++)
		{
			license[i] = '0';
		}
		license[126] = '\0';
		return;
	}

	// r and a0 is a blend of progID and the current date code imbedded in license
	k = (int)license[93] - 48;
	r = 3.9 + (double)((int)progID[vCount] % 10) * 0.01
		+ (double)((int)license[k * 7 + 5] - 48) * 0.001
		+ (double)((int)progID[16 + vCount] % 10) * 0.0001
		+ (double)((int)license[k * 7 + 13] - 48) * 0.00001
		+ (double)((int)progID[24 + vCount] % 10) * 0.000001;
	a0 = (double)((int)progID[8 + vCount] % 10) * 0.1
		+ (double)((int)license[k * 7 + 8] - 48) * 0.01
		+ (double)((int)progID[vCount] % 10) * 0.001
		+ (double)((int)license[k * 7 + 3] - 48) * 0.0001
		+ (double)((int)progID[16 + vCount] % 10) * 0.00001
		+ (double)((int)license[k * 7 + 15] - 48) * 0.000001;


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
		license[i * 6] = s[2];
		license[i * 6 + 1] = s[3];
		license[i * 6 + 2] = s[4];
		license[i * 6 + 3] = s[5];
		license[i * 6 + 4] = s[6];
		license[i * 6 + 5] = s[7];
		//sprintf(&license[i * 6], "%06d", k);
	}
	license[126] = '\0';

	vCount++;

	//k++;

	return;

}

