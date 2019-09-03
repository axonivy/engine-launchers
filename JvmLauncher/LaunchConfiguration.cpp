/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

- Holds the launch configuration

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
#include "LaunchConfiguration.h"
#include <assert.h>
#include "Log.h"

CLaunchConfiguration::CLaunchConfiguration()
: m_pcMainJavaClass(NULL),
  m_pcOsgiApplicationName(NULL),
  m_bConsole(false),
  m_bUseLowMemoryJvmSettings(false),
  m_pcServerStopArgument(NULL)
{
}

CLaunchConfiguration::~CLaunchConfiguration()
{
	if (m_pcMainJavaClass != NULL)
	{
		delete m_pcMainJavaClass;
	}
	if (m_pcOsgiApplicationName != NULL)
	{
		delete m_pcOsgiApplicationName;
	}
	if (m_pcServerStopArgument != NULL)
	{
		delete m_pcServerStopArgument;
	}
}

LPCSTR CLaunchConfiguration::getMainJavaClass()
{
	return m_pcMainJavaClass;
}

void CLaunchConfiguration::setMainJavaClass(LPCSTR pcMainJavaClass)
{
	assert(pcMainJavaClass != NULL);

	if (m_pcMainJavaClass != NULL)
	{
		delete m_pcMainJavaClass;
	}

	m_pcMainJavaClass = new char[strlen(pcMainJavaClass)+1];
	strcpy_s(m_pcMainJavaClass, strlen(pcMainJavaClass)+1, pcMainJavaClass);
}

LPCSTR CLaunchConfiguration::getOsgiApplicationName()
{
	return m_pcOsgiApplicationName;
}

void CLaunchConfiguration::setOsgiApplicationName(LPCSTR pcOsgiApplicationName)
{
	assert(pcOsgiApplicationName != NULL);

	if (m_pcOsgiApplicationName != NULL)
	{
		delete m_pcOsgiApplicationName;
	}

	m_pcOsgiApplicationName = new char[strlen(pcOsgiApplicationName)+1];
	strcpy_s(m_pcOsgiApplicationName, strlen(pcOsgiApplicationName)+1, pcOsgiApplicationName);
}

bool CLaunchConfiguration::isConsole()
{
	return m_bConsole;
}

void CLaunchConfiguration::setConsole(bool bConsole)
{
	m_bConsole = bConsole;
}

bool CLaunchConfiguration::isUseLowMemoryJvmSettings()
{
	return m_bUseLowMemoryJvmSettings;
}

void CLaunchConfiguration::useLowMemoryJvmSettings(bool bUseLowMemoryJvmSettings)
{
	m_bUseLowMemoryJvmSettings = bUseLowMemoryJvmSettings;
}

LPCSTR CLaunchConfiguration::getServerStopArgument()
{
	return m_pcServerStopArgument;
}

void CLaunchConfiguration::setServerStopArgument(LPCSTR pcServerStopArgument)
{
	assert(pcServerStopArgument != NULL);

	if (m_pcServerStopArgument != NULL)
	{
		delete m_pcServerStopArgument;
	}

	m_pcServerStopArgument = new char[strlen(pcServerStopArgument)+1];
	strcpy_s(m_pcServerStopArgument, strlen(pcServerStopArgument)+1, pcServerStopArgument);
}

void CLaunchConfiguration::setEnableLoggingInfo(bool bEnableLoggingInfo)
{
	CLog::enableLoggingInfo = bEnableLoggingInfo;
}
