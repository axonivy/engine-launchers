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

#define JVMLauncher

#include "WindowsService.h"
#include "LaunchException.h"

CWindowsService::CWindowsService(LPCSTR pcWindowsServiceName)
: m_pcWindowsServiceName(pcWindowsServiceName)
{}

void CWindowsService::registerWindowsService(
		LPCSTR pcUserName,
		LPCSTR pcPassword,
		DWORD dwStartType,
		DWORD dwDesiredAccess,
		DWORD dwErrorControl)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	char binPath[MAX_PATH];

	/* Get the full module filename and cut it after the last directory */
    GetModuleFileName(NULL, binPath, MAX_PATH);


	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_OPEN_SERVICE_CTRL_MANAGER, GetLastError(), 
			"Could not create service control manager for service %s", m_pcWindowsServiceName);
	}

    hSCService = CreateService( 
        hSCManager,               // SCManager database 
        TEXT(m_pcWindowsServiceName),     // name of service 
        m_pcWindowsServiceName,           // service name to display 
        dwDesiredAccess,	       // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        dwStartType,			   // start type 
        dwErrorControl,			   // error control type 
        binPath,			       // service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        pcUserName,                // LocalSystem account 
        pcPassword);               // no password 
 
    if (hSCService == NULL) 
	{
		CloseServiceHandle(hSCManager);
        throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CREATE_SERVICE, GetLastError(), 
			"Could not create service '%s'", m_pcWindowsServiceName);
	}
    
    CloseServiceHandle(hSCService); 
	CloseServiceHandle(hSCManager);
}

void CWindowsService::unregisterWindowsService()
{
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	
	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		throw new CLaunchException(JVMLauncherErrorCodes.COULD_NOT_OPEN_SERVICE_CTRL_MANAGER, GetLastError(), 
			"Could not create service control manager for service %s", m_pcWindowsServiceName);
	}
	
	hSCService = OpenService( 
        hSCManager,				// SCManager database 
        TEXT(m_pcWindowsServiceName),	// name of service 
        DELETE);				// only need DELETE access 
 
    if (hSCService == NULL) 
	{
		CloseServiceHandle(hSCManager);
		throw new CLaunchException(JVMLauncherErrorCodes.COULD_NOT_OPEN_SERVICE, GetLastError(), 
			"Could not open service '%s'", m_pcWindowsServiceName);
	}
 
    if (! DeleteService(hSCService) ) 
	{
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hSCService);
		throw new CLaunchException(JVMLauncherErrorCodes.COULD_NOT_DELETE_SERVICE, GetLastError(),
			"Could not delete service '%s'", m_pcWindowsServiceName);
	}
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hSCService);
}

void CWindowsService::startWindowsService()
{
	SERVICE_STATUS ssStatus; 
    DWORD dwOldCheckPoint; 
	DWORD dwWaitingTime=0;

	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	
	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_OPEN_SERVICE_CTRL_MANAGER, GetLastError(),
			"Could not create service control manager for service %s", m_pcWindowsServiceName);
	}
 
    hSCService = OpenService( 
        hSCManager,				// SCM database 
        TEXT(m_pcWindowsServiceName),  // service name
        SERVICE_ALL_ACCESS); 
 
    if (hSCService == NULL) 
	{
		CloseServiceHandle(hSCManager);  
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_OPEN_SERVICE, GetLastError(),
			"Cound not open service '%s'", m_pcWindowsServiceName);
	}
 
    if (!StartService(
            hSCService,  // handle to service 
            0,           // number of arguments 
            NULL) )      // no arguments 
    {
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hSCService); 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_START_SERVICE, GetLastError(),
			"Could not start service %s", m_pcWindowsServiceName);
    }
 
	// Check the status until the service is no longer start pending. 
    if (!QueryServiceStatus( 
            hSCService,   // handle to service 
            &ssStatus) )  // address of status information
	{
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hSCService); 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_QUERY_SERVICE_STATUS, GetLastError(),
			"Could not query status of service %s", m_pcWindowsServiceName);
	}
 
    while (ssStatus.dwCurrentState != SERVICE_RUNNING) 
    { 
        // Save the current checkpoint. 
        dwOldCheckPoint = ssStatus.dwCheckPoint; 
 
        // Wait for the specified interval. 
		if (ssStatus.dwWaitHint != 0)
		{
			dwWaitingTime+=ssStatus.dwWaitHint;
			Sleep(ssStatus.dwWaitHint); 
		}
		else
		{
			dwWaitingTime+=1000;
			Sleep(1000);
		}
 
        // Check the status again. 
         if (!QueryServiceStatus( 
                hSCService,   // handle to service 
                &ssStatus) )  // address of status information
		 {
			CloseServiceHandle(hSCManager);
			CloseServiceHandle(hSCService); 
			throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_QUERY_SERVICE_STATUS, GetLastError(),
				"Could not query status of service %s", m_pcWindowsServiceName);		 
		 }
 
        // Break if the checkpoint has not been incremented and waiting time > 60s
        if (dwOldCheckPoint == ssStatus.dwCheckPoint) 
		{
			if (dwWaitingTime > 60000)
			{
				CloseServiceHandle(hSCManager);
				CloseServiceHandle(hSCService); 
				throw CLaunchException(JVMLauncherErrorCodes.ERROR_SERVICE_HAS_NOT_STARTED, 
					"Service %s has not started and has not increased processing check point within 60 seconds", m_pcWindowsServiceName);
			}
		}
    } 
 	
	CloseServiceHandle(hSCManager);
    CloseServiceHandle(hSCService); 
}

void CWindowsService::stopWindowsService()
{
	SERVICE_STATUS ssStatus; 
    DWORD dwOldCheckPoint; 
	DWORD dwWaitingTime=0;

	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	
	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_OPEN_SERVICE_CTRL_MANAGER, GetLastError(),
			"Could not create service control manager for service %s", m_pcWindowsServiceName);
	}
 
    hSCService = OpenService( 
        hSCManager,				// SCM database 
        TEXT(m_pcWindowsServiceName),  // service name
        SERVICE_ALL_ACCESS); 
 
    if (hSCService == NULL) 
	{
		CloseServiceHandle(hSCManager);  
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_OPEN_SERVICE, GetLastError(),	
			"Could not open service '%s'", m_pcWindowsServiceName);
	}
 
    if (!ControlService(
            hSCService,				// handle to service 
            SERVICE_CONTROL_STOP,   // control command
            &ssStatus) )			 // no arguments 
    {
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hSCService); 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_STOP_SERVICE, GetLastError(),
			"Could not stop service %s", m_pcWindowsServiceName);
    }
 
    while (ssStatus.dwCurrentState != SERVICE_STOPPED) 
    { 
        // Save the current checkpoint. 
         dwOldCheckPoint = ssStatus.dwCheckPoint; 
 
        // Wait for the specified interval. 
		 if (ssStatus.dwWaitHint != 0)
		 {
			dwWaitingTime+=ssStatus.dwWaitHint;
			Sleep(ssStatus.dwWaitHint); 
		 }
		 else
		 {
			dwWaitingTime+=1000;
			Sleep(1000);
		 }
 
        // Check the status again. 
         if (!QueryServiceStatus( 
                hSCService,   // handle to service 
                &ssStatus) )  // address of status information
		 {
			CloseServiceHandle(hSCManager);
			CloseServiceHandle(hSCService);
			throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_QUERY_SERVICE_STATUS, GetLastError(),
					"Could not query status of service '%s'", m_pcWindowsServiceName);
			break;
		 }
 
        // Break if the checkpoint has not been incremented and waiting time > 60s. 
        if (dwOldCheckPoint == ssStatus.dwCheckPoint) 
		{
			if (dwWaitingTime > 60000)
			{
				CloseServiceHandle(hSCManager);
				CloseServiceHandle(hSCService);
				throw CLaunchException(JVMLauncherErrorCodes.ERROR_SERVICE_HAS_NOT_STOPPED, 
					"Service %s has not stopped and has not increased processing check point within 60 seconds", m_pcWindowsServiceName);
			}
		}
    } 
 	
	CloseServiceHandle(hSCManager);
    CloseServiceHandle(hSCService); 
}
