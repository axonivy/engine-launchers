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

int main(int argc, char* argv[])
{
	CJavaWindowsService* pJavaWindowsService;

	try
	{
		pJavaWindowsService = CJavaWindowsService::createJavaWindowsService(getLaunchConfiguration(), true);
		if (argc >= 2)
		{
			if (strcmp(argv[1], "-register")==0)
			{
				if (argc >= 4)
				{
					pJavaWindowsService->registerWindowsService(argv[2], argv[3]);
				}
				else
				{
					pJavaWindowsService->registerWindowsService();
				}
			}
			else if(strcmp(argv[1], "-unregister")==0)
			{
				pJavaWindowsService->unregisterWindowsService();
			}
			else if(strcmp(argv[1], "-start")==0)
			{
				pJavaWindowsService->startWindowsService();
			}
			else if (strcmp(argv[1], "-stop")==0)
			{
				pJavaWindowsService->stopWindowsService();
			}
			else
			{
				CLog::info("Options:");
				CLog::info("-register [service_user, password]");
				CLog::info("-unregister");
				CLog::info("-start");
				CLog::info("-stop");
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
			delete pJavaWindowsService;
		}
		CLog::error(ex.getMessage());
		return ex.getErrorCode();
	}
}
 