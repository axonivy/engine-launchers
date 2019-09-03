#ifndef __JVM_OPTIONS_FILE_H__
#define __JVM_OPTIONS_FILE_H__

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

#include "VmOptions.h"

#define MAX_LINE_LENGTH 10000

class JVMLauncherExport CJvmOptionsFile  
{
public:
	CJvmOptionsFile(LPSTR pcJvmOptionsFile);
	~CJvmOptionsFile();
	void parse(CVmOptions& options);

private: 
	LPSTR m_pcJvmOptionsFile;
	
	bool isCommentLine(LPSTR pcLine);
	bool isEmpty(LPSTR pcLine);
    LPSTR trim(LPSTR pcLine);
};

#endif 