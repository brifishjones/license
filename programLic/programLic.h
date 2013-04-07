/*
	this header is not required for this dll
	to compile; but is required for the app
	that will use this dll
*/

#define PROGRAM_ERROR_LICENSE_PIPE_FILE -100
#define PROGRAM_ERROR_NO_ETHERNET_CARD -101
#define PROGRAM_ERROR_INVALID_LICENSE -102

#ifndef CPPDLL_H
#define CPPDLL_H

//prevent the function name from being mangled
extern "C" 
void yoValidLicense(char *license);

#endif //CPPDLL_H