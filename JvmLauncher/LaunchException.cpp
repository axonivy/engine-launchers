/*-----------------------------------------------------------------------------------

   JavaVirtual Maschine Launcher

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

#define JVMLauncher

#include "LaunchException.h"
#include "Log.h"
#include <assert.h>
#include <stdio.h>

CLaunchException::~CLaunchException()
{
	delete m_pcMessage;
}

CLaunchException::CLaunchException(DWORD dwErrorCode, LPCSTR pcMessage, ...)
: m_dwErrorCode(dwErrorCode), m_dwWindowsErrorCode(0)
{
	va_list args;
	assert(pcMessage != NULL);
	va_start(args, pcMessage);
	m_pcMessage = new char[strlen(pcMessage)+1000];
	vsprintf_s(m_pcMessage, strlen(pcMessage)+1000, pcMessage, args);
}

CLaunchException::CLaunchException(DWORD dwErrorCode, DWORD dwWindowsErrorCode, LPCSTR pcMessage, ...)
: m_dwErrorCode(dwErrorCode), m_dwWindowsErrorCode(dwWindowsErrorCode)
{
	LPSTR pcWindowsMessage;
	LPSTR pcFormat;
	size_t dwLength;
	va_list args;
	assert(pcMessage != NULL);
	va_start(args, pcMessage);
	pcWindowsMessage = new char[1000];
	FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		m_dwWindowsErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) pcWindowsMessage,
		1000,
		NULL 
	);
	dwLength = strlen(pcMessage)+1000;
	pcFormat = new char[dwLength];
	vsprintf_s(pcFormat, dwLength, pcMessage, args);
	strcat_s(pcFormat, dwLength, "\n%s (0x%08X)");

	dwLength = strlen(pcFormat)+strlen(pcWindowsMessage)+100;
	m_pcMessage = new char[dwLength];	
	sprintf_s(m_pcMessage, dwLength, pcFormat, pcWindowsMessage, dwWindowsErrorCode);
	delete pcFormat;
}

CLaunchException::CLaunchException(const CLaunchException& copy)
: m_dwErrorCode(copy.m_dwErrorCode), m_dwWindowsErrorCode(copy.m_dwWindowsErrorCode)
{
	size_t dwLength;

	dwLength = strlen(copy.m_pcMessage)+1;
	m_pcMessage = new char[dwLength];
	strcpy_s(m_pcMessage, dwLength, copy.m_pcMessage);
}

CLaunchException::CLaunchException(DWORD dwErrorCode, JNIEnv* pJavaNativeInterface, LPCSTR pcMessage, ...)
: m_dwErrorCode(dwErrorCode), m_dwWindowsErrorCode(0)
{
	size_t dwLength = 0;
	LPSTR pcThrowableMessageAndStackTrace = NULL;
	va_list args;

	assert(pcMessage != NULL);
	assert(pJavaNativeInterface);
	va_start(args, pcMessage);
	if (pJavaNativeInterface->ExceptionCheck())
	{
		pcThrowableMessageAndStackTrace = getJavaExceptionMessageAndStackTrace(
				pJavaNativeInterface,
				pJavaNativeInterface->ExceptionOccurred());
		if (pcThrowableMessageAndStackTrace != NULL)
		{
			dwLength = strlen(pcThrowableMessageAndStackTrace);
		}
	}
	dwLength += strlen(pcMessage)+1000;
	m_pcMessage = new char[dwLength];
	vsprintf_s(m_pcMessage, dwLength, pcMessage, args);
	if (pcThrowableMessageAndStackTrace != NULL)
	{
		strcat_s(m_pcMessage, dwLength, "\n");
		strcat_s(m_pcMessage, dwLength, pcThrowableMessageAndStackTrace);
		delete pcThrowableMessageAndStackTrace;
	}
}

LPSTR CLaunchException::getJavaExceptionMessageAndStackTrace(JNIEnv* pJavaNativeInterface, jthrowable javaException)
{
	jclass clazz;
	jmethodID method;
	jstring message;
	const char* bytes;
	LPSTR pcBuffer;
	jint iSize;

	CLog::debug("getJavaExceptionMessageAndStackTrace()");
	if (javaException != NULL)
	{
		pJavaNativeInterface->ExceptionDescribe();
		clazz = pJavaNativeInterface->GetObjectClass(javaException);
		if (clazz == NULL)
		{
			CLog::error("Could not evaluate class of java exception");
			return NULL;
		}
		method = pJavaNativeInterface->GetMethodID(clazz, "toString", "()Ljava/lang/String;");
		if (method == NULL)
		{
			CLog::error("Could not get method getMessage of java exception");
			return NULL;
		}
		pJavaNativeInterface->ExceptionClear();
		message = (jstring)pJavaNativeInterface->CallObjectMethod(javaException, method);
		if (message == NULL)
		{
			CLog::error("Could not call method getMessage of java exception");
			return NULL;
		}
		iSize = pJavaNativeInterface->GetStringUTFLength(message);
		bytes = pJavaNativeInterface->GetStringUTFChars(message, NULL);
		if (bytes == NULL)
		{
			CLog::error("Could not get method message string of java exception");
			return NULL;
		}
		pcBuffer = new char[iSize+1];
		strcpy_s(pcBuffer, iSize+1, bytes);
		return pcBuffer;
	}
	return NULL;
}

LPSTR CLaunchException::getMessage()
{
	return m_pcMessage;
}

DWORD CLaunchException::getErrorCode()
{
	return m_dwErrorCode;
}

DWORD CLaunchException::getWindowsErrorCode()
{
	return m_dwWindowsErrorCode;
}

