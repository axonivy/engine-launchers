/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

A java nt service. This is an nt service that calls certain static java methods of a java class
if some events on the nt service happends. 

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

#include "JavaWindowsService.h"
#include "LaunchException.h"
#include "WindowsService.h"
#include "Log.h"
#include <assert.h>

/* The service wait hint time */
const DWORD SERVICE_WAIT_HINT_TIME=10000; // 10 seconds

// Max time to start the service
const DWORD MAX_SERVICE_START_TIME=600000; // 10 minute, there are some slow machine out there and large projects ...


/* The name of the java start service method */
const LPCSTR START_SERVICE_METHOD_NAME = "startService";

/* The name of the java pause service method */
const LPCSTR PAUSE_SERVICE_METHOD_NAME = "pauseService";

/* The name of the java continue service method */
const LPCSTR CONTINUE_SERVICE_METHOD_NAME = "continueService";

/* The name of the java stop service method */
const LPCSTR STOP_SERVICE_METHOD_NAME = "stopService";

/* The name of the java shutdown service method */
const LPCSTR SHUTDOWN_SERVICE_METHOD_NAME = "shutdownService";

/* Initialize the static member pInstance */
CJavaWindowsService* CJavaWindowsService::m_pInstance = NULL;

/*
 * The jvm exit hook is call if System.exit() is called within java 
 */
void JNICALL jvmExitHook(jint code)
{
	CJavaWindowsService::exitWindowsService(code);
}

/*
 * The Windows service main function 
 */
void WINAPI startupWinService(DWORD argc, LPSTR* argv)
{
	CJavaWindowsService::startupWindowsService(argc, argv);
}

/*
 * The Windows service control handler function
 */
void WINAPI controlWinService (
		DWORD operationCode)
{
	CJavaWindowsService::controlWindowsService(operationCode);
}

/* 
 * Main function of the java startup thread 
 */
DWORD WINAPI javaStartupThread(void* pArg)
{
	CJavaWindowsService::startupJavaWindowsService();
	return 0;
}

/*
 * Console Control Handler 
 * Workaround for java bug 4323062.
 * This Bug should be fixed in JDK 1.3.1 which is not available yet
 */
BOOL __stdcall consoleControlHandler(DWORD dwMsg)
{
   switch (dwMsg)
   {
      case CTRL_LOGOFF_EVENT :	// Consume Log off event, so that JVM Console Control Hander
								// does not shutdown JVM
         return TRUE;
   }
   return FALSE;				// Other events can be processed by JVM Console Control Handler
}

CJavaWindowsService::CJavaWindowsService(CLaunchConfiguration& launchConfiguration)
: CJavaProgram(launchConfiguration), m_continueServiceMethod(NULL), m_hEvent(NULL),  
m_javaServiceClass(NULL), m_pauseServiceMethod(NULL), m_pJavaVirtualMaschine(NULL), 
m_pStartupException(NULL), m_shutdownServiceMethod(NULL), m_startServiceMethod(NULL), 
m_stopServiceMethod(NULL), m_hServiceStatus(NULL)
{
	memset(&m_serviceStatus, 0, sizeof(SERVICE_STATUS));
	assert(launchConfiguration.getWindowsServiceName() != NULL);
}

CJavaWindowsService::~CJavaWindowsService(void)
{
	if (m_pStartupException != NULL)
	{
		delete m_pStartupException;
	}
	if (m_pJavaVirtualMaschine != NULL)
	{
		delete m_pJavaVirtualMaschine;
	}
	if (m_hEvent != NULL)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	if (m_hServiceStatus != NULL)
	{
		CloseHandle(m_hServiceStatus);
		m_hServiceStatus = NULL;
	}
}

CJavaWindowsService* CJavaWindowsService::createJavaWindowsService(LPCSTR pcWindowsServiceName, LPCSTR pcMainJavaWindowsServiceClass)
{
	CLaunchConfiguration defaultLaunchConfiguration;

	assert(pcMainJavaWindowsServiceClass != NULL);
	assert(pcWindowsServiceName != NULL);
	defaultLaunchConfiguration.setMainJavaClass(pcMainJavaWindowsServiceClass);
	defaultLaunchConfiguration.setWindowsServiceName(pcWindowsServiceName);
	return new CJavaWindowsService(defaultLaunchConfiguration);
}

CJavaWindowsService* CJavaWindowsService::createJavaWindowsService()
{
	return new CJavaWindowsService(
		CLaunchConfigurationFile::getForExecutable().readLaunchConfiguration());
}

CJavaWindowsService* CJavaWindowsService::createJavaWindowsService(CLaunchConfigurationFile launchConfigurationFile)
{
	return new CJavaWindowsService(launchConfigurationFile.readLaunchConfiguration());
}

CJavaWindowsService* CJavaWindowsService::createJavaWindowsService(CLaunchConfiguration launchConfiguration)
{
	return new CJavaWindowsService(launchConfiguration);
}

CJavaWindowsService* CJavaWindowsService::createJavaWindowsService(CLaunchConfiguration defaultLaunchConfiguration, bool readLaunchConfigurationFileIfExists)
{
	if (readLaunchConfigurationFileIfExists)
	{
		CLaunchConfigurationFile launchConfigurationFile;

		launchConfigurationFile = CLaunchConfigurationFile::getForExecutable();
		if (launchConfigurationFile.exists())
		{
			CLaunchConfiguration launchConfigurationFromFile, launchConfiguration;

			launchConfigurationFromFile = launchConfigurationFile.readLaunchConfiguration();
			launchConfiguration = defaultLaunchConfiguration.overwrite(launchConfigurationFromFile);
			return createJavaWindowsService(launchConfiguration);
		}
		else
		{
			return createJavaWindowsService(defaultLaunchConfiguration);
		}
	}
	else
	{
		return createJavaWindowsService(defaultLaunchConfiguration);
	}
}

CJavaWindowsService* CJavaWindowsService::createJavaWindowsService(CLaunchConfiguration defaultLaunchConfiguration, CLaunchConfigurationFile launchConfigurationFile)
{
	CLaunchConfiguration launchConfiguration;

	launchConfiguration = defaultLaunchConfiguration.overwrite(launchConfigurationFile.readLaunchConfiguration());
	return createJavaWindowsService(launchConfiguration);
}

void CJavaWindowsService::main()
{
	CLog::debug("main start");
	SERVICE_TABLE_ENTRY   DispatchTable[] = 
    { 
		{ TEXT((LPSTR)m_launchConfiguration.getWindowsServiceName()), startupWinService      }, 
        { NULL,              NULL                } 
    }; 
 
	assert(m_pInstance == NULL);// there should be only one instance 
	m_pInstance = this;

    if (!StartServiceCtrlDispatcher( DispatchTable)) 
    { 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_START_SERVICE_DISPATCHER, GetLastError(),
			"Could not start service control dispatcher for service %s", m_launchConfiguration.getWindowsServiceName());
    } 
	CLog::debug("main end");
}

void CJavaWindowsService::main(int argc, LPSTR argv[])
{
	main();
}

void CJavaWindowsService::registerWindowsService(
			LPCSTR pcUserName,
			LPCSTR pcPassword,
			DWORD dwStartType,
			DWORD dwDesiredAccess,
			DWORD dwErrorControl)
{
	CWindowsService ntService(m_launchConfiguration.getWindowsServiceName());
	ntService.registerWindowsService(pcUserName, pcPassword, dwStartType, dwDesiredAccess, dwErrorControl);
}

void CJavaWindowsService::unregisterWindowsService()
{
	CWindowsService ntService(m_launchConfiguration.getWindowsServiceName());
	ntService.unregisterWindowsService();
}

void CJavaWindowsService::startWindowsService()
{
	CWindowsService ntService(m_launchConfiguration.getWindowsServiceName());
	ntService.startWindowsService();
}

void CJavaWindowsService::stopWindowsService()
{
	CWindowsService ntService(m_launchConfiguration.getWindowsServiceName());
	ntService.stopWindowsService();
}

void CJavaWindowsService::startupWindowsService(DWORD argc, LPSTR *argv)
{
	assert(m_pInstance != NULL);
	try
	{
		m_pInstance->startup(argc, argv);
	}
	catch(CLaunchException ex)
	{
		CLog::error(ex.getMessage());
	}
}

void CJavaWindowsService::controlWindowsService(DWORD operationCode)
{
	assert(m_pInstance != NULL);
	try
	{
		m_pInstance->control(operationCode);
	}
	catch(CLaunchException ex)
	{
		CLog::error(ex.getMessage());
	}
}

void CJavaWindowsService::control(DWORD operationCode)
{
	switch(operationCode) 
	{ 
		case SERVICE_CONTROL_PAUSE: 
			pause();
			break; 
		case SERVICE_CONTROL_CONTINUE: 
			continue_();
			break; 
		case SERVICE_CONTROL_STOP: 
			stop();
			break; 
		case SERVICE_CONTROL_SHUTDOWN:
			shutdown();
			break;; 
		case SERVICE_CONTROL_INTERROGATE: 
			// Fall through to send current status. 
			break; 
		default: 
			// ignore
			CLog::error("Unrecognized opcode %ld", operationCode); 
			break;
	} 
    // Send current status. 
    if (!SetServiceStatus (m_hServiceStatus,  &m_serviceStatus)) 
	{ 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_SET_SERVICE_STATUS, GetLastError(),
			"Could not set status of service %s", m_launchConfiguration.getWindowsServiceName());
	} 
};

void CJavaWindowsService::startup(DWORD argc, LPSTR* argv)
{
	CLog::info("Starting service %s", m_launchConfiguration.getWindowsServiceName()); 

    m_serviceStatus.dwServiceType        = SERVICE_WIN32; 
    m_serviceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    m_serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | 
        SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN; 
    m_serviceStatus.dwWin32ExitCode      = 0; 
    m_serviceStatus.dwServiceSpecificExitCode = 0; 
    m_serviceStatus.dwCheckPoint         = 0; 
    m_serviceStatus.dwWaitHint           = SERVICE_WAIT_HINT_TIME; 
 
    m_hServiceStatus = RegisterServiceCtrlHandler( 
        TEXT(m_launchConfiguration.getWindowsServiceName()), 
        controlWinService); 
 
    if (m_hServiceStatus == (SERVICE_STATUS_HANDLE)0) 
    { 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_REGISTER_SERVICE_CONTROL_HANDLER,
			GetLastError(), "Could	not register service control handler for service %s", m_launchConfiguration.getWindowsServiceName());
    } 

	// set status immediatly
	if (!SetServiceStatus (m_hServiceStatus, &m_serviceStatus)) 
    { 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_SET_SERVICE_STATUS,
			GetLastError(),
			"Could not set status of service %s", m_launchConfiguration.getWindowsServiceName());
    } 
 
    // Initialization code goes here. 
	try
	{
		initialize(argc, argv);
	}
	catch(CLaunchException ex)
	{
        m_serviceStatus.dwCurrentState       = SERVICE_STOPPED; 
        m_serviceStatus.dwCheckPoint         = 0; 
        m_serviceStatus.dwWaitHint           = 0; 
        m_serviceStatus.dwWin32ExitCode      = ERROR_SERVICE_SPECIFIC_ERROR; 
        m_serviceStatus.dwServiceSpecificExitCode = ex.getErrorCode(); 
 
        SetServiceStatus (m_hServiceStatus, &m_serviceStatus); 
		throw ex;
	}
	  
    // Initialization complete - report running status. 
    m_serviceStatus.dwCurrentState       = SERVICE_RUNNING; 
    m_serviceStatus.dwCheckPoint         = 0; 
    m_serviceStatus.dwWaitHint           = 0; 
 
    if (!SetServiceStatus (m_hServiceStatus, &m_serviceStatus)) 
    { 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_SET_SERVICE_STATUS, GetLastError(),
			"Could not set status of service %s", m_launchConfiguration.getWindowsServiceName());
    } 

	// Workaround for Java Bug 4323062 
	// This Bug should be fixed in JDK 1.3.1 which is not available yet
	// ---------------------------------------------------------------------------

	// Register console handler
	if (!SetConsoleCtrlHandler(&consoleControlHandler, TRUE))
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_SET_CONSOLE_CTRL_HANDLER, GetLastError(),
			"Could not set console ctrl handler of service %s", m_launchConfiguration.getWindowsServiceName());
	}

	// Workaround end
	// ---------------------------------------------------------------------------

	CLog::info("Service %s started", m_launchConfiguration.getWindowsServiceName()); 
    return; 
}

void CJavaWindowsService::pause()
{
	JNIEnv* pJavaNativeInterface;

	assert(m_pJavaVirtualMaschine != NULL);
	pJavaNativeInterface = m_pJavaVirtualMaschine->attachCurrentThread("Windows service control thread");

	CLog::info("Pausing service %s", m_launchConfiguration.getWindowsServiceName());

	/* Invoke pause method. */
	pJavaNativeInterface->CallStaticVoidMethod(m_javaServiceClass, m_pauseServiceMethod, NULL);
	if (pJavaNativeInterface->ExceptionCheck()) 
	{
		CLaunchException ex(JVMLauncherErrorCodes::EXCEPTION_IN_JAVA_SERVICE_CONTROL_METHOD,
			pJavaNativeInterface,
			"Java windows service method %s throws excpetion", PAUSE_SERVICE_METHOD_NAME);
		m_pJavaVirtualMaschine->detachCurrentThread();
		throw ex;
	}
	else
	{
		m_serviceStatus.dwCurrentState = SERVICE_PAUSED; 
		CLog::info("Service %s paused", m_launchConfiguration.getWindowsServiceName());
	}
	m_pJavaVirtualMaschine->detachCurrentThread();
}

void CJavaWindowsService:: continue_()
{
	JNIEnv* pJavaNativeInterface;

	assert(m_pJavaVirtualMaschine != NULL);
	pJavaNativeInterface = m_pJavaVirtualMaschine->attachCurrentThread("Windows service control thread");

	CLog::info("Continueing service %s", m_launchConfiguration.getWindowsServiceName());

	/* Invoke pause method. */
	pJavaNativeInterface->CallStaticVoidMethod(m_javaServiceClass, m_continueServiceMethod, NULL);
	if (pJavaNativeInterface->ExceptionCheck()) 
	{
		CLaunchException ex(JVMLauncherErrorCodes::EXCEPTION_IN_JAVA_SERVICE_CONTROL_METHOD,
			pJavaNativeInterface,
			"Java windows service method %s throws excpetion", CONTINUE_SERVICE_METHOD_NAME);
		m_pJavaVirtualMaschine->detachCurrentThread();
		throw ex;
	}
	else
	{
		m_serviceStatus.dwCurrentState = SERVICE_RUNNING; 
		CLog::info("Service %s continued", m_launchConfiguration.getWindowsServiceName());
	}
	m_pJavaVirtualMaschine->detachCurrentThread();
}
	
void CJavaWindowsService::stop()
{
	JNIEnv* pJavaNativeInterface;

	assert(m_pJavaVirtualMaschine != NULL);
	pJavaNativeInterface = m_pJavaVirtualMaschine->attachCurrentThread("Windows service control thread");

	CLog::info("Stopping service %s", m_launchConfiguration.getWindowsServiceName());

	/* Invoke pause method. */
	pJavaNativeInterface->CallStaticVoidMethod(m_javaServiceClass, m_stopServiceMethod, NULL);
	if (pJavaNativeInterface->ExceptionCheck()) 
	{
		CLaunchException ex(JVMLauncherErrorCodes::EXCEPTION_IN_JAVA_SERVICE_CONTROL_METHOD,
			pJavaNativeInterface,
			"Java windows service method %s throws excpetion", STOP_SERVICE_METHOD_NAME);
		m_pJavaVirtualMaschine->detachCurrentThread();
		throw ex;
	}
	else
	{
		m_serviceStatus.dwWin32ExitCode = 0; 
		m_serviceStatus.dwCurrentState  = SERVICE_STOP_PENDING; 
		m_serviceStatus.dwCheckPoint    = 0; 
		m_serviceStatus.dwWaitHint      = 0; 
		CLog::info("Service %s is stopping", m_launchConfiguration.getWindowsServiceName());
	}
	m_pJavaVirtualMaschine->detachCurrentThread();
}

void CJavaWindowsService::shutdown()
{
	JNIEnv* pJavaNativeInterface;

	assert(m_pJavaVirtualMaschine != NULL);
	pJavaNativeInterface = m_pJavaVirtualMaschine->attachCurrentThread("Windows service control thread");

	CLog::info("Shutdown service %s", m_launchConfiguration.getWindowsServiceName());

	/* Invoke pause method. */
	pJavaNativeInterface->CallStaticVoidMethod(m_javaServiceClass, m_shutdownServiceMethod, NULL);
	if (pJavaNativeInterface->ExceptionCheck()) 
	{
		CLaunchException ex(JVMLauncherErrorCodes::EXCEPTION_IN_JAVA_SERVICE_CONTROL_METHOD,
			pJavaNativeInterface,
			"Java windows service method %s throws excpetion", SHUTDOWN_SERVICE_METHOD_NAME);
		m_pJavaVirtualMaschine->detachCurrentThread();
		throw ex;
	}
	else
	{
		m_serviceStatus.dwWin32ExitCode = 0; 
		m_serviceStatus.dwCurrentState  = SERVICE_STOP_PENDING; 
		m_serviceStatus.dwCheckPoint    = 0; 
		m_serviceStatus.dwWaitHint      = 0; 
		CLog::info("Service %s is stopping", m_launchConfiguration.getWindowsServiceName());
	}
	m_pJavaVirtualMaschine->detachCurrentThread();
}

void CJavaWindowsService::initialize(DWORD argc, LPSTR* argv)
{
	HANDLE hThread;
	HANDLE handles[2];
	DWORD res = 0;
	DWORD serviceStartTime = 0;
	DWORD result=WAIT_TIMEOUT;
		
	m_hEvent = CreateEvent(0, true, false, 0);
	if (m_hEvent == 0)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CREATE_EVENT_FOR_JAVA_THREAD,
			GetLastError(), "Could not create event for service %s", m_launchConfiguration.getWindowsServiceName());
	}

	hThread = CreateThread(0, 10000, &javaStartupThread, 0, 0, 0);
	if (hThread == 0)
	{
		CloseHandle(m_hEvent);
		throw CLaunchException(JVMLauncherErrorCodes::COULD_NOT_CREATE_JAVA_SERVICE_STARTUP_THREAD,
			GetLastError(), "Could not create java startup thread for service %s", m_launchConfiguration.getWindowsServiceName());
	}

	handles[0] = m_hEvent;
	handles[1] = hThread;
	
	while ((result == WAIT_TIMEOUT)&&(serviceStartTime < MAX_SERVICE_START_TIME))
	{
		result = WaitForMultipleObjects(2, handles, false, SERVICE_WAIT_HINT_TIME/4);	
		serviceStartTime += SERVICE_WAIT_HINT_TIME/4;
		// Bug #2190 increment dwCheckPoint after a while, otherwise server manager stops the service during startup
		// if startup procedure last a long period.
		m_serviceStatus.dwCheckPoint++;
	}
	if (result == WAIT_TIMEOUT)
	{
		CloseHandle(hThread);
		CloseHandle(m_hEvent);
		throw CLaunchException(JVMLauncherErrorCodes::TIMEOUT_JAVA_SERVICE_STARTUP_THREAD,
			"Timeout while starting java windows service %s", m_launchConfiguration.getWindowsServiceName());
	}
	
	if (m_pStartupException!=NULL)
	{
		CloseHandle(hThread);
		CloseHandle(m_hEvent);
		throw *m_pStartupException;
	}
	CloseHandle(hThread);
	CloseHandle(m_hEvent);
}

void CJavaWindowsService::start()
{
	JNIEnv* pJavaNativeInterface;

	pJavaNativeInterface = m_pJavaVirtualMaschine->getJavaNativeInterface();
	    
	CLog::info("Starting service %s", m_launchConfiguration.getWindowsServiceName());

    /* Invoke start method. */
    pJavaNativeInterface->CallStaticVoidMethod(m_javaServiceClass, m_startServiceMethod, NULL);
    if (pJavaNativeInterface->ExceptionCheck()) {
		throw CLaunchException(JVMLauncherErrorCodes.EXCEPTION_IN_JAVA_START_SERVICE_METHOD,
			pJavaNativeInterface,
			"Exception in java windows service start method");
    }
	CLog::info("Service %s started", m_launchConfiguration.getWindowsServiceName());
}

void CJavaWindowsService::initializeJavaWindowsService()
{
	JNIEnv* pJavaNativeInterface;

	pJavaNativeInterface = m_pJavaVirtualMaschine->getJavaNativeInterface();

	/* Gets the java windows service class */
	m_javaServiceClass = getJavaClass(pJavaNativeInterface, m_launchConfiguration.getMainJavaClass());
    
	/* Get the java windows service start method */
	m_startServiceMethod = getJavaStaticMethod(pJavaNativeInterface, m_javaServiceClass, START_SERVICE_METHOD_NAME, "()V");

	/* Get the java windows service stop method */
    m_stopServiceMethod = getJavaStaticMethod(pJavaNativeInterface, m_javaServiceClass, STOP_SERVICE_METHOD_NAME, "()V");

	/* Get the java windows service pause method */
    m_pauseServiceMethod = getJavaStaticMethod(pJavaNativeInterface, m_javaServiceClass, PAUSE_SERVICE_METHOD_NAME, "()V");

	/* Get the java windows service continue method */
    m_continueServiceMethod = getJavaStaticMethod(pJavaNativeInterface, m_javaServiceClass, CONTINUE_SERVICE_METHOD_NAME, "()V");

	/* Get the java windows service shutdown method */
    m_shutdownServiceMethod = getJavaStaticMethod(pJavaNativeInterface, m_javaServiceClass, SHUTDOWN_SERVICE_METHOD_NAME, "()V");
}

void CJavaWindowsService::startupJava()
{    
	CVmOptions vmOptions;
	char pcApplicationDirectory[MAX_PATH];
	char pcJvmPath[MAX_PATH];

	CLog::info("=============================================================");
	CLog::info("Starting java windows service in class");
	CLog::info("%s", m_launchConfiguration.getMainJavaClass());
	CLog::info("with ivyteam's java windows service launcher"); 
	CLog::info("=============================================================");

	try
	{
		getApplicationDirectory(pcApplicationDirectory, MAX_PATH);
		
		CLog::info("Setting current directory to '%s'.", pcApplicationDirectory);
		/* Set the current directory */
		SetCurrentDirectory(pcApplicationDirectory);

		// initialize vm options
		initializeVmOptions(vmOptions, pcApplicationDirectory);
		addJvmExitHook(vmOptions);

		// create java virtual maschine
		m_pJavaVirtualMaschine = CJavaVirtualMaschine::createJavaVirtualMaschine(
			getJvmPath(pcApplicationDirectory, pcJvmPath, MAX_PATH), 
			vmOptions);

		// initialize java windows service
		initializeJavaWindowsService();

		// start java window service
		start();
	}
	catch(CLaunchException ex)
	{
		m_pStartupException = new CLaunchException(ex);
		SetEvent(m_hEvent);
		return;
	}

	// inform service thread that service was started
	SetEvent(m_hEvent);

	CLog::debug("Service start method returned");

	// if start returns, detach current thread, so that threads waiting for the start thread to die are notified
    m_pJavaVirtualMaschine->detachCurrentThread();

	CLog::debug("Wait until jvm has ended and is destroyed");
	m_pJavaVirtualMaschine->waitForAndDestroy();
	CLog::debug("Jvm ended and destroyed");

	// ReW 28.7.2004 See Bug 1786
	CLog::debug("Set service state STOPPED");
    
	m_serviceStatus.dwCurrentState       = SERVICE_STOPPED; 
    m_serviceStatus.dwCheckPoint         = 0; 
    m_serviceStatus.dwWaitHint           = 0; 
    m_serviceStatus.dwWin32ExitCode      = NO_ERROR; 
    m_serviceStatus.dwServiceSpecificExitCode = 0; 

    SetServiceStatus (m_hServiceStatus, &m_serviceStatus); 

	CLog::info("Service %s stopped", m_launchConfiguration.getWindowsServiceName());

	// normally the main method should return and process is terminated
	Sleep(60*1000);

	CLog::debug("Service main does not return, call exit(...) explicit");
	// if not call exit
	::exit(0);
}

void CJavaWindowsService::addJvmExitHook(CVmOptions& options)
{
	options.addOption("exit" , jvmExitHook);
}

void CJavaWindowsService::startupJavaWindowsService()
{
	assert(m_pInstance != NULL);
	m_pInstance->startupJava();
}

void CJavaWindowsService::exitWindowsService(jint javaExitCode)
{
	assert(m_pInstance != NULL);
	m_pInstance->exit(javaExitCode);
}

void CJavaWindowsService::exit(jint javaExitCode)
{
	CLog::debug("Set service state STOPPED");
    
	// set service status to STOPPED
	m_serviceStatus.dwCurrentState       = SERVICE_STOPPED; 
    m_serviceStatus.dwCheckPoint         = 0; 
    m_serviceStatus.dwWaitHint           = 0; 
    m_serviceStatus.dwWin32ExitCode      = NO_ERROR; 
    m_serviceStatus.dwServiceSpecificExitCode = javaExitCode; 

    SetServiceStatus (m_hServiceStatus, &m_serviceStatus); 

	CLog::info("Service %s stopped", m_launchConfiguration.getWindowsServiceName());

	// normally the main method should return and process is terminated
	Sleep(60*1000);

	CLog::debug("Service main does not return, call exit(...) explicit");
	// if not call exit
	::exit(javaExitCode);
}



