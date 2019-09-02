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

CLaunchConfiguration::CLaunchConfiguration(const CLaunchConfiguration &copy)
: m_pcMainJavaClass(NULL),
  m_pcOsgiApplicationName(NULL),
  m_bUseLowMemoryJvmSettings(copy.m_bUseLowMemoryJvmSettings),
  m_bConsole(copy.m_bConsole),
  m_pcServerStopArgument(NULL)
{
	if (copy.m_pcMainJavaClass != NULL)
	{
		setMainJavaClass(copy.m_pcMainJavaClass);
	}
	if (copy.m_pcOsgiApplicationName != NULL)
	{
		setOsgiApplicationName(copy.m_pcOsgiApplicationName);
	}
	if (copy.m_pcServerStopArgument != NULL)
	{
		setServerStopArgument(copy.m_pcServerStopArgument);
	}
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

CLaunchConfiguration CLaunchConfiguration::getGuiApplicationDefault()
{
	CLaunchConfiguration launchConfiguration;
	launchConfiguration.setMainJavaClass("org.eclipse.equinox.launcher.Main");	
	return launchConfiguration;
}

CLaunchConfiguration CLaunchConfiguration::getServerApplicationDefault()
{
	CLaunchConfiguration launchConfiguration;
	launchConfiguration.setMainJavaClass("org.eclipse.equinox.launcher.Main");	
	launchConfiguration.setEnableLoggingInfo(true);
	return launchConfiguration;
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

CLaunchConfiguration& CLaunchConfiguration::operator=(const CLaunchConfiguration& rightValue)
{
	if (this != &rightValue)
	{
		if (m_pcMainJavaClass != NULL)
		{
			delete m_pcMainJavaClass;
			m_pcMainJavaClass = NULL;
		}
		if (m_pcOsgiApplicationName != NULL)
		{
			delete m_pcOsgiApplicationName;
			m_pcOsgiApplicationName = NULL;
		}
		if (m_pcServerStopArgument != NULL)
		{
			delete m_pcServerStopArgument;
			m_pcServerStopArgument = NULL;
		}
		m_bConsole = rightValue.m_bConsole;
		m_bUseLowMemoryJvmSettings = rightValue.m_bUseLowMemoryJvmSettings;
		if (rightValue.m_pcMainJavaClass != NULL)
		{
			setMainJavaClass(rightValue.m_pcMainJavaClass);
		}
		if (rightValue.m_pcOsgiApplicationName != NULL)
		{
			setOsgiApplicationName(rightValue.m_pcOsgiApplicationName);
		}
		if (rightValue.m_pcServerStopArgument != NULL)
		{
			setServerStopArgument(rightValue.m_pcServerStopArgument);
		}
	}
	return *this;
}