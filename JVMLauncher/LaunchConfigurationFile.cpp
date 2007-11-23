/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

Read an ivy launch configuration file. The file must contain name value pairs (properties) that are 
located on the same line and separated with a '=' sign.
The following properties are supported:

ivy.heap.max.ratio				Max heap size (-Xmx) in percentage of the phyiscal memory.
ivy.heap.max.size				Max heap size (-Xmx) in mega bytes.
ivy.heap.start.size				Start heap size (-Xms) in mega bytes.
ivy.heap.free.max.ratio			Max free heap memory (-XX:MaxHeapFreeRatio) in percentage of the current heap size.
ivy.heap.free.min.ratio			Min free heap memory (-XX:MinHeapFreeRatio) in percentage of the current heap size.
ivy.heap.young.max.size			Max young heap size (-XX:MaxNewSize) in mega bytes.
ivy.heap.young.min.size			Min young heap size (-XX:NewSize) in mega bytes.
ivy.heap.eden.survivor.ratio	Survivor heap size as ratio between the eden and the survivor heap size (-XX:SurvivorRatio).
ivy.heap.tenured.young.ratio	Young heap size as ratio between the tenured and the young heap size (-XX:NewRatio).
ivy.heap.perm.max.size			Max permanent heap size (-XX:MaxPermSize) in mega bytes.
ivy.jvm.type					The java virtual machine type to use (ClientHotspotJVM, ServerHotspotJVM).
ivy.dir.aux						The directory where the ivyTeam jars are located.
ivy.dir.jre						The directory where the java runtime environment is located.
ivy.java.main.class				The java main class to launch
ivy.java.main.method			The java main method to launch
ivy.vm.additional.options       Additional vm arguments
ivy.garbage.collector.options   Garbage collector arguments
ivy.windows.service.name		The name of the windows service
ivy.application.name			The name of the application
ivy.application.singleton		Is the application a singleton (true, false)

The following properties are still supported but deprecated:

ivy.StartMem			Start heap size (-Xms) in mega bytes. Deprecated.
ivy.MaxMem				Max heap size (-Xmx) in permille of the physical memory. Deprecated.
ivy.JvmType				Same as ivy.jvm.type. Deprecated.
ivy.AuxDirectory		Same as ivy.dir.aux. Deprecated.
ivy.MainClassName		Same as ivy.java.main.class. Deprecated.


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

#include "LaunchConfigurationFile.h"
#include "LaunchException.h"
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_LINE_LENGTH 10000

CLaunchConfigurationFile::CLaunchConfigurationFile(LPCSTR pcLauncherConfigurationFileName)
{
	assert(pcLauncherConfigurationFileName != NULL);
	m_pcLauncherConfigurationFileName = new char[strlen(pcLauncherConfigurationFileName)+1];
	strcpy_s(m_pcLauncherConfigurationFileName, strlen(pcLauncherConfigurationFileName)+1, pcLauncherConfigurationFileName);
}

CLaunchConfigurationFile::CLaunchConfigurationFile()
{
	m_pcLauncherConfigurationFileName = new char[1];
	strcpy_s(m_pcLauncherConfigurationFileName, 1, "");
}


CLaunchConfigurationFile::CLaunchConfigurationFile(const CLaunchConfigurationFile& copy)
{
	m_pcLauncherConfigurationFileName = new char[strlen(copy.m_pcLauncherConfigurationFileName)+1];
	strcpy_s(m_pcLauncherConfigurationFileName, strlen(copy.m_pcLauncherConfigurationFileName)+1, copy.m_pcLauncherConfigurationFileName);
}

CLaunchConfigurationFile::~CLaunchConfigurationFile()
{
	delete m_pcLauncherConfigurationFileName;
}

CLaunchConfiguration CLaunchConfigurationFile::readLaunchConfiguration()
{
	FILE *file = NULL;  
	char pcLine[MAX_LINE_LENGTH];
	char* pcValue;
	char* pcName;
	int pos;
	char ch;
	int errno;
	CLaunchConfiguration launchConfiguration;

	errno = 0;
	errno = fopen_s(&file, m_pcLauncherConfigurationFileName, "r");
	if ((errno != 0)||(file == NULL))
	{
		// file does not exists or could not be read
		throw CLaunchException(
			JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, 
			"Could not open launch configuration file %s", m_pcLauncherConfigurationFileName);
	}

	CLog::info("Read launch configuration file '%s'", m_pcLauncherConfigurationFileName);
	
	do 
	{
		pos = 0;
		do
		{
			ch = fgetc(file);
			if ((ch!='\r')&&(ch!='\n')&&(ch!=EOF))
			{
				pcLine[pos] = ch;
				pos++;
			}
		}
		while ((ch!=EOF)&&(ch!='\n')&&(pos<MAX_LINE_LENGTH));
		pcLine[pos] = '\0';

		if (pos==MAX_LINE_LENGTH)
		{
			throw CLaunchException(JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, "Launch configuration line: %s is to long", pcLine);
		}
		else if (!isCommentLine(pcLine))
		{
			CLog::debug("Line: %s", pcLine);
			pcName = pcLine;
			// search '='
			pcValue = strchr(pcLine, '=');
			// is a value defined?
			if (pcValue!=NULL)
			{
				*pcValue = '\0';
				pcValue++;
				// TODO trim pcName and pcValue!
				analyseNameValuePair(&launchConfiguration, trim(pcName), trim(pcValue));
			}
		}
	} while (ch!=EOF);

	// close file
	fclose(file);
	return launchConfiguration;
}

void CLaunchConfigurationFile::analyseNameValuePair(CLaunchConfiguration* pLaunchConfiguration, LPCSTR pcName, LPCSTR pcValue)
{
	CLog::debug("Launch configuration property %s=%s", pcName, pcValue);
	if (strcmp(pcName, "ivy.MaxMem")==0)	
	{
		pLaunchConfiguration->setHeapMaxRatio(toPermille(pcName, pcValue)/10);
	}
	else if (strcmp(pcName, "ivy.heap.max.ratio")==0)
	{
		pLaunchConfiguration->setHeapMaxRatio(toPercentage(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.StartMem")==0)
	{
		pLaunchConfiguration->setHeapStartSize(toNumber(pcName, pcValue)*1024);
	}
	else if (strcmp(pcName, "ivy.heap.start.size")==0)
	{
		pLaunchConfiguration->setHeapStartSize(toNumber(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.heap.max.size")==0)
	{
		pLaunchConfiguration->setHeapMaxSize(toNumber(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.heap.free.max.ratio")==0)
	{
		pLaunchConfiguration->setHeapMaxFreeRatio(toPercentage(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.heap.free.min.ratio")==0)
	{
		pLaunchConfiguration->setHeapMinFreeRatio(toPercentage(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.heap.young.max.size")==0)
	{
		pLaunchConfiguration->setHeapMaxYoungSize(toNumber(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.heap.young.min.size")==0)
	{
		pLaunchConfiguration->setHeapMinYoungSize(toNumber(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.heap.eden.survivor.ratio")==0)
	{
		pLaunchConfiguration->setHeapEdenToSurvivorRatio(toNumber(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.heap.tenured.young.ratio")==0)
	{
		pLaunchConfiguration->setHeapTenuredToYoungRatio(toNumber(pcName, pcValue));
	}
	else if (strcmp(pcName, "ivy.heap.perm.max.size")==0)
	{
		pLaunchConfiguration->setHeapMaxPermSize(toNumber(pcName, pcValue));
	}
	else if ((strcmp(pcName, "ivy.JvmType")==0)||
		     (strcmp(pcName, "ivy.jvm.type")==0))
	{
		if (strcmp(pcValue, "ClassicJVM")==0)
		{
			pLaunchConfiguration->setJvmType(ClassicJVM);
		}
		else if(strcmp(pcValue, "ServerHotspotJVM")==0)
		{
			pLaunchConfiguration->setJvmType(ServerHotspotJVM);
		}
		else if(strcmp(pcValue, "ClientHotspotJVM")==0)
		{
			pLaunchConfiguration->setJvmType(ClientHotspotJVM);
		}
		else
		{
			throw CLaunchException(JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, "The value %s of the launch configuration property is not a valid jvm type", pcValue, pcName);
		}
	}
	else if ((strcmp(pcName, "ivy.AuxDirectory")==0)||
		     (strcmp(pcName, "ivy.dir.aux")==0))
	{
		pLaunchConfiguration->setAuxDirectory(pcValue);
	}
	else if ((strcmp(pcName, "ivy.JreDirectory")==0)||
		     (strcmp(pcName, "ivy.dir.jre")==0))
	{
		pLaunchConfiguration->setJreDirectory(pcValue);
	}
	else if ((strcmp(pcName, "ivy.MainClassName")==0)||
		    (strcmp(pcName, "ivy.java.main.class")==0))
	{
		pLaunchConfiguration->setMainJavaClass(pcValue);
	}
	else if (strcmp(pcName, "ivy.java.main.method")==0)
	{
		pLaunchConfiguration->setMainJavaMethod(pcValue);
	}
	else if (strcmp(pcName, "ivy.vm.additional.options")==0)
	{
		pLaunchConfiguration->setAdditionalVmOptions(pcValue);
	}
	else if (strcmp(pcName, "ivy.garbage.collector.options")==0)
	{
		pLaunchConfiguration->setGarbageCollectorOptions(pcValue);
	}
	else if (strcmp(pcName, "ivy.windows.service.name")==0)
	{
		pLaunchConfiguration->setWindowsServiceName(pcValue);
	}
	else if (strcmp(pcName, "ivy.application.name")==0)
	{
		pLaunchConfiguration->setApplicationName(pcValue);
	}
	else if (strcmp(pcName, "ivy.application.singleton")==0)
	{
		pLaunchConfiguration->setSingleton(toBoolean(pcName, pcValue));
	}
	else
	{
		throw CLaunchException(2, "The launch configuration property %s is unknown", pcName);
	}
}

DWORD CLaunchConfigurationFile::toPermille(LPCSTR pcName, LPCSTR pcValue)
{
	DWORD value;
	value = toNumber(pcName, pcValue);
	if ((value < 0)||(value>1000))
	{
		throw CLaunchException(JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, "The value of the launch configuration property %s must be in permille (0..1000) but was %s", pcName, pcValue);
	}
	return value;
}

DWORD CLaunchConfigurationFile::toPercentage(LPCSTR pcName, LPCSTR pcValue)
{
	DWORD value;
	value = toNumber(pcName, pcValue);
	if ((value < 0)||(value>100))
	{
		throw CLaunchException(JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, "The value of the launch configuration property %s must be in percentage (0..100) but was %s", pcName, pcValue);
	}
	return value;
}

DWORD CLaunchConfigurationFile::toNumber(LPCSTR pcName, LPCSTR pcValue)
{
	DWORD value;

	errno = 0;
	value = (DWORD)atol(pcValue);
	if (errno != 0)
	{
		throw CLaunchException(JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, "The value of the launch configuration property %s must be a number but was %s", pcName, pcValue);
	}
	return value;
}

bool CLaunchConfigurationFile::toBoolean(LPCSTR pcName, LPCSTR pcValue)
{
	if (_stricmp(pcValue, "true")==0)
	{
		return true;
	}
	else if (_stricmp(pcValue, "false")==0)
	{
		return false;
	}
	else
	{
		throw CLaunchException(JVMLauncherErrorCodes::WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT, "The value of the launch configuration property %s must be a boolean (true or false) but was %s", pcName, pcValue);
	}
}	


CLaunchConfigurationFile CLaunchConfigurationFile::getForExecutable()
{
	char pcExecutableName[MAX_PATH];
	DWORD result;

	/* Get the full module filename and cut it after the last directory */
    result = GetModuleFileName(NULL, pcExecutableName, MAX_PATH);
	if (result == 0)
	{
		throw CLaunchException(JVMLauncherErrorCodes.WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT,
			GetLastError(),
			"Could not get module file name");
	}

	*(strrchr(pcExecutableName, '.')) = '\0'; /* remove .exe */
	strcat_s(pcExecutableName, MAX_PATH, ".ilc");
	return CLaunchConfigurationFile(pcExecutableName);
}

bool CLaunchConfigurationFile::exists()
{
	FILE* file;
	int errno;
	errno = fopen_s(&file, m_pcLauncherConfigurationFileName, "r");
	if (errno == 0)
	{
		fclose(file);
		return true;
	}
	return false;
}

CLaunchConfigurationFile& CLaunchConfigurationFile::operator=(const CLaunchConfigurationFile& rightValue)
{
	if (this != &rightValue)
	{
		delete m_pcLauncherConfigurationFileName;
		m_pcLauncherConfigurationFileName = new char[strlen(rightValue.m_pcLauncherConfigurationFileName)+1];
		strcpy_s(m_pcLauncherConfigurationFileName, strlen(rightValue.m_pcLauncherConfigurationFileName)+1, rightValue.m_pcLauncherConfigurationFileName);
	}
	return *this;
}

LPSTR CLaunchConfigurationFile::trim(LPSTR pcString)
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

bool CLaunchConfigurationFile::isCommentLine(LPSTR pcLine)
{
	while (isspace(*pcLine))
	{
		pcLine++;
	}
	return (*pcLine=='#');
}