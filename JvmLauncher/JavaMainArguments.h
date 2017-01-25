#ifndef __JAVA_MAIN_ARGUMENTS_H__
#define __JAVA_MAIN_ARGUMENTS_H__

#include <windows.h>

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

class JVMLauncherExport JavaMainArguments
{
private:
	int m_argc;
	LPSTR* m_argv;
	int m_length;

public:
	JavaMainArguments(int argc, LPSTR argv[]);
	~JavaMainArguments();
	void addArgument(LPSTR arg);
	int getCount();
	LPSTR getArgument(int pos);
	LPSTR* getArguments();
};

#endif 
