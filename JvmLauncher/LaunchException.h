/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

Launch exception

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

#ifndef __LAUNCH_EXCEPTION_H__
#define __LAUNCH_EXCEPTION_H__

#include <jni.h>
#include <windows.h>

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

/*
 * JVMLauncher error codes
 */
typedef struct{
enum{
	SUCCESSFUL								= 0,
	COULD_NOT_LOAD_JVM						= 1,
	COULD_NOT_INIT_JVM						= 2,
	ALREADY_CREATED_JVM						= 3,
	COULD_NOT_LOAD_JAVA_CLASS				= 4,
	COULD_NOT_FIND_METHOD					= 5,
	JVM_WAS_NOT_CREATED						= 6,
	COULD_NOT_CONVERT_PARAMS				= 7,
	EXCEPTION_IN_MAIN_METHOD				= 8,
	COULD_NOT_START_SERVICE_DISPATCHER		= 9,
	COULD_NOT_OPEN_SERVICE_CTRL_MANAGER		= 10,
	COULD_NOT_CREATE_SERVICE				= 11,
	COULD_NOT_OPEN_SERVICE					= 12,
	COULD_NOT_DELETE_SERVICE				= 13,
	COULD_NOT_START_SERVICE					= 14,
	COULD_NOT_STOP_SERVICE					= 15,
	COULD_NOT_QUERY_SERVICE_STATUS			= 16,
	ERROR_SERVICE_HAS_NOT_STARTED			= 17, 
	ERROR_SERVICE_HAS_NOT_STOPPED			= 18,
	COULD_NOT_CREATE_JAVA_SERVICE_OBJECT	= 19,
	EXCEPTION_IN_JAVA_MAIN_SERVICE_METHOD	= 20,
	COULD_NOT_CREATE_EVENT_FOR_JAVA_THREAD	= 21,
	COULD_NOT_CREATE_JAVA_SERVICE_STARTUP_THREAD = 22,
	TIMEOUT_JAVA_SERVICE_STARTUP_THREAD		= 23,
	WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT  = 24,
	COULD_NOT_DESTROY_JVM					= 25,
	COULD_NOT_DETACH_THREAD_FROM_JVM		= 26,
	COULD_NOT_ATTACH_THREAD_TO_JVM		    = 27,
	COULD_NOT_EVALUATE_APPLICATION_DIRECTORY= 28,
	COULD_NOT_REGISTER_SERVICE_CONTROL_HANDLER=29,
	COULD_NOT_SET_SERVICE_STATUS=30,
	COULD_NOT_SET_CONSOLE_CTRL_HANDLER=31,
	EXCEPTION_IN_JAVA_SERVICE_CONTROL_METHOD=32,
	COULD_NOT_ENUMERATE_WINDOWS=33,
	COULD_NOT_MAP_VIEW_OF_VIEW=34,
	COULD_NOT_CREATE_FILE_MAPPING=35,
	COULD_NOT_FIND_JRE=36
} codes;
} JVMLauncherErrorCodes;

/*
 An exception that is thrown if launch fails 
 */
class JVMLauncherExport CLaunchException  
{
private:
	/* The exception message */
	LPSTR m_pcMessage;
	/* The error code */
	DWORD m_dwErrorCode;
	/* The windows error code */
	DWORD m_dwWindowsErrorCode;

	/*
	 * Gets the message and stack trace of a java exception
	 * @param pJavaNativeInterface the java native interface
	 * @param javaException the java exception
	 * @return pointer to a string holding message and stack trace. use delete on returned pointer after use.
	 */
	LPSTR getJavaExceptionMessageAndStackTrace(JNIEnv* pJavaNativeInterface, jthrowable javaException);
public:
	/*
	 * Constructor
	 * @param dwErrorCode the error code of the exception 
	 * @param pcMessage the exception message. May hold format placeholders
	 */
	CLaunchException(DWORD dwErrorCode, LPCSTR pcMessage, ...);

	/*
	 * Constructor
	 * @param dwErrorCode the error code of the exception 
	 * @param dwWindowsErrorCode the windows error code 
	 * @param pcMessage the exception message. May hold format placeholders
	 */
	CLaunchException(DWORD dwErrorCode, DWORD dwWindowsErrorCode, LPCSTR pcMessage, ...);

	/*
	 * Copy constructor 
	 * @param the object to copy 
	 */
	CLaunchException(const CLaunchException& copy);

	/*
	 * Constructor
	 * @param dwErrorCode the error code of the exception
	 * @param pJavaNativeInterface the java native interface. If there is 
	 *		  a java exception occured the message of the java exception will be added
	 *        to pcMessage
	 * @param pcMessage the exception message. My hold format placeholders
	 */
	CLaunchException(DWORD dwErrorCode, JNIEnv* pJavaNativeInterface, LPCSTR pcMessage, ...);


	/*
	 * Destructor 
	 */
	virtual ~CLaunchException();

	/*
	 * Gets the formated message of the exception 
     * @return exception message
	 */
	LPSTR getMessage();

	/* 
	 * Gets the error code 
	 * @return error code
	 */
	DWORD getErrorCode();

	/*
	 * Gets the windows error code. Return 0 if no windows error code is defined
	 * @return windows error code 
	 */
	DWORD getWindowsErrorCode();
};

#endif 
