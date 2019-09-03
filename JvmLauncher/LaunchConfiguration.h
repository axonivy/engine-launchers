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
 Holds the java virtual machine launch configuration 
 */
class JVMLauncherExport CLaunchConfiguration
{
private:
	LPSTR m_pcMainJavaClass;
	LPSTR m_pcOsgiApplicationName;
	bool m_bConsole;
	LPSTR m_pcServerStopArgument;
	bool m_bUseLowMemoryJvmSettings;

public:
	/*
	 * Copy Constructor
	 * @param copy the object to copy
	 */
	CLaunchConfiguration(const CLaunchConfiguration &copy);

	CLaunchConfiguration();
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

	LPCSTR getServerStopArgument();
	void setServerStopArgument(LPCSTR pcServerStopArgument);

	void setEnableLoggingInfo(bool bEnableLoggingInfo);

	void useLowMemoryJvmSettings(bool bUseLowMemoryJvmSettings);
	bool isUseLowMemoryJvmSettings();

	/*
	 * Assign operator
	 * @param rightValue the right value of the assign operator
	 * @return the result of the assign operation
	 */
	CLaunchConfiguration& operator=(const CLaunchConfiguration& rightValue);
};
#endif