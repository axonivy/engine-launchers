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

#ifndef __LOG_H__
#define __LOG_H__

#include <windows.h>

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

class JVMLauncherExport CLog
{
private:
	static void log(LPCSTR pcPrefix, LPCSTR pcFormat, va_list args);
public:
	static bool enableLoggingInfo;
	static void debug(LPCSTR pcFormat, ...);
	static void info(LPCSTR pcFormat, ...);
	static void error(LPCSTR pcFormat, ...);
	static void warn(LPCSTR pcFormat, ...);	
};

#endif 
