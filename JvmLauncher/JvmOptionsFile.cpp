#define JVMLauncher

#include "JvmOptionsFile.h"
#include "LaunchException.h"
#include "Log.h"
#include "VmOptions.h"

CJvmOptionsFile::CJvmOptionsFile(LPSTR pcJvmOptionsFile)
{	
	m_pcJvmOptionsFile = new char[strlen(pcJvmOptionsFile) + 1];	
	strcpy_s(m_pcJvmOptionsFile, strlen(pcJvmOptionsFile) + 1, pcJvmOptionsFile);
}

CJvmOptionsFile::~CJvmOptionsFile()
{
	delete m_pcJvmOptionsFile;
}

void CJvmOptionsFile::parse(CVmOptions& options)
{
	FILE *file = NULL;
	char pcLine[MAX_LINE_LENGTH];
	int pos;
	char ch;
	int errno;

	errno = 0;
	errno = fopen_s(&file, m_pcJvmOptionsFile, "r");
	if ((errno != 0) || (file == NULL))
	{
		throw CLaunchException(JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, "Could not open jvm.options file %s", m_pcJvmOptionsFile);
	}

	CLog::info("Read jvm.options file from '%s'", m_pcJvmOptionsFile);
	
	do 
	{
		pos = 0;
		do
		{
			ch = fgetc(file);
			if ((ch != '\r') && (ch != '\n') && (ch != EOF))
			{
				pcLine[pos] = ch;
				pos++;
			}
		}
		while ((ch != EOF) && (ch != '\n') && (pos < MAX_LINE_LENGTH));
		pcLine[pos] = '\0';

		if (pos == MAX_LINE_LENGTH)
		{
			throw CLaunchException(JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, "jvm.options line: %s is to long", pcLine);
		}
		
		LPSTR trimmedLine;
		trimmedLine = trim(pcLine);
		if (!isCommentLine(trimmedLine) && !isEmpty(trimmedLine))
		{
		   CLog::debug("Line: %s", trim(trimmedLine));
		   options.addOption(trimmedLine, NULL);
		}
	} while (ch!=EOF);

	fclose(file);
}


bool CJvmOptionsFile::isCommentLine(LPSTR pcLine)
{
	while (isspace(*pcLine))
	{
		pcLine++;
	}
	return (*pcLine == '#');
}

LPSTR CJvmOptionsFile::trim(LPSTR pcString)
{
	LPSTR pcResult;
	pcResult = pcString;
	while (isspace(*pcResult))
	{
		pcResult++;
	}

	if (strlen(pcResult)>0)
	{
		pcString = pcResult+strlen(pcResult)-1;
		while (isspace(*pcString))
		{
			pcString--;
		}
		pcString[1] = '\0';
	}
	return pcResult;
}

bool CJvmOptionsFile::isEmpty(LPSTR pcLine)
{
	return (pcLine == NULL || pcLine[0] == 0);
}
