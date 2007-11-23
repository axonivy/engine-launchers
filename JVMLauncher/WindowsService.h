/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

Register, unregister, start and stop a nt service

--------------------------------------------------------------------------------------

Project		: JVMLauncher
Version		: 1.0
Compiler	: VC++ 6
Java		: JDK1.4
Author		: Reto Weiss
Copyright	: Ivyteam, 2006

--------------------------------------------------------------------------------------
History:
		  28.3.2006 ReW created
--------------------------------------------------------------------------------------*/

#ifndef __Windows_SERVICE_H__
#define __Windows_SERVICE_H__

#include <windows.h>

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

class JVMLauncherExport CWindowsService  
{
private:
	/* The windows service name */
	LPCSTR m_pcWindowsServiceName;

public:
	/*
	 * Constructor
	 * @param pcWindowsServiceName the windows service name 
	 */
	CWindowsService(LPCSTR pcWindowsSeviceName);

	/*
	 * Registers the Windows Service
	 * @param pcUserName Name of user as this service should interact as (NULL means SystemUser)
	 * @param pcPassword Password of service user (can be NULL for SystemUser)
	 * @param dwStartType How is the service started. See CreateService of Win32 documentation for details
	 * @param dwDesiredAccess What access rights has the service. See CreateService of Win32 documentation for details
	 * @param dwErrorControl What happens if the service can not started. See CreateService of Win32 documentation for details
	 * @throws CLaunchException if Windows Service cannot be registered
	 */
	void registerWindowsService(
			LPCSTR pcUserName = NULL,
			LPCSTR pcPassword = NULL,
			DWORD dwStartType = SERVICE_AUTO_START,
			DWORD dwDesiredAccess = SERVICE_ALL_ACCESS,
			DWORD dwErrorControl = SERVICE_ERROR_NORMAL);

	/*
	 * Unregisters the Windows Service
	 * @throws CLaunchException if Windows service cannot be unregistered
	 */
	void unregisterWindowsService();

	/*
	 * Starts the Windows Service
	 * @throws CLaunchException if Windows service cannot be started
	 */
	void startWindowsService();

	/*
	 * Stops the Windows Service
	 * @throws CLaunchException if Windows service cannot be started
	 */
	void stopWindowsService();
};

#endif 
