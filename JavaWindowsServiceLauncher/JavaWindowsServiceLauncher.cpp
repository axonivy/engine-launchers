/*-----------------------------------------------------------------------------------

   Java NT Service Launcher

-------------------------------------------------------------------------------------

Java NT Service Launcher is a nt service which is implemented with a Java class. 

--------------------------------------------------------------------------------------

Project		: JavaNTServiceLauncher
Version		: 1.0
Compiler	: VC++ 5
Java		: JDK1.2
Author		: Reto Weiss
Copyright	: Ivyteam, 2000

--------------------------------------------------------------------------------------
History:
		  3.8.2000 ReW created
--------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "..\JVMLauncher\JVMLauncher.h"
#include "config.h"

// CLI Tool

// AxonIvyEngineService.exe
// AxonIvyEngineService.exe -start
// AxonIvyEngineService.exe -stop
// AxonIvyEngineService.exe -register
// AxonIvyEngineService.exe -unregister

// AxonIvyEngineService.exe -start <windowsServiceName>
// AxonIvyEngineService.exe -stop <windowsServiceName>
// AxonIvyEngineService.exe -register <windowsServiceName>
// AxonIvyEngineService.exe -unregister <windowsServiceName>

// AxonIvyEngineService.exe -register <windowsServiceName> <service_user> <password>

int main(int argc, char* argv[])
{
	CJavaWindowsService* pJavaWindowsService;
	LPSTR windowsServiceName = "Axon.ivy Engine";
	LPSTR command = NULL;
	try
	{
		if (argc >= 3)
		{
			windowsServiceName = argv[2];
		}
	
		pJavaWindowsService = CJavaWindowsService::createJavaWindowsService(windowsServiceName, getLaunchConfiguration());

		if (argc >= 2)
		{
			command = argv[1];
			if (strcmp(command, "-register")==0)
			{
				if (argc >= 5)
				{
					pJavaWindowsService->registerWindowsService(argv[3], argv[4]);
				}
				else
				{
					pJavaWindowsService->registerWindowsService();
				}
			}
			else if (strcmp(command, "-unregister") == 0)
			{
				pJavaWindowsService->unregisterWindowsService();
			}
			else if (strcmp(command, "-start") == 0)
			{
				pJavaWindowsService->startWindowsService();
			}
			else if (strcmp(command, "-stop") == 0)
			{
				pJavaWindowsService->stopWindowsService();
			}
			else
			{
				CLog::info("Options:");
				CLog::info("-register [windowsServiceName] [service_user, password]");
				CLog::info("-unregister [windowsServiceName]");
				CLog::info("-start [windowsServiceName]");
				CLog::info("-stop [windowsServiceName]");
			}
		}
		else
		{
			pJavaWindowsService->main();
		}
		
		delete pJavaWindowsService;
		return 0;
	}
	catch(CLaunchException ex)
	{
		if (pJavaWindowsService != NULL)
		{
			pJavaWindowsService->reportError(ex);
			delete pJavaWindowsService;
		}
		CLog::error(ex.getMessage());
		return ex.getErrorCode();
	}
}
 