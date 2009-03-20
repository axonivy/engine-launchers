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

CLaunchConfiguration::CLaunchConfiguration(LPCSTR pcMainJavaClass)
: m_pcAdditionalVmOptions(NULL), m_pcGarbageCollectorOptions(NULL), m_dwHeapEdenToSurvivorRatio(DISABLED),
  m_dwHeapMaxFreeRatio(DISABLED), m_dwHeapMaxRatio(DISABLED), m_dwHeapMaxSize(DISABLED), m_dwHeapMaxYoungSize(DISABLED), m_dwHeapMinFreeRatio(DISABLED), m_dwHeapMinYoungSize(DISABLED),
  m_dwHeapStartSize(DISABLED), m_dwHeapTenuredToYoungRatio(DISABLED), m_dwHeapMaxPermSize(DISABLED),
  m_jvmType(Disabled), m_pcAuxDirectory(NULL), m_pcJreDirectory(NULL), m_pcMainJavaMethod(NULL), m_pcWindowsServiceName(NULL),
  m_pcApplicationName(NULL), m_bSingleton(false)
{
	setMainJavaClass(pcMainJavaClass);
}

CLaunchConfiguration::CLaunchConfiguration()
: m_pcMainJavaClass(NULL),
  m_pcAdditionalVmOptions(NULL), m_pcGarbageCollectorOptions(NULL), m_dwHeapEdenToSurvivorRatio(DISABLED),
  m_dwHeapMaxFreeRatio(DISABLED), m_dwHeapMaxRatio(DISABLED), m_dwHeapMaxSize(DISABLED), m_dwHeapMaxYoungSize(DISABLED), m_dwHeapMinFreeRatio(DISABLED), m_dwHeapMinYoungSize(DISABLED),
  m_dwHeapStartSize(DISABLED), m_dwHeapTenuredToYoungRatio(DISABLED), m_dwHeapMaxPermSize(DISABLED),
  m_jvmType(Disabled), m_pcAuxDirectory(NULL), m_pcJreDirectory(NULL), m_pcMainJavaMethod(NULL),  m_pcWindowsServiceName(NULL),
  m_pcApplicationName(NULL), m_bSingleton(false)
{
}

CLaunchConfiguration::CLaunchConfiguration(const CLaunchConfiguration &copy)
: m_dwHeapEdenToSurvivorRatio(copy.m_dwHeapEdenToSurvivorRatio),
  m_dwHeapMaxFreeRatio(copy.m_dwHeapMaxFreeRatio), 
  m_dwHeapMaxSize(copy.m_dwHeapMaxSize), 
  m_dwHeapMaxRatio(copy.m_dwHeapMaxRatio),
  m_dwHeapMaxYoungSize(copy.m_dwHeapMaxYoungSize),
  m_dwHeapMinFreeRatio(copy.m_dwHeapMinFreeRatio), 
  m_dwHeapMinYoungSize(copy.m_dwHeapMinYoungSize),
  m_dwHeapStartSize(copy.m_dwHeapStartSize), 
  m_dwHeapTenuredToYoungRatio(copy.m_dwHeapTenuredToYoungRatio), 
  m_dwHeapMaxPermSize(copy.m_dwHeapMaxPermSize),
  m_jvmType(copy.m_jvmType),
  m_bSingleton(copy.m_bSingleton),
  m_pcApplicationName(NULL),
  m_pcAuxDirectory(NULL),
  m_pcMainJavaClass(NULL),
  m_pcGarbageCollectorOptions(NULL),
  m_pcJreDirectory(NULL),
  m_pcMainJavaMethod(NULL),
  m_pcWindowsServiceName(NULL),
  m_pcAdditionalVmOptions(NULL)
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
}

CLaunchConfiguration CLaunchConfiguration::getGuiApplicationDefault()
{
	CLaunchConfiguration launchConfiguration;
	launchConfiguration.setHeapMaxRatio(80);	// 80% of physical memory
	launchConfiguration.setHeapStartSize(64); // 64 MBytes
	launchConfiguration.setJvmType(ClientHotspotJVM); // use client hotspot
	launchConfiguration.setAdditionalVmOptions("-Xverify:none -XX:-OmitStackTraceInFastThrow");
	launchConfiguration.setGarbageCollectorOptions("-XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:+CMSParallelRemarkEnabled -XX:+DisableExplicitGC");

	return launchConfiguration;
}

CLaunchConfiguration CLaunchConfiguration::getServerApplicationDefault()
{
	CLaunchConfiguration launchConfiguration;
	launchConfiguration.setHeapMaxRatio(80);	// 80% of physical memory
	launchConfiguration.setHeapStartSize(64); // 64 MBytes
	launchConfiguration.setJvmType(ServerHotspotJVM); // use server hotspot
	launchConfiguration.setAdditionalVmOptions("-Xverify:none -XX:-OmitStackTraceInFastThrow");
	launchConfiguration.setGarbageCollectorOptions("-XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:+CMSParallelRemarkEnabled -XX:+DisableExplicitGC");
	return launchConfiguration;
}

CLaunchConfiguration CLaunchConfiguration::overwrite(CLaunchConfiguration launchConfiguration)
{
	CLaunchConfiguration overwrittenLaunchConfiguration(*this);

	if (launchConfiguration.getAdditionalVmOptions() != NULL)
	{
		overwrittenLaunchConfiguration.setAdditionalVmOptions(launchConfiguration.getAdditionalVmOptions());
	}

	if (launchConfiguration.getAuxDirectory() != NULL)
	{
		overwrittenLaunchConfiguration.setAuxDirectory(launchConfiguration.getAuxDirectory());
	}

	if (launchConfiguration.getGarbageCollectorOptions() != NULL)
	{
		overwrittenLaunchConfiguration.setGarbageCollectorOptions(launchConfiguration.getGarbageCollectorOptions());
	}

	if (launchConfiguration.getHeapEdenToSurvivorRatio() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapEdenToSurvivorRatio(launchConfiguration.getHeapEdenToSurvivorRatio());
	}

	if (launchConfiguration.getHeapMaxFreeRatio() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapMaxFreeRatio(launchConfiguration.getHeapMaxFreeRatio());
	}

	if (launchConfiguration.getHeapMaxSize() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapMaxSize(launchConfiguration.getHeapMaxSize());
		overwrittenLaunchConfiguration.setHeapMaxRatio(DISABLED);
	}

	if (launchConfiguration.getHeapMaxRatio() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapMaxRatio(launchConfiguration.getHeapMaxRatio());
		overwrittenLaunchConfiguration.setHeapMaxSize(DISABLED);
	}

	if (launchConfiguration.getHeapMaxYoungSize() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapMaxYoungSize(launchConfiguration.getHeapMaxYoungSize());
	}

	if (launchConfiguration.getHeapMinFreeRatio() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapMinFreeRatio(launchConfiguration.getHeapMinFreeRatio());
	}

	if (launchConfiguration.getHeapMinYoungSize() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapMinYoungSize(launchConfiguration.getHeapMinYoungSize());
	}

	if (launchConfiguration.getHeapStartSize() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapStartSize(launchConfiguration.getHeapStartSize());
	}

	if (launchConfiguration.getHeapTenuredToYoungRatio() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapTenuredToYoungRatio(launchConfiguration.getHeapTenuredToYoungRatio());
	}
	
	if (launchConfiguration.getHeapMaxPermSize() != DISABLED)
	{
		overwrittenLaunchConfiguration.setHeapMaxPermSize(launchConfiguration.getHeapMaxPermSize());
	}

	if (launchConfiguration.getJreDirectory() != NULL)
	{
		overwrittenLaunchConfiguration.setJreDirectory(launchConfiguration.getJreDirectory());
	}

	if (launchConfiguration.getJvmType() != Disabled)
	{
		overwrittenLaunchConfiguration.setJvmType(launchConfiguration.getJvmType());
	}

	if (launchConfiguration.getMainJavaClass() != NULL)
	{
		overwrittenLaunchConfiguration.setMainJavaClass(launchConfiguration.getMainJavaClass());
	}

	if (launchConfiguration.getMainJavaMethod() != NULL)
	{
		overwrittenLaunchConfiguration.setMainJavaMethod(launchConfiguration.getMainJavaMethod());
	}

	if (launchConfiguration.getWindowsServiceName() != NULL)
	{
		overwrittenLaunchConfiguration.setWindowsServiceName(launchConfiguration.getWindowsServiceName());
	}

	if (launchConfiguration.getApplicationName() != NULL)
	{
		overwrittenLaunchConfiguration.setApplicationName(launchConfiguration.getApplicationName());
	}

	if (launchConfiguration.isSingleton() != isSingleton())
	{
		overwrittenLaunchConfiguration.setSingleton(launchConfiguration.isSingleton());
	}

	return overwrittenLaunchConfiguration;
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

DWORD CLaunchConfiguration::getHeapMaxSize()
{
	return m_dwHeapMaxSize;
}

void CLaunchConfiguration::setHeapMaxSize(DWORD dwHeapMaxSize)
{
	m_dwHeapMaxSize = dwHeapMaxSize;
}

DWORD CLaunchConfiguration::getHeapMaxRatio()
{
	return m_dwHeapMaxRatio;
}

void CLaunchConfiguration::setHeapMaxRatio(DWORD dwHeapMaxRatio)
{
	m_dwHeapMaxRatio = dwHeapMaxRatio;
}

DWORD CLaunchConfiguration::getHeapStartSize()
{
	return m_dwHeapStartSize;
}

void CLaunchConfiguration::setHeapStartSize(DWORD dwHeapStartSize)
{
	m_dwHeapStartSize = dwHeapStartSize;
}

DWORD CLaunchConfiguration::getHeapMinFreeRatio()
{
	return m_dwHeapMinFreeRatio;
}

void CLaunchConfiguration::setHeapMinFreeRatio(DWORD dwHeapMinFreeRatio)
{
	m_dwHeapMinFreeRatio = dwHeapMinFreeRatio; 
}

DWORD CLaunchConfiguration::getHeapMaxFreeRatio()
{
	return m_dwHeapMaxFreeRatio;
}

void CLaunchConfiguration::setHeapMaxFreeRatio(DWORD dwHeapMaxFreeRatio)
{
	m_dwHeapMaxFreeRatio = dwHeapMaxFreeRatio;
}

DWORD CLaunchConfiguration::getHeapTenuredToYoungRatio()
{
	return m_dwHeapTenuredToYoungRatio;
}

void CLaunchConfiguration::setHeapTenuredToYoungRatio(DWORD dwHeapTenuredToYoungRatio)
{
	m_dwHeapTenuredToYoungRatio = dwHeapTenuredToYoungRatio;
}

DWORD CLaunchConfiguration::getHeapMinYoungSize()
{
	return m_dwHeapMinYoungSize;
}

void CLaunchConfiguration::setHeapMinYoungSize(DWORD dwHeapMinYoungSize)
{
	m_dwHeapMinYoungSize = dwHeapMinYoungSize;
}

DWORD CLaunchConfiguration::getHeapMaxYoungSize()
{
	return m_dwHeapMaxYoungSize;
}

void CLaunchConfiguration::setHeapMaxYoungSize(DWORD dwHeapMaxYoungSize)
{
	m_dwHeapMaxYoungSize = dwHeapMaxYoungSize;
}

DWORD CLaunchConfiguration::getHeapEdenToSurvivorRatio()
{
	return m_dwHeapEdenToSurvivorRatio;
}

void CLaunchConfiguration::setHeapEdenToSurvivorRatio(DWORD dwHeapEdenToSurvivorRatio)
{
	m_dwHeapEdenToSurvivorRatio = dwHeapEdenToSurvivorRatio;
}

DWORD CLaunchConfiguration::getHeapMaxPermSize()
{
	return m_dwHeapMaxPermSize;
}

void CLaunchConfiguration::setHeapMaxPermSize(DWORD dwHeapMaxPermSize)
{
	m_dwHeapMaxPermSize = dwHeapMaxPermSize;
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
		m_dwHeapEdenToSurvivorRatio = rightValue.m_dwHeapEdenToSurvivorRatio;
		m_dwHeapMaxFreeRatio = rightValue.m_dwHeapMaxFreeRatio; 
		m_dwHeapMaxSize = rightValue.m_dwHeapMaxSize;
		m_dwHeapMaxRatio = rightValue.m_dwHeapMaxRatio;
		m_dwHeapMaxYoungSize = rightValue.m_dwHeapMaxYoungSize;
		m_dwHeapMinFreeRatio = rightValue.m_dwHeapMinFreeRatio;
		m_dwHeapMinYoungSize = rightValue.m_dwHeapMinYoungSize;
		m_dwHeapStartSize = rightValue.m_dwHeapStartSize;
		m_dwHeapTenuredToYoungRatio = rightValue.m_dwHeapTenuredToYoungRatio;
		m_dwHeapMaxPermSize = rightValue.m_dwHeapMaxPermSize;
		m_jvmType = rightValue.m_jvmType;
		m_bSingleton = rightValue.m_bSingleton;
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
	}
	return *this;
}