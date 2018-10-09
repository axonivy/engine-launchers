/*-----------------------------------------------------------------------------------

   Java Launcher

-------------------------------------------------------------------------------------

Java Launcher creates a java virtual machine and starts a java programm

--------------------------------------------------------------------------------------

Project		: JavaLauncher
Version		: 1.0
Compiler	: VC++ 5
Java		: JDK1.2
Author		: Reto Weiss
Copyright	: Ivyteam, 2000

--------------------------------------------------------------------------------------
History:
		  7.8.2000 ReW created
--------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "..\JVMLauncher\JVMLauncher.h"
#include "config.h"

/* 
 * The main function
 */
void _main(int argc, char* argv[])
{
	CJavaProgram* pProgram;
	
	pProgram = CJavaProgram::createJavaProgram(getLaunchConfiguration(), true);
	try
	{
	 	argc--; // skip the application name
		argv++;
		pProgram->main(argc, argv);
	}
	catch(CLaunchException ex)
	{
		pProgram->reportError(ex);
		delete pProgram;
		throw ex;
	}
	delete pProgram;
}


/**
 * The main function for console app
 */
#ifdef _WINDOWS

/*
The main function for windows app
*/
int WINAPI
WinMain(HINSTANCE inst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
	try
	{
		_main(__argc, __argv);
		return 0;
	}
	catch(CLaunchException ex)
	{
		MessageBox(NULL, ex.getMessage(), "AXON Ivy Java Launcher",
            (MB_OK|MB_ICONSTOP|MB_APPLMODAL)); 
		return ex.getErrorCode();
	}    
}

#else

/*
 * The main function for console application
 */
int main(int argc, char *argv[])
{
	try
	{
		_main(argc, argv);
		return 0;
	}
	catch(CLaunchException ex)
	{
		CLog::error(ex.getMessage());
		return ex.getErrorCode();
	}	
}

#endif