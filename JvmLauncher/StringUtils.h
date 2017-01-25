#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <windows.h>

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

class JVMLauncherExport StringUtils
{
public:
	static bool isEmpty(LPCSTR pcString);
};

#endif 
