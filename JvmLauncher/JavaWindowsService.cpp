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
const LPCSTR MAIN_SERVICE_METHOD_NAME = "main";

/* The name of the java pause service method */
const LPCSTR CONTROL_SERVICE_METHOD_NAME = "control";

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
: CJavaProgram(launchConfiguration), m_hEvent(NULL),  
m_javaServiceClass(NULL), m_controlServiceMethod(NULL), m_pJavaVirtualMaschine(NULL), 
m_pStartupException(NULL), m_mainServiceMethod(NULL), m_hServiceStatus(NULL), m_argc(0), m_argv(NULL)
{
	memset(&m_serviceStatus, 0, sizeof(SERVICE_STATUS));
	assert(launchConfiguration.getWindowsServiceName() != NULL);

	m_serviceStatus.dwServiceType        = SERVICE_WIN32; 
    m_serviceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    m_serviceStatus.dwControlsAccepted   = 0;
    m_serviceStatus.dwWin32ExitCode      = 0; 
    m_serviceStatus.dwServiceSpecificExitCode = 0; 
    m_serviceStatus.dwCheckPoint         = 0; 
    m_serviceStatus.dwWaitHint           = SERVICE_WAIT_HINT_TIME; 
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
	if (m_hJavaThread != NULL)
	{
		CloseHandle(m_hJavaThread);
		m_hJavaThread = NULL;
	}
	if (m_hServiceStatus != NULL)
	{
		// service status must not be close! See documentation of RegisterServiceControlHandle
		m_hServiceStatus = NULL;
	}
	if (m_argv != NULL)
	{
		for (DWORD pos = 0; pos < m_argc; pos++)
		{
			delete m_argv[pos];
		}
		delete m_argv;
	}
}

CJavaWindowsService* CJavaWindowsService::createJavaWindowsService(CLaunchConfiguration launchConfiguration)
{
	return new CJavaWindowsService(launchConfiguration);
}


void CJavaWindowsService::main()
{
	CLog::debug("CJavaWindowsService::main() entry");

	SERVICE_TABLE_ENTRY   DispatchTable[] = 
    { 
		{ TEXT((LPSTR)m_launchConfiguration.getWindowsServiceName()), startupWinService      }, 
        { NULL,              NULL                } 
    }; 
 
	assert(m_pInstance == NULL);// there should be only one instance 
	m_pInstance = this;

	CLog::info("StartServiceCtrlDispatcher entry");
    if (!StartServiceCtrlDispatcher( DispatchTable)) 
    { 
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes.COULD_NOT_START_SERVICE_DISPATCHER, GetLastError(),
			"Could not start service control dispatcher for service %s", m_launchConfiguration.getWindowsServiceName());
		CLog::error("CJavaWindowsService::main() throw (%s)", ex.getMessage());
		throw ex;
    } 
	CLog::info("Wait till java thread ends or system exit is called.");
	WaitForSingleObject(m_hJavaThread, 30000); // Wait till java is finished or system exit
	CLog::debug("CJavaWindowsService::main() leave");
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
		m_pInstance->reportError(ex);
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
		m_pInstance->reportError(ex);
	}
}

void CJavaWindowsService::control(DWORD operationCode)
{
	JNIEnv* pJavaNativeInterface;
	CLog::debug("CJavaWindowsService::control(%d) entry", operationCode);
	if (m_javaService != NULL) // java service ready?
	{
		// yes -> forward control request to java service
		assert(m_pJavaVirtualMaschine != NULL);
		pJavaNativeInterface = m_pJavaVirtualMaschine->attachCurrentThread("Windows service control thread");

		CLog::info("Call method control(%d) of java windows service class %s", operationCode, m_launchConfiguration.getMainJavaClass());
		/* Invoke control method. */
		pJavaNativeInterface->CallVoidMethod(m_javaService, m_controlServiceMethod, operationCode);
		if (pJavaNativeInterface->ExceptionCheck()) 
		{
			CLaunchException ex(JVMLauncherErrorCodes::EXCEPTION_IN_JAVA_SERVICE_CONTROL_METHOD,
				pJavaNativeInterface,
				"Java windows service method %s throws exception", CONTROL_SERVICE_METHOD_NAME);
			m_pJavaVirtualMaschine->detachCurrentThread();
			
			CLog::error("CJavaWindowsService::control(...) throw %s", ex.getMessage());	
			throw ex;
		}
		m_pJavaVirtualMaschine->detachCurrentThread();
	}
	else
	{
		// no -> set at least the service state
		SetServiceStatus(m_hServiceStatus, &m_serviceStatus);
	}
	CLog::debug("CJavaWindowsService::control() leave");	
};

void CJavaWindowsService::startup(DWORD argc, LPSTR* argv)
{
	CLog::debug("CJavaWindowsService::startup(...) entry"); 

	m_argc = argc;
	if (argv != NULL)
	{
		m_argv = new LPSTR[argc];
		for (DWORD pos = 0; pos < argc; pos++)
		{
			m_argv[pos] = new char[strlen(argv[pos])+1];
			strcpy_s(m_argv[pos], strlen(argv[pos])+1, argv[pos]);
		}
	}
	else
	{
		m_argv = NULL;
	}
 
    m_hServiceStatus = RegisterServiceCtrlHandler( 
        TEXT(m_launchConfiguration.getWindowsServiceName()), 
        controlWinService); 
 
    if (m_hServiceStatus == (SERVICE_STATUS_HANDLE)0) 
    { 
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes.COULD_NOT_REGISTER_SERVICE_CONTROL_HANDLER,
			GetLastError(), "Could	not register service control handler for service %s", m_launchConfiguration.getWindowsServiceName());
		CLog::error("CJavaWindowsService::startup(...) throw (%s)", ex.getMessage()); 
		throw ex;
    } 

	reportStartProgress();
    // Initialization code goes here. 
	try
	{
		initialize(argc, argv);
	}
	catch(CLaunchException ex)
	{
		reportError(ex);
		CLog::error("Set service status to STOPPED because initialization has been failed with error '%s'", ex.getMessage());
		reportStop(ex);
		CLog::debug("CJavaWindowsService::startup(...) leave"); 
		return;
	}
	  
	// Workaround for Java Bug 4323062 
	// This Bug should be fixed in JDK 1.3.1 which is not available yet
	// ---------------------------------------------------------------------------

	// Register console handler
	if (!SetConsoleCtrlHandler(&consoleControlHandler, TRUE))
	{
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes.COULD_NOT_SET_CONSOLE_CTRL_HANDLER, GetLastError(),
			"Could not set console ctrl handler of service %s", m_launchConfiguration.getWindowsServiceName());
		CLog::error("CJavaWindowsService::startup(...) throw (%s)", ex.getMessage()); 
		throw ex;
	}

	// Workaround end
	// ---------------------------------------------------------------------------

	CLog::debug("CJavaWindowsService::startup(...) leave"); 
	return; 
}

void CJavaWindowsService::reportStartProgress()
{
	m_serviceStatus.dwCheckPoint++;

	// set status immediatly
	if (!SetServiceStatus (m_hServiceStatus, &m_serviceStatus)) 
    { 
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_SET_SERVICE_STATUS,
			GetLastError(),
			"Could not set status of service %s", m_launchConfiguration.getWindowsServiceName());
    } 
}

void CJavaWindowsService::reportStop(CLaunchException ex)
{
    m_serviceStatus.dwCurrentState       = SERVICE_STOPPED; 
    m_serviceStatus.dwCheckPoint         = 0; 
    m_serviceStatus.dwWaitHint           = 0; 
    m_serviceStatus.dwWin32ExitCode      = ERROR_SERVICE_SPECIFIC_ERROR; 
    m_serviceStatus.dwServiceSpecificExitCode = ex.getErrorCode(); 

	
    SetServiceStatus (m_hServiceStatus, &m_serviceStatus); 
}

void CJavaWindowsService::initialize(DWORD argc, LPSTR* argv)
{
	HANDLE handles[2];
	DWORD res = 0;
	DWORD serviceStartTime = 0;
	DWORD result=WAIT_TIMEOUT;
		
	CLog::debug("CJavaWindowsService::initialize(...) entry");
	m_hEvent = CreateEvent(0, true, false, 0);
	if (m_hEvent == 0)
	{
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CREATE_EVENT_FOR_JAVA_THREAD,
			GetLastError(), "Could not create event for service %s", m_launchConfiguration.getWindowsServiceName());
		CLog::error("CJavaWindowsService::initialize(...) throw (%s)", m_pStartupException->getMessage());
		throw ex;
	}

	m_hJavaThread = CreateThread(0, 10000, &javaStartupThread, 0, 0, 0);
	if (m_hJavaThread == 0)
	{
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes::COULD_NOT_CREATE_JAVA_SERVICE_STARTUP_THREAD,
			GetLastError(), "Could not create java startup thread for service %s", m_launchConfiguration.getWindowsServiceName());
		CLog::error("CJavaWindowsService::initialize(...) throw (%s)", m_pStartupException->getMessage());
		throw ex;
	}

	handles[0] = m_hEvent;
	handles[1] = m_hJavaThread;
	
	while ((result == WAIT_TIMEOUT)&&(serviceStartTime < MAX_SERVICE_START_TIME))
	{
		result = WaitForMultipleObjects(2, handles, false, SERVICE_WAIT_HINT_TIME/4);	
		serviceStartTime += SERVICE_WAIT_HINT_TIME/4;
		// Bug #2190 increment dwCheckPoint after a while, otherwise server manager stops the service during startup
		// if startup procedure last a long period.
		reportStartProgress();
	}
	if (result == WAIT_TIMEOUT)
	{
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes::TIMEOUT_JAVA_SERVICE_STARTUP_THREAD,
			"Timeout while starting java windows service %s", m_launchConfiguration.getWindowsServiceName());
		CLog::error("CJavaWindowsService::initialize(...) throw (%s)", m_pStartupException->getMessage());
		throw ex;
	}
	if (result == (WAIT_OBJECT_0+1))
	{
		// thread has died
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes::TIMEOUT_JAVA_SERVICE_STARTUP_THREAD,
			"Java Service Startup Thread has died unexpected while starting java windows service %s", m_launchConfiguration.getWindowsServiceName());
		CLog::error("CJavaWindowsService::initialize(...) throw (%s)", m_pStartupException->getMessage());
		throw ex;
	}
	
	if (m_pStartupException!=NULL)
	{
		CLog::error("CJavaWindowsService::initialize(...) throw (%s)", m_pStartupException->getMessage());
		throw *m_pStartupException;
	}
	CLog::debug("CJavaWindowsService::initialize(...) leave");
}

void CJavaWindowsService::callJavaMain()
{
	JNIEnv* pJavaNativeInterface;
	jobjectArray mainArguments;

	CLog::debug("CJavaWindowsService::callJavaMain() entry");
	pJavaNativeInterface = m_pJavaVirtualMaschine->getJavaNativeInterface();
	    
	CLog::info("Calling main(...) method of java windows service class %s", m_launchConfiguration.getMainJavaClass());

	/* The main arguments */
	mainArguments = convert2JavaStringArray(pJavaNativeInterface, m_argc, m_argv);
    if (pJavaNativeInterface->ExceptionCheck()) 
	{
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CONVERT_PARAMS,
			pJavaNativeInterface,
			"Could not convert command line arguments");
		CLog::error("CJavaWindowsService::callJavaMain() throw (%s)", ex.getMessage());
		throw ex;
    }

    /* Invoke start method. */
    pJavaNativeInterface->CallVoidMethod(m_javaService, m_mainServiceMethod, mainArguments);
    if (pJavaNativeInterface->ExceptionCheck()) 
	{
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes.EXCEPTION_IN_JAVA_MAIN_SERVICE_METHOD,
			pJavaNativeInterface,
			"Exception in java windows service main method");
		CLog::error("CJavaWindowsService::callJavaMain() throw (%s)", ex.getMessage());
		throw ex;
    }
	CLog::debug("CJavaWindowsService::callJavaMain() leave");
}

void CJavaWindowsService::initializeJavaWindowsService()
{
	JNIEnv* pJavaNativeInterface;

	CLog::debug("CJavaWindowsService::initializeJavaWindowsService() entry");
	pJavaNativeInterface = m_pJavaVirtualMaschine->getJavaNativeInterface();

	/* Gets the java windows service class */
	m_javaServiceClass = getJavaClass(pJavaNativeInterface, m_launchConfiguration.getMainJavaClass());
    
	/* Get the java windows service start method */
	m_mainServiceMethod = getJavaMethod(pJavaNativeInterface, m_javaServiceClass, MAIN_SERVICE_METHOD_NAME, "([Ljava/lang/String;)V");

	/* Get the java windows service stop method */
    m_controlServiceMethod = getJavaMethod(pJavaNativeInterface, m_javaServiceClass, CONTROL_SERVICE_METHOD_NAME, "(I)V");

	/* Get the java windows service constructor */
	m_serviceConstructor = getJavaMethod(pJavaNativeInterface, m_javaServiceClass, "<init>", "(J)V");

	/* Create java windows service object and call constructor */
	CLog::info("Windows Service Handle %d", m_hServiceStatus);
	m_javaService = pJavaNativeInterface->NewObject(m_javaServiceClass, m_serviceConstructor, (jlong)m_hServiceStatus);
	if (m_javaService == NULL)
	{
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CREATE_JAVA_SERVICE_OBJECT,
			pJavaNativeInterface,
			"Could not create java windows service object of class %s", m_launchConfiguration.getMainJavaClass());
		CLog::error("CJavaWindowsService::initializeJavaWindowsService() throw (%s)", ex.getMessage());
		throw ex;
	}
	/* Get a global reference to the java windows service object, so that I can access it from other threads */
	m_javaService = pJavaNativeInterface->NewGlobalRef(m_javaService);
	if (m_javaService == NULL)
	{
		CLaunchException ex = CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CREATE_JAVA_SERVICE_OBJECT,
			pJavaNativeInterface,
			"Could not create global reference of java windows service object");
		CLog::error("CJavaWindowsService::initializeJavaWindowsService() throw (%s)", ex.getMessage());
		throw ex;
	}
	CLog::debug("CJavaWindowsService::initializeJavaWindowsService() leave");
}

void CJavaWindowsService::startupJava()
{    
	CVmOptions vmOptions;
	char pcApplicationDirectory[MAX_PATH];
	char pcJvmPath[MAX_PATH];

	CLog::debug("CJavaWindowsService::startupJava() entry");
	CLog::info("=============================================================");
	CLog::info("Starting java windows service in class");
	CLog::info("%s", m_launchConfiguration.getMainJavaClass());
	CLog::info("=============================================================");

	try
	{
		reportStartProgress();

		getApplicationDirectory(pcApplicationDirectory, MAX_PATH);
		
		CLog::info("Setting current directory to '%s'.", pcApplicationDirectory);
		/* Set the current directory */
		SetCurrentDirectory(pcApplicationDirectory);

		reportStartProgress();

		JavaMainArguments javaMainArguments;
		initializeVmOptions(vmOptions, pcApplicationDirectory, javaMainArguments, false, false);
		//addJvmExitHook(vmOptions);

		reportStartProgress();

		// create java virtual maschine
		m_pJavaVirtualMaschine = CJavaVirtualMaschine::createJavaVirtualMaschine(
			getJvmPath(pcApplicationDirectory, pcJvmPath, MAX_PATH), 
			vmOptions);

		reportStartProgress();

		// initialize java windows service
		initializeJavaWindowsService();
		
		reportStartProgress();

		// start java window service
		callJavaMain();
	}
	catch(CLaunchException ex)
	{
		m_pStartupException = new CLaunchException(ex);
		SetEvent(m_hEvent);
		CLog::debug("CJavaWindowsService::startupJava() leave");
		return;
	}

	// inform service thread that service was started
	SetEvent(m_hEvent);

	// if start returns, detach current thread, so that threads waiting for the start thread to die are notified
    m_pJavaVirtualMaschine->detachCurrentThread();

	CLog::info("Wait until jvm has been ended and destroyed");
	m_pJavaVirtualMaschine->waitForAndDestroy();
	CLog::info("Jvm was ended and destroyed");
	CLog::debug("CJavaWindowsService::startupJava() leave");
}

void CJavaWindowsService::addJvmExitHook(CVmOptions& options)
{
	options.addOption("exit" , jvmExitHook);
}

void CJavaWindowsService::startupJavaWindowsService()
{
	assert(m_pInstance != NULL);
	try
	{
		m_pInstance->startupJava();
	}
	catch(CLaunchException ex)
	{
		CLog::error(ex.getMessage());
		m_pInstance->reportError(ex);
	}
}

void CJavaWindowsService::exitWindowsService(jint javaExitCode)
{
	assert(m_pInstance != NULL);
	m_pInstance->exit(javaExitCode);
}

void CJavaWindowsService::exit(jint javaExitCode)
{
	CLog::info("CJavaWindowsService::exit(%d)", javaExitCode);
}



