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
#ifndef __LAUNCH_CONFIGURATION_H__
#define __LAUNCH_CONFIGURATION_H__

#include <windows.h>

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

/*
 * JVM type enumeration
 */
typedef enum{Disabled=-1, ClientHotspotJVM=1, ServerHotspotJVM=2} JVMType;


/*
 Holds the java virtual machine launch configuration 
 */
class JVMLauncherExport CLaunchConfiguration
{
private:
	// --- Application  ---
	/* the application name */
	LPSTR m_pcApplicationName;
	/* is a singleton application */
	bool m_bSingleton;

	// --- Windows Service ---
	/* The windows service name */
	LPSTR m_pcWindowsServiceName;

	// --- Directories ---

	/* The java runtime engine directory */
	LPSTR m_pcJreDirectory;
	/* The directory where the project specific libs are located */
	LPSTR m_pcAuxDirectory;

	// --- What to launch ---

	/* The name of the main java class to launch */
	LPSTR m_pcMainJavaClass;
	/* The name of the main java method to launch */
	LPSTR m_pcMainJavaMethod;

	// --- Heap settings ---

	/* The max heap size in mega bytes. -1 means disabled. Note either m_HeapMaxSize or m_HeapMaxRatio may be defined, but not both. -Xmx */
	DWORD m_dwHeapMaxSize;
	/* The max heap size in percentage of the physical memory. -1 means disable. Note either m_HeapMaxSize or m_HeapMaxRatio may be defined, but not both. -Xmx */
	DWORD m_dwHeapMaxRatio;

	/* the start heap size in mega bytes. -1 means disabled. -Xms */
	DWORD m_dwHeapStartSize;
	/* The minimum free heap size in percent. 
	   The heap is expanded if min free heap size is not available. 
	   -1 means disabled. -XX:MinHeapFreeRatio */
	DWORD m_dwHeapMinFreeRatio;
	/* The maximum free heap size in percent. 
	   The heap is shrinked if max free heap size is available. 
	   -1 means disabled. -XX:MaxHeapFreeRatio */
	DWORD m_dwHeapMaxFreeRatio;
	/* The tenured to young ratio. 
	   Ration between young and tenured generation heap space. 
	   4 means that the tenured part divided to the young part is 4/1. 
	   -1 means disabled. -XX:NewRatio */
	DWORD m_dwHeapTenuredToYoungRatio;
	/* The minimum size of the young generation heap size in mega bytes. 
	   -1 means disabled. -XX:NewSize */
	DWORD m_dwHeapMinYoungSize;
	/* The maximum size of the young generation heap size in mega bytes. 
	   -1 means disabled. -XX:MaxNewSize */
	DWORD m_dwHeapMaxYoungSize;
	/* The eden to survivor ratio. 
	   Ratio between the survivor heap space and the eden heap space. 
	   6 means that eden part divided to survivor part is 6/1. 
	   -1 means disabled. -XX:SurvivorRatio */
	DWORD m_dwHeapEdenToSurvivorRatio;

	/* The maximum of the permanent size in mega  bytes.-1 means disabled. -XX:MaxPermSize */
	DWORD m_dwHeapMaxPermSize;

	// --- JVM Settings ---

	/* The java virtual machine type */
	JVMType m_jvmType;
	/* The garbage collector options */
	LPSTR m_pcGarbageCollectorOptions;
	/* Additional vm options */
	LPSTR m_pcAdditionalVmOptions;

	// --- Managment Settings ---

	/** -1 means disable jmx */
	DWORD m_dwManagementPort;

	bool m_bAutoDiscovery;

	LPSTR m_pcServerStopArgument;

public:
	/* Value that means that a launch configuration property is DISABLED */
	static const DWORD DISABLED = -1;

	/*
	 * Constructor
	 * @param pcMainJavaClass the main java class
	 */
	CLaunchConfiguration(LPCSTR pcMainJavaClass);

	/*
	 * Copy Constructor
	 * @param copy the object to copy
	 */
	CLaunchConfiguration(const CLaunchConfiguration &copy);

	/*
	 * Default constructor
	 */
	CLaunchConfiguration();

	/*
	 *Destructor 
	 */
	~CLaunchConfiguration();

	/*
	 * Gets a default launch configuration for a gui application
	 */
	static CLaunchConfiguration getGuiApplicationDefault();

	/*
	 * Gets a default launch configuration for a server application
	 */
	static CLaunchConfiguration getServerApplicationDefault();

	/*
	 * Overwrites the launch configuration with another launch configuration
	 * @param launchConfiguration the launch configuration to overwrite this launch configuration
	 * @return the overwritten launch configuration
	 */
	CLaunchConfiguration overwrite(CLaunchConfiguration launchConfiguration);
	
	LPCSTR getJreDirectory();
	void setJreDirectory(LPCSTR pcJreDirectory);
	
	LPCSTR getAuxDirectory();
	void setAuxDirectory(LPCSTR pcAuxDirectory);
	
	LPCSTR getMainJavaClass();
	void setMainJavaClass(LPCSTR pcMainJavaClass);
	
	LPCSTR getMainJavaMethod();
	void setMainJavaMethod(LPCSTR pcMainJavaMethod);
	
	DWORD getHeapMaxSize();
	void setHeapMaxSize(DWORD dwHeapMaxSize);

	DWORD getHeapMaxRatio();
	void setHeapMaxRatio(DWORD dwHeapMaxRatio);

	DWORD getHeapStartSize();
	void setHeapStartSize(DWORD dwHeapStartSize);

	DWORD getHeapMinFreeRatio();
	void setHeapMinFreeRatio(DWORD dwHeapMinFreeRatio);

	DWORD getHeapMaxFreeRatio();
	void setHeapMaxFreeRatio(DWORD dwHeapMaxFreeRatio);

	DWORD getHeapTenuredToYoungRatio();
	void setHeapTenuredToYoungRatio(DWORD dwHeapTenuredToYoungRatio);

	DWORD getHeapMinYoungSize();
	void setHeapMinYoungSize(DWORD dwHeapMinYoungSize);

	DWORD getHeapMaxYoungSize();
	void setHeapMaxYoungSize(DWORD dwHeapMaxYoungSize);

	DWORD getHeapEdenToSurvivorRatio();
	void setHeapEdenToSurvivorRatio(DWORD dwHeapEdenToSurvivorRatio);

	DWORD getHeapMaxPermSize();
	void setHeapMaxPermSize(DWORD dwHeapMaxPermSize);
	
	JVMType getJvmType();
	void setJvmType(JVMType jvmType);

	LPCSTR getGarbageCollectorOptions();
	void setGarbageCollectorOptions(LPCSTR pcGarbageCollectorOptions);

	LPCSTR getAdditionalVmOptions();
	void setAdditionalVmOptions(LPCSTR pcAdditionalVmOptions);

	LPCSTR getWindowsServiceName();
	void setWindowsServiceName(LPCSTR pcWindowsServiceName);

	LPCSTR getApplicationName();
	void setApplicationName(LPCSTR pcApplicationName);

	bool isSingleton();
	void setSingleton(bool isSingleton);

	DWORD getManagementPort();
	void setManagementPort(DWORD dwManagementPort);

	bool isAutoDiscovery();
	void setAutoDiscovery(bool bAutoDiscovery);

	LPCSTR getServerStopArgument();
	void setServerStopArgument(LPCSTR pcServerStopArgument);

	/*
	 * Assign operator
	 * @param rightValue the right value of the assign operator
	 * @return the result of the assign operation
	 */
	CLaunchConfiguration& operator=(const CLaunchConfiguration& rightValue);
};
#endif