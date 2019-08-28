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

CLaunchConfiguration::CLaunchConfiguration(LPCSTR pcMainJavaClass)
: m_pcAdditionalVmOptions(NULL), m_pcGarbageCollectorOptions(NULL),
  m_jvmType(Disabled), m_pcAuxDirectory(NULL), m_pcJreDirectory(NULL), m_pcMainJavaMethod(NULL), m_pcOsgiApplicationName(NULL), m_bConsole(false), m_pcWindowsServiceName(NULL),
  m_pcApplicationName(NULL), m_bSingleton(false), m_dwManagementPort(DISABLED), m_bAutoDiscovery(true), m_pcServerStopArgument(NULL)
{
	setMainJavaClass(pcMainJavaClass);
}

CLaunchConfiguration::CLaunchConfiguration()
: m_pcMainJavaClass(NULL),
  m_pcAdditionalVmOptions(NULL), m_pcGarbageCollectorOptions(NULL),
  m_jvmType(Disabled), m_pcAuxDirectory(NULL), m_pcJreDirectory(NULL), m_pcMainJavaMethod(NULL), m_pcOsgiApplicationName(NULL), m_bConsole(false), m_pcWindowsServiceName(NULL),
  m_pcApplicationName(NULL), m_bSingleton(false), m_dwManagementPort(DISABLED), m_bAutoDiscovery(true), m_pcServerStopArgument(NULL)
{
}

CLaunchConfiguration::CLaunchConfiguration(const CLaunchConfiguration &copy)
: m_jvmType(copy.m_jvmType),
  m_bSingleton(copy.m_bSingleton),
  m_pcApplicationName(NULL),
  m_pcAuxDirectory(NULL),
  m_pcMainJavaClass(NULL),
  m_pcGarbageCollectorOptions(NULL),
  m_pcJreDirectory(NULL),
  m_pcMainJavaMethod(NULL),
  m_pcOsgiApplicationName(NULL),
  m_bConsole(copy.m_bConsole),
  m_pcWindowsServiceName(NULL),
  m_pcAdditionalVmOptions(NULL),
  m_dwManagementPort(copy.m_dwManagementPort), 
  m_bAutoDiscovery(copy.m_bAutoDiscovery),
  m_pcServerStopArgument(NULL)
{
	if (copy.m_pcMainJavaClass != NULL)
	{
		setMainJavaClass(copy.m_pcMainJavaClass);
	}
	if (copy.m_pcAuxDirectory != NULL)
	{
		setAuxDirectory(copy.m_pcAuxDirectory);
	}
	if (copy.m_pcGarbageCollectorOptions != NULL)
	{
		setGarbageCollectorOptions(copy.m_pcGarbageCollectorOptions);
	}
	if (copy.m_pcJreDirectory != NULL)
	{
		setJreDirectory(copy.m_pcJreDirectory);
	}
	if (copy.m_pcMainJavaMethod != NULL)
	{
		setMainJavaMethod(copy.m_pcMainJavaMethod);
	}
	if (copy.m_pcOsgiApplicationName != NULL)
	{
		setOsgiApplicationName(copy.m_pcOsgiApplicationName);
	}
	if (copy.m_pcWindowsServiceName != NULL)
	{
		setWindowsServiceName(copy.m_pcWindowsServiceName);
	}
	if (copy.m_pcApplicationName != NULL)
	{
		setApplicationName(copy.m_pcApplicationName);
	}
	if (copy.m_pcAdditionalVmOptions != NULL)
	{
		setAdditionalVmOptions(copy.m_pcAdditionalVmOptions);
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
	if (m_pcAuxDirectory != NULL)
	{
		delete m_pcAuxDirectory;
	}
	if (m_pcGarbageCollectorOptions != NULL)
	{
		delete m_pcGarbageCollectorOptions;
	}
	if (m_pcJreDirectory != NULL)
	{
		delete m_pcJreDirectory;
	}
	if (m_pcMainJavaMethod != NULL)
	{
		delete m_pcMainJavaMethod;
	}
	if (m_pcOsgiApplicationName != NULL)
	{
		delete m_pcOsgiApplicationName;
	}
	if (m_pcWindowsServiceName != NULL)
	{
		delete m_pcWindowsServiceName;
	}
	if (m_pcApplicationName != NULL)
	{
		delete m_pcApplicationName;
	}
	if (m_pcAdditionalVmOptions != NULL)
	{
		delete m_pcAdditionalVmOptions;
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
	launchConfiguration.setJvmType(ClientHotspotJVM);
	launchConfiguration.setAdditionalVmOptions("-XX:-OmitStackTraceInFastThrow");
	launchConfiguration.setGarbageCollectorOptions("-XX:+UseG1GC -XX:+UseStringDeduplication");
	return launchConfiguration;
}

CLaunchConfiguration CLaunchConfiguration::getServerApplicationDefault()
{
	CLaunchConfiguration launchConfiguration;
	launchConfiguration.setMainJavaClass("org.eclipse.equinox.launcher.Main");	
	launchConfiguration.setJvmType(ServerHotspotJVM);
	launchConfiguration.setAdditionalVmOptions("-XX:-OmitStackTraceInFastThrow");
	launchConfiguration.setGarbageCollectorOptions("-XX:+UseG1GC -XX:+UseStringDeduplication");
	launchConfiguration.setEnableLoggingInfo(true);
	return launchConfiguration;
}

LPCSTR CLaunchConfiguration::getJreDirectory()
{
	return m_pcJreDirectory;
}

void CLaunchConfiguration::setJreDirectory(LPCSTR pcJreDirectory)
{
	assert(pcJreDirectory != NULL);

	if (m_pcJreDirectory != NULL)
	{
		delete m_pcJreDirectory;
	}

	m_pcJreDirectory = new char[strlen(pcJreDirectory)+1];
	strcpy_s(m_pcJreDirectory, strlen(pcJreDirectory)+1, pcJreDirectory);
}

LPCSTR CLaunchConfiguration::getAuxDirectory()
{
	return m_pcAuxDirectory;
}

void CLaunchConfiguration::setAuxDirectory(LPCSTR pcAuxDirectory)
{
	assert(pcAuxDirectory != NULL);

	if (m_pcAuxDirectory != NULL)
	{
		delete m_pcAuxDirectory;
	}

	m_pcAuxDirectory = new char[strlen(pcAuxDirectory)+1];
	strcpy_s(m_pcAuxDirectory, strlen(pcAuxDirectory)+1, pcAuxDirectory);
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

LPCSTR CLaunchConfiguration::getMainJavaMethod()
{
	return m_pcMainJavaMethod;
}

void CLaunchConfiguration::setMainJavaMethod(LPCSTR pcMainJavaMethod)
{
	assert(pcMainJavaMethod != NULL);

	if (m_pcMainJavaMethod != NULL)
	{
		delete m_pcMainJavaMethod;
	}

	m_pcMainJavaMethod = new char[strlen(pcMainJavaMethod)+1];
	strcpy_s(m_pcMainJavaMethod, strlen(pcMainJavaMethod)+1, pcMainJavaMethod);
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

JVMType CLaunchConfiguration::getJvmType()
{
	return m_jvmType;
}

void CLaunchConfiguration::setJvmType(JVMType jvmType)
{
	m_jvmType = jvmType;
}

LPCSTR CLaunchConfiguration::getGarbageCollectorOptions()
{
	return m_pcGarbageCollectorOptions;
}

void CLaunchConfiguration::setGarbageCollectorOptions(LPCSTR pcGarbageCollectorArgument)
{
	assert(pcGarbageCollectorArgument != NULL);

	if (m_pcGarbageCollectorOptions != NULL)
	{
		delete m_pcGarbageCollectorOptions;
	}

	m_pcGarbageCollectorOptions = new char[strlen(pcGarbageCollectorArgument)+1];
	strcpy_s(m_pcGarbageCollectorOptions, strlen(pcGarbageCollectorArgument)+1, pcGarbageCollectorArgument);
}

LPCSTR CLaunchConfiguration::getAdditionalVmOptions()
{
	return m_pcAdditionalVmOptions;
}

void CLaunchConfiguration::setAdditionalVmOptions(LPCSTR pcAdditionalVmOptions)
{
	assert(pcAdditionalVmOptions != NULL);

	if (m_pcAdditionalVmOptions != NULL)
	{
		delete m_pcAdditionalVmOptions;
	}

	m_pcAdditionalVmOptions = new char[strlen(pcAdditionalVmOptions)+1];
	strcpy_s(m_pcAdditionalVmOptions, strlen(pcAdditionalVmOptions)+1, pcAdditionalVmOptions);
}

LPCSTR CLaunchConfiguration::getWindowsServiceName()
{
	return m_pcWindowsServiceName;
}

void CLaunchConfiguration::setWindowsServiceName(LPCSTR pcWindowsServiceName)
{
	assert(pcWindowsServiceName != NULL);

	if (m_pcWindowsServiceName != NULL)
	{
		delete m_pcWindowsServiceName;
	}

	m_pcWindowsServiceName = new char[strlen(pcWindowsServiceName)+1];
	strcpy_s(m_pcWindowsServiceName, strlen(pcWindowsServiceName)+1, pcWindowsServiceName);
}

LPCSTR CLaunchConfiguration::getApplicationName()
{
	return m_pcApplicationName;
}

void CLaunchConfiguration::setApplicationName(LPCSTR pcApplicationName)
{
	assert(pcApplicationName != NULL);

	if (m_pcApplicationName != NULL)
	{
		delete m_pcApplicationName;
	}

	m_pcApplicationName = new char[strlen(pcApplicationName)+1];
	strcpy_s(m_pcApplicationName, strlen(pcApplicationName)+1, pcApplicationName);
}

bool CLaunchConfiguration::isSingleton()
{
	return m_bSingleton;
}

void CLaunchConfiguration::setSingleton(bool isSingleton)
{
	m_bSingleton = isSingleton;
}

bool CLaunchConfiguration::isAutoDiscovery()
{
	return m_bAutoDiscovery;
}

void CLaunchConfiguration::setAutoDiscovery(bool bAutoDiscovery)
{
	m_bAutoDiscovery = bAutoDiscovery;
}

DWORD CLaunchConfiguration::getManagementPort()
{
	return m_dwManagementPort;
}

void CLaunchConfiguration::setManagementPort(DWORD dwManagementPort)
{
	m_dwManagementPort = dwManagementPort;
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
		if (m_pcAuxDirectory != NULL)
		{
			delete m_pcAuxDirectory;
			m_pcAuxDirectory = NULL;
		}
		if (m_pcGarbageCollectorOptions != NULL)
		{
			delete m_pcGarbageCollectorOptions;
			m_pcGarbageCollectorOptions = NULL;
		}
		if (m_pcJreDirectory != NULL)
		{
			delete m_pcJreDirectory;
			m_pcJreDirectory = NULL;
		}
		if (m_pcMainJavaMethod != NULL)
		{
			delete m_pcMainJavaMethod;
			m_pcMainJavaMethod = NULL;
		}
		if (m_pcOsgiApplicationName != NULL)
		{
			delete m_pcOsgiApplicationName;
			m_pcOsgiApplicationName = NULL;
		}
		if (m_pcWindowsServiceName != NULL)
		{
			delete m_pcWindowsServiceName;
			m_pcWindowsServiceName = NULL;
		}
		if (m_pcApplicationName != NULL)
		{
			delete m_pcApplicationName;
			m_pcApplicationName = NULL;
		}
		if (m_pcAdditionalVmOptions != NULL)
		{
			delete m_pcAdditionalVmOptions;
			m_pcAdditionalVmOptions = NULL;
		}
		if (m_pcServerStopArgument != NULL)
		{
			delete m_pcServerStopArgument;
			m_pcServerStopArgument = NULL;
		}		
		m_jvmType = rightValue.m_jvmType;
		m_bSingleton = rightValue.m_bSingleton;
		m_bConsole = rightValue.m_bConsole;
		m_bAutoDiscovery = rightValue.m_bAutoDiscovery;
		m_dwManagementPort = rightValue.m_dwManagementPort;
		if (rightValue.m_pcMainJavaClass != NULL)
		{
			setMainJavaClass(rightValue.m_pcMainJavaClass);
		}
		if (rightValue.m_pcAuxDirectory != NULL)
		{
			setAuxDirectory(rightValue.m_pcAuxDirectory);
		}
		if (rightValue.m_pcGarbageCollectorOptions != NULL)
		{
			setGarbageCollectorOptions(rightValue.m_pcGarbageCollectorOptions);
		}
		if (rightValue.m_pcJreDirectory != NULL)
		{
			setJreDirectory(rightValue.m_pcJreDirectory);
		}
		if (rightValue.m_pcMainJavaMethod != NULL)
		{
			setMainJavaMethod(rightValue.m_pcMainJavaMethod);
		}
		if (rightValue.m_pcOsgiApplicationName != NULL)
		{
			setOsgiApplicationName(rightValue.m_pcOsgiApplicationName);
		}
		if (rightValue.m_pcWindowsServiceName != NULL)
		{
			setWindowsServiceName(rightValue.m_pcWindowsServiceName);
		}
		if (rightValue.m_pcApplicationName != NULL)
		{
			setApplicationName(rightValue.m_pcApplicationName);
		}
		if (rightValue.m_pcAdditionalVmOptions != NULL)
		{
			setAdditionalVmOptions(rightValue.m_pcAdditionalVmOptions);
		}
		if (rightValue.m_pcServerStopArgument != NULL)
		{
			setServerStopArgument(rightValue.m_pcServerStopArgument);
		}
	}
	return *this;
}