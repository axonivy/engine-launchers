/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

A java Windows service. This is an Windows service that calls certain static java methods of a java class
if some eveWindowss on the Windows service happends. 

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
#ifndef __JAVA_NT_SERVICE_H__
#define __JAVA_NT_SERVICE_H__

#include "JavaProgram.h"
#include "LaunchException.h"

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

class JVMLauncherExport CJavaWindowsService : public CJavaProgram
{
private:
	/*  The java virtual maschine to use */
	CJavaVirtualMaschine* m_pJavaVirtualMaschine;

	/* The one and only java windows service in this process */
	static CJavaWindowsService*  m_pInstance;

	/* The status of the windows service */
	SERVICE_STATUS			m_serviceStatus;		

	/* The service status handle */
	SERVICE_STATUS_HANDLE   m_hServiceStatus;		

	/* The java class which represents the windows service */
	jclass					m_javaServiceClass;	

	/** The java windows service object */
	jobject				    m_javaService;

	/* The main java method of the windows service */
	jmethodID				m_mainServiceMethod;

	/* The control java method of the windows service */
	jmethodID				m_controlServiceMethod;	

	/** The constructor of the service */
	jmethodID				m_serviceConstructor;

	/* Handle to the event used to signal that java startup thread has startuped the java windows service */
	HANDLE					m_hEvent;

	/* Handle to the java startup thread */
	HANDLE					m_hJavaThread;

	/* The exception thrown if java startup thread fails */
	CLaunchException*		m_pStartupException;

	/* The number of arguments */
	DWORD m_argc;

	/** The arguments */
	LPSTR* m_argv;

	LPSTR m_windowsServiceName;

	/* 
	 * This method is called by the static startupWindowsService method
	 * @param argc number of arguments
	 * @param argv arguments
     * @throws CLaunchException if an error occurs
	 */
	void startup(DWORD argc, LPSTR* argv);

	/*
	 * This method is called by the static startupJavaWindowsService method. It startups the java virtual maschine and 
	 * calls the startService method of the java service class
	 */
	void startupJava();

	/*
	 * Initializes the java windows service 
	 * @throws CLaunchException if java service class can not be loaded or java service control method are not found
	 */
	void initializeJavaWindowsService();

	/*
	 * Initializes the service. This method starts the java main thread and then waits until the java main 
	 * thread has launched the java virtual maschine and has invoked the startService method of the java service class
	 */
	void initialize(DWORD argc, LPSTR* argv);

	/*
	 * Adds the exit hook to the vm options
	 * @param options the vm options
	 */
	void addJvmExitHook(CVmOptions& options);

	/*
	 * This method is called by the static controlWindowsService method
	 * @param operationCode the code of the operation the windows service should do
	 * @throws CLaunchException if an error occurs
	 */
	void control(DWORD operationCode);

	/*
	 * Calls the main method in the java windows service class
	 */
	void callJavaMain();

	/*
	 * This method is called by the static exitWindowsService method
	 * @param javaExitCode the java exit code
	 */
	void exit(jint javaExitCode);

	/*
	 * Reports a start progress 
	 */
	void reportStartProgress();

	/*
	 * Reports a start error 
	 * @param ex the launch exception to report
	 */
	void reportStop(CLaunchException ex);	
public:

	CJavaWindowsService(LPSTR windowsServiceName, CLaunchConfiguration& launchConfiguration);
	
	virtual ~CJavaWindowsService(void);

	static CJavaWindowsService* createJavaWindowsService(LPSTR windowsServiceName, CLaunchConfiguration launchConfiguration);

	/*
	 * The main method of the Windows service. This function must be called if the windows starts the windows service.
	 * The method installs then a service control manager and exits. After that windows calls the startup method to startup the service.
	 * Note you can use the startWindowsService method to told the system to start the service.
	 * @throws CLaunchException if the Windows service can not be started.
	 */
	void main();

	/*
	 * Calls the main method above argc and argv are ignored
	 * @param argc the number of argumeWindowss passed to the java program
	 * @param argv the argumeWindowss passed to the java program
	 */
	void main(int argc, LPSTR argv[]);

	/*
	 * Registers the NT Service
	 * @param pcUserName Name of user as this service should iWindowseract as (NULL means SystemUser)
	 * @param pcPassword Password of service user (can be NULL for SystemUser)
	 * @param dwStartType How is the service started. See CreateService of Win32 documeWindowsation for details
	 * @param dwDesiredAccess What access rights has the service. See CreateService of Win32 documeWindowsation for details
	 * @param dwErrorCoWindowsrol What happens if the service can not started. See CreateService of Win32 documeWindowsation for details
	 * @throws CLaunchException if NT Service cannot be registered
	 */
	void registerWindowsService(
			LPCSTR pcUserName = NULL,
			LPCSTR pcPassword = NULL,
			DWORD dwStartType = SERVICE_AUTO_START,
			DWORD dwDesiredAccess = SERVICE_ALL_ACCESS,
			DWORD dwErrorCoWindowsrol = SERVICE_ERROR_NORMAL);

	/*
	 * Unregisters the NT Service
	 * @throws CLaunchException if NT service cannot be unregistered
	 */
	void unregisterWindowsService();

	/*
	 * Starts the NT Service. Note this function simply told windows to start the service. 
	 * It does not implemeWindows the service itself. To implemeWindows the service one has to call the main method of this class
	 * @throws CLaunchException if NT service cannot be started
	 */
	void startWindowsService();

	/*
	 * Stops the NT Service
	 * @throws CLaunchException if NT service cannot be started
	 */
	void stopWindowsService();

	/* 
	 * This method is called by the system to start the windows service. Do not call this method otherwise.
 	 * @param argc number of arguments
	 * @param argv arguments
	*/
	static void startupWindowsService(DWORD argc, LPSTR *argv);

	/*
	 * This method is called by the system to control the windows service. Do not call this method otherwise.
	 * @param operationCode the code of the operation the windows service should do
	 */
	static void controlWindowsService(DWORD operationCode);

	/*
	 * This method is called by the java startup thread. Do not call this method otherwise. 
	 * The method startups the java virtual maschine and calls the startService method of the java service class.
	 */
	static void startupJavaWindowsService();

	/*
	 * This method is called by the jvm exit hook (jvmExitHook). Do not call this method otherwise.
	 * The method set the status of the windows service to stop and sets the exit code. Then it calls exit()
	 * to stop the windows service process 
	 */
	static void exitWindowsService(jint javaExitCode);
};

#endif
