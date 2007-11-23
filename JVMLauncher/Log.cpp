/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

- Log functions

--------------------------------------------------------------------------------------

Project		: JVMLauncher
Version		: 1.0
Compiler	: VC++ 6
Java		: JDK1.4
Author		: Reto Weiss
Copyright	: Ivyteam, 2006

--------------------------------------------------------------------------------------
History:
		  27.3.2006 ReW created
--------------------------------------------------------------------------------------*/
#define JVMLauncher

#include "Log.h"
#include <fstream>
#include <stdio.h>

using namespace std;

/*
 * Debug output function
 */
void CLog::log(LPCSTR pcPrefix, LPCSTR pcFormat, va_list args)
{
	char pcBuffer[20000]; 
	// dump to console
	printf(pcPrefix);
	vprintf(pcFormat, args);
	printf("\n");
	vsprintf_s(pcBuffer, 20000, pcFormat, args);
	// dump to debug console of visual studio
	OutputDebugString(pcPrefix);
	OutputDebugString(pcBuffer);
	OutputDebugString("\n");
#ifdef _DEBUG
	// dump to jvmlauncher.log file
	char pcLogFile[MAX_PATH];
    
	GetModuleFileName(NULL, pcLogFile, MAX_PATH);
	 *(strrchr(pcLogFile, '\\')) = '\0'; /* remove \xxx.exe */
	 strcat_s(pcLogFile, MAX_PATH, "\\jvmlauncher.log");

	fstream strm(pcLogFile, ios::app|ios::out);
	strm << pcPrefix << pcBuffer << endl;
	strm.close();
#endif
}

void CLog::debug(LPCSTR pcFormat, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, pcFormat);

	log("[DEBUG] ", pcFormat, args);
#endif
}

void CLog::info(LPCSTR pcFormat, ...)
{
	va_list args;
	va_start(args, pcFormat);

	log("", pcFormat, args);
}

void CLog::error(LPCSTR pcFormat, ...)
{
	va_list args;
	va_start(args, pcFormat);

	log("Error: ", pcFormat, args);
}

void CLog::warn(LPCSTR pcFormat, ...)
{
	va_list args;
	va_start(args, pcFormat);

	log("Warning: ", pcFormat, args);
}


