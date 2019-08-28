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

	// --- Windows Service ---
	/* The windows service name */
	LPSTR m_pcWindowsServiceName;

	// --- What to launch ---
	/* The name of the main java class to launch */
	LPSTR m_pcMainJavaClass;
	/* OSGI application name */
	LPSTR m_pcOsgiApplicationName;
	/* Console mode */
	bool m_bConsole;

	// --- JVM Settings ---

	/* The java virtual machine type */
	JVMType m_jvmType;
	/* Additional vm options */
	LPSTR m_pcAdditionalVmOptions;

	// --- Managment Settings ---
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
		
	LPCSTR getMainJavaClass();
	void setMainJavaClass(LPCSTR pcMainJavaClass);
	
	LPCSTR getOsgiApplicationName();
	void setOsgiApplicationName(LPCSTR pcOsgiApplicationName);
	
	bool isConsole();
	void setConsole(bool bConsole);

	JVMType getJvmType();
	void setJvmType(JVMType jvmType);

	LPCSTR getAdditionalVmOptions();
	void setAdditionalVmOptions(LPCSTR pcAdditionalVmOptions);

	LPCSTR getWindowsServiceName();
	void setWindowsServiceName(LPCSTR pcWindowsServiceName);

	LPCSTR getApplicationName();
	void setApplicationName(LPCSTR pcApplicationName);

	LPCSTR getServerStopArgument();
	void setServerStopArgument(LPCSTR pcServerStopArgument);

	void setEnableLoggingInfo(bool bEnableLoggingInfo);

	/*
	 * Assign operator
	 * @param rightValue the right value of the assign operator
	 * @return the result of the assign operation
	 */
	CLaunchConfiguration& operator=(const CLaunchConfiguration& rightValue);
};
#endif