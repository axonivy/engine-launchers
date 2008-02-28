/*-----------------------------------------------------------------------------------

   Java Nt Service Handler

-------------------------------------------------------------------------------------

  Implements the native method of class ivyteam.server.NtServiceHandler

--------------------------------------------------------------------------------------

Project		: JavaNtServiceHandler
Version		: 1.0
Compiler	: VC++ 5
Java		: JDK1.3
Author		: Reto Weiss
Copyright	: Ivyteam, 2001

--------------------------------------------------------------------------------------
History:
		  26.5.2003 ReW Bug #1349 ThrowWin32 and ThrowWin32Msg calls must be direct after the 
		                the failed Win32 API call. If not an other error code may be reported
						of a Win32 API call which is called after the failed one.
		  12.2.2001 ReW created
--------------------------------------------------------------------------------------*/
#include <windows.h>
#include <jni.h>
#include <io.h>
#include <iostream>
#include "..\JNILib\JNILib.h"

#define JavaNtServiceHandler
#include "ivyteam_server_NtServiceHandler.h"


/*
 * Gets the service handle out of the java object
 */
SC_HANDLE GetServiceHandle(JNIEnv *jpEnv, jobject jpThis)
{
	jlong jlServiceHandle;
	jfieldID jpField;

	jpField = GetField(jpEnv, jpThis, "serviceHandle", "J");
	if (jpField==NULL)
	{
		Throw(jpEnv, "ivyteam/server/NtServiceException", "serviceHandle field not found");
		return NULL;
	}

	jlServiceHandle = jpEnv->GetLongField(jpThis, jpField);
	return reinterpret_cast<SC_HANDLE>(jlServiceHandle);
}

/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    openService
 * Signature: (Ljava/lang/String;)Livyteam/server/NtServiceHandler;
 */
JNIEXPORT jobject JNICALL Java_ivyteam_server_NtServiceHandler_openService
  (JNIEnv *jpEnv, jclass jpClass, jstring jpStrServiceName)
{
	LPCSTR pcServiceName;
	LPSTR pcDependencies;
	jboolean isCopy;
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	BOOL success;
	DWORD bytesNeeded;
	LPQUERY_SERVICE_CONFIG lpServiceConfig;
	jstring jpStrDisplayName, jpStrBinaryPathName, jpStrServiceUserName;
	jint jiStartKind;
	jlong jlServiceHandle;
	jobject jpServiceHandler, jpDependOnServices;
	jmethodID jpConstructor, jpVectorAdd;

	pcServiceName = jpEnv->GetStringUTFChars(jpStrServiceName, &isCopy);
	
	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not open service manager");
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		return NULL;
	}
	 
	// open service
    hSCService = OpenService( 
        hSCManager,				// SCM database 
        TEXT(pcServiceName),  // service name
        SERVICE_ALL_ACCESS); 
 
    if (hSCService == NULL) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not open service");
		CloseServiceHandle(hSCManager);
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		return NULL;
	}
	
	// allocate config buffer
	lpServiceConfig = reinterpret_cast<LPQUERY_SERVICE_CONFIG>(LocalAlloc(LPTR, 4096));
	if (lpServiceConfig == NULL)
	{
		ThrowWin32Msg(jpEnv, "java/lang/OutOfMemoryError", "Could not allocate buffer for service configuration");
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		return NULL;
	}
	
	// query service configuration
	success = QueryServiceConfig(
					hSCService, 
					lpServiceConfig, 
					4096, 
					&bytesNeeded);

	if (success == FALSE)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not query service configuration");
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		LocalFree(lpServiceConfig); 
		return NULL;
	}
	
	// prepare parameters
	jiStartKind = lpServiceConfig->dwStartType;
	jpStrDisplayName = jpEnv->NewStringUTF(lpServiceConfig->lpDisplayName);
	jpStrBinaryPathName = jpEnv->NewStringUTF(lpServiceConfig->lpBinaryPathName);
	jpStrServiceUserName = jpEnv->NewStringUTF(lpServiceConfig->lpServiceStartName);
	jlServiceHandle = reinterpret_cast<jlong>(hSCService);
	pcDependencies = lpServiceConfig->lpDependencies;

	// create java.util.Vector to store depend on service names in it
	jpDependOnServices = CreateObject(jpEnv, "java/util/Vector");
	if (jpDependOnServices == NULL)
	{
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not create Vector object");
		return NULL;
	}
	jpVectorAdd = GetMethod(jpEnv,jpDependOnServices , "add", "(Ljava/lang/Object;)Z");
	if (jpVectorAdd == NULL)
	{
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not get method add(Object) of Vector class");
		return NULL;
	}

	// add all depend on service names to the vector
	while ((pcDependencies!=NULL)&&(pcDependencies[0] != '\0'))
	{	
		// call add method on the vector
		jpEnv->CallBooleanMethod(jpDependOnServices, jpVectorAdd, jpEnv->NewStringUTF(pcDependencies));
		pcDependencies=pcDependencies+strlen(pcDependencies)+1;
	}
	
	// release config buffer
	LocalFree(lpServiceConfig); 
	
	// create service handler object
	jpServiceHandler = CreateUninitObject(jpEnv, "ivyteam/server/NtServiceHandler");
	if (jpServiceHandler == NULL)
	{
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not create NtServiceHandler object");
		return NULL;
	}
	
	// get constructor
	jpConstructor = GetConstructor(jpEnv, jpServiceHandler, "(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/util/Vector;)V");
	if (jpConstructor == NULL)
	{
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		Throw(jpEnv, "ivyteam/server/NtServiceException", "NtServiceHandler constructor not found");
		return NULL;
	}
	
	// call constructor
	jpEnv->CallVoidMethod(
				jpServiceHandler, 
				jpConstructor, 
				jlServiceHandle, 
				jpStrServiceName,
				jpStrDisplayName,
				jpStrBinaryPathName,
				jpStrServiceUserName,
				jiStartKind,
				jpDependOnServices);
	
	jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
	CloseServiceHandle(hSCManager);

	return jpServiceHandler;
}

/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    getServiceNames
 * Signature: ()Ljava/util/Enumeration;
 */
JNIEXPORT jobject JNICALL Java_ivyteam_server_NtServiceHandler_getServiceNames
  (JNIEnv *jpEnv, jclass jpClass)
{
	BOOL success;
	DWORD dwServicesReturned, dwNextService, dwBytesNeeded;
	LPENUM_SERVICE_STATUS lpEnumServiceStatus;
	SC_HANDLE hSCManager;
	
	jobject jpVector;
	jmethodID jpAddElementMethod, jpElementsMethod;
	jstring jpStrServiceName;

	// create vector object
	jpVector = CreateObject(jpEnv, "java/util/Vector");
	if (jpVector==NULL)
	{
		Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not create java/util/Vector object");
		return NULL;
	}

	// get addElement method of vector
	jpAddElementMethod = GetMethod(jpEnv, jpVector, "addElement", "(Ljava/lang/Object;)V");
	if (jpAddElementMethod == NULL)
	{
		Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not get addElement method of java/util/Vector object");
		return NULL;
	}

	// get elements method of vector
	jpElementsMethod = GetMethod(jpEnv, jpVector, "elements", "()Ljava/util/Enumeration;");
	if (jpAddElementMethod == NULL)
	{
		Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not get addElement method of java/util/Vector object");
		return NULL;
	}

	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not open service manager");
		return NULL;
	}

	lpEnumServiceStatus = reinterpret_cast<LPENUM_SERVICE_STATUS>(LocalAlloc(LPTR, 4096));
	if (lpEnumServiceStatus == NULL)
	{
		ThrowWin32Msg(jpEnv, "java/lang/OutOfMemoryError", "Could not allocate buffer for service configuration");
		CloseServiceHandle(hSCManager);
		return NULL;
	}

	dwNextService = 0;

	do
	{
		success = EnumServicesStatus(
					hSCManager,
					SERVICE_WIN32,
					SERVICE_STATE_ALL,
					lpEnumServiceStatus,
					4096,
					&dwBytesNeeded,
					&dwServicesReturned,
					&dwNextService);
		if ((success == false)&&(GetLastError()!=ERROR_MORE_DATA))
		{
			ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not enumerate services");
			CloseServiceHandle(hSCManager);
			LocalFree(lpEnumServiceStatus);
			return NULL;
		}

		for (DWORD index = 0; index < dwServicesReturned; index++)
		{
			jpStrServiceName = jpEnv->NewStringUTF(lpEnumServiceStatus[index].lpServiceName);
			if (jpStrServiceName == NULL)
			{
				CloseServiceHandle(hSCManager);
				LocalFree(lpEnumServiceStatus);
				Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not enumerate services");
				return NULL;
			}

			jpEnv->CallVoidMethod(jpVector, jpAddElementMethod, jpStrServiceName);
		}
	} while (success == false);

	LocalFree(lpEnumServiceStatus);
	CloseServiceHandle(hSCManager);

	return jpEnv->CallObjectMethod(jpVector, jpElementsMethod);
}



/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    createService
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Livyteam/server/NtServiceHandler;
 */
JNIEXPORT jobject JNICALL Java_ivyteam_server_NtServiceHandler_createService
  (JNIEnv *jpEnv, jclass jpClass, jstring jpStrServiceName, jstring jpStrDisplayName, jstring jpStrBinaryPathName)
{
	LPCSTR pcServiceName;
	LPCSTR pcDisplayName;
	LPCSTR pcBinaryPathName;
	jboolean isCopy;
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	
	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not open service manager");
		return NULL;
	}

	pcServiceName = jpEnv->GetStringUTFChars(jpStrServiceName, &isCopy);
	pcDisplayName = jpEnv->GetStringUTFChars(jpStrDisplayName, &isCopy);
	pcBinaryPathName = jpEnv->GetStringUTFChars(jpStrBinaryPathName, &isCopy);
	

	// Create service
	hSCService = CreateService( 
        hSCManager,                // SCManager database 
        pcServiceName,			   // name of service 
        pcDisplayName,             // service name to display 
        SERVICE_ALL_ACCESS,	       // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,	   // error control type 
        pcBinaryPathName,	       // service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,		               // LocalSystem account 
        NULL);			           // no password 
 
    if (hSCService == NULL) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not create service");
		jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
		jpEnv->ReleaseStringUTFChars(jpStrDisplayName, pcDisplayName);
		jpEnv->ReleaseStringUTFChars(jpStrBinaryPathName, pcBinaryPathName);
		CloseServiceHandle(hSCManager);
        return NULL;
	}
    
    CloseServiceHandle(hSCService); 
	CloseServiceHandle(hSCManager);
	jpEnv->ReleaseStringUTFChars(jpStrServiceName, pcServiceName);
	jpEnv->ReleaseStringUTFChars(jpStrDisplayName, pcDisplayName);
	jpEnv->ReleaseStringUTFChars(jpStrBinaryPathName, pcBinaryPathName);
		
	return Java_ivyteam_server_NtServiceHandler_openService(jpEnv, jpClass, jpStrServiceName);
}


/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    getServiceState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_ivyteam_server_NtServiceHandler_getServiceState
  (JNIEnv *jpEnv, jobject jpThis)
{
	SERVICE_STATUS serviceStatus;
	BOOL success;
			
	success = QueryServiceStatus(
					GetServiceHandle(jpEnv, jpThis),
					&serviceStatus);

	if (success == false)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not query service status");
		return 0;
	}

	return serviceStatus.dwCurrentState;
}

/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    startService
 * Signature: ()I
 */
JNIEXPORT void JNICALL Java_ivyteam_server_NtServiceHandler_startService
  (JNIEnv *jpEnv, jobject jpThis)
{
	BOOL success;
	
	success = StartService(
					GetServiceHandle(jpEnv, jpThis),
					0, 
					NULL);

	if (success == false)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not start service");
		return;
	}
}

/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    stopService
 * Signature: ()I
 */
JNIEXPORT void JNICALL Java_ivyteam_server_NtServiceHandler_stopService
  (JNIEnv *jpEnv, jobject jpThis)
{
	SERVICE_STATUS serviceStatus;
	BOOL success;
	
	success = ControlService(
					GetServiceHandle(jpEnv, jpThis),
					SERVICE_CONTROL_STOP, 
					&serviceStatus);

	if (success == false)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not stop service");
		return;
	}
}

/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ivyteam_server_NtServiceHandler_close
  (JNIEnv *jpEnv, jobject jpThis)
{
	
	CloseServiceHandle(GetServiceHandle(jpEnv, jpThis));
}

/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    setStartTypeNative
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_ivyteam_server_NtServiceHandler_setStartTypeNative
  (JNIEnv *jpEnv, jobject jpThis, jint jiStartType)
{
	BOOL success;
	SC_HANDLE hSCService;
	SC_HANDLE hSCManager;
	SC_LOCK hSCLock;

	hSCService = GetServiceHandle(jpEnv, jpThis);

	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not open service manager");
		return;
	}

	// Lock the service database 
	hSCLock = LockServiceDatabase(hSCManager);
	
	if (hSCLock == NULL)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not lock service database");
		CloseServiceHandle(hSCManager);
		return;
	}

	// change service configuration
	success = ChangeServiceConfig(
					hSCService, 
					SERVICE_NO_CHANGE ,
					jiStartType,
					SERVICE_NO_CHANGE ,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL);

	if (success == FALSE)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not change service configuration");
		// Do not return here because service database must be unlocked first
	}

	// unlock the service database
	UnlockServiceDatabase(hSCLock);

	CloseServiceHandle(hSCManager);		
}


/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    setServiceUserNative
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ivyteam_server_NtServiceHandler_setServiceUserNative
  (JNIEnv *jpEnv, jobject jpThis, jstring jpStrUserName, jstring jpStrPassword)
{
	BOOL success;
	SC_HANDLE hSCService;
	SC_HANDLE hSCManager;
	SC_LOCK hSCLock;

	LPCSTR lpszUserName, lpszPassword;
	jboolean isCopy;

	hSCService = GetServiceHandle(jpEnv, jpThis);

	if (jpStrUserName == NULL)
	{
		lpszUserName= "LocalSystem";
		lpszPassword= "LocalSystem";
	}
	else
	{
		lpszUserName = jpEnv->GetStringUTFChars(jpStrUserName, &isCopy);
		lpszPassword = jpEnv->GetStringUTFChars(jpStrPassword, &isCopy);
	}

	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not open service manager");
		return;
	}

	// Lock the service database 
	hSCLock = LockServiceDatabase(hSCManager);
	
	if (hSCLock == NULL)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not lock service database");
		CloseServiceHandle(hSCManager);	
		return;
	}

	// change service configuration
	success = ChangeServiceConfig(
					hSCService,
					SERVICE_NO_CHANGE ,
					SERVICE_NO_CHANGE ,
					SERVICE_NO_CHANGE ,
					NULL,
					NULL,
					NULL,
					NULL,
					lpszUserName,
					lpszPassword,
					NULL);

	if (success == FALSE)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not change service configuration");
		// Do not return here because service database must be unlocked and strings must be released first
	}

	// unlock the service database
	UnlockServiceDatabase(hSCLock);
	
	
	// release utf char string
	if (jpStrUserName!=NULL)
	{
		jpEnv->ReleaseStringUTFChars(jpStrUserName, lpszUserName);
		jpEnv->ReleaseStringUTFChars(jpStrPassword, lpszPassword);
	}

	CloseServiceHandle(hSCManager);		
}

/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    deleteService
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ivyteam_server_NtServiceHandler_deleteService
  (JNIEnv *jpEnv, jobject jpThis)
{
	SC_HANDLE hSCService;
	
	hSCService = GetServiceHandle(jpEnv, jpThis);

	if (! DeleteService(hSCService) ) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not delete service");
	}
	CloseServiceHandle(hSCService);
}

/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    setDependOnServices
 * Signature: (Ljava/util/Vector;)V
 */
JNIEXPORT void JNICALL Java_ivyteam_server_NtServiceHandler_setDependOnServices
  (JNIEnv * jpEnv, jobject jpThis, jobject jpDependOnServices)
{
	BOOL success;
	SC_HANDLE hSCService;
	SC_HANDLE hSCManager;
	SC_LOCK hSCLock;
	jmethodID jpGetSize, jpElementAt;
	LPSTR pcDependencies;
	LPCSTR* ppcDependencies;
	jstring*  jppDependencies;
	jint numberOfDependencies;
	jboolean isCopy;
	size_t dependenciesSize;
	size_t bufferLength;

	jpGetSize = GetMethod(jpEnv, jpDependOnServices, "size", "()I");
	if (jpGetSize==NULL)
	{
		Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not get method size() of class Vector");
		return;
	}

	jpElementAt = GetMethod(jpEnv, jpDependOnServices, "elementAt", "(I)Ljava/lang/Object;");
	if (jpGetSize==NULL)
	{
		Throw(jpEnv, "ivyteam/server/NtServiceException", "Could not get method elementAt(int) of class Vector");
		return;
	}

	hSCService = GetServiceHandle(jpEnv, jpThis);

	// Open a handle to the SC Manager database. 
	hSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (hSCManager == NULL) 
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not open service manager");
		return;
	}

	// Lock the service database 
	hSCLock = LockServiceDatabase(hSCManager);
	
	if (hSCLock == NULL)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not lock service database");
		CloseServiceHandle(hSCManager);
		return;
	}

	// Get number of dependent services
	numberOfDependencies = jpEnv->CallIntMethod(jpDependOnServices, jpGetSize);

	// Allocate a java string array and a c++ string array
	ppcDependencies = (LPCSTR*)LocalAlloc(LPTR, (numberOfDependencies)*sizeof(LPSTR));
	jppDependencies = (jstring*)LocalAlloc(LPTR, numberOfDependencies*sizeof(jstring));
	dependenciesSize = 0;

	// Get the java strings and c++ strings out of the vector
	for (jint pos=0; pos < numberOfDependencies; pos++)
	{
		jppDependencies[pos] = (jstring)jpEnv->CallObjectMethod(jpDependOnServices, jpElementAt, pos);
		ppcDependencies [pos] = jpEnv->GetStringUTFChars(jppDependencies[pos], &isCopy);
		dependenciesSize += strlen(ppcDependencies[pos])+1;
	}

	// Allocate block to store c++ strings

	pcDependencies = (LPSTR)LocalAlloc(LPTR, dependenciesSize+1);

	// Store c++ strings in block
	bufferLength = dependenciesSize+1;
	dependenciesSize = 0;
	for (int pos = 0; pos < numberOfDependencies; pos++)
	{
		strcpy_s(&pcDependencies[dependenciesSize], bufferLength, ppcDependencies[pos]);
		dependenciesSize += strlen(ppcDependencies[pos])+1;
		bufferLength -= (strlen(ppcDependencies[pos])+1);
	}
	pcDependencies[dependenciesSize] = '\0';

	// change service configuration
	success = ChangeServiceConfig(
					hSCService, 
					SERVICE_NO_CHANGE ,
					SERVICE_NO_CHANGE,
					SERVICE_NO_CHANGE ,
					NULL,
					NULL,
					NULL,
					pcDependencies,
					NULL,
					NULL,
					NULL);

	if (success == FALSE)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not change service configuration");
		// Do not return here because the service database must be unlocked and the strings and buffers must be released first
	}

	// unlock the service database
	UnlockServiceDatabase(hSCLock);

	// release c++ strings
	for (int pos=0; pos < numberOfDependencies; pos ++)
	{
		jpEnv->ReleaseStringUTFChars(jppDependencies[pos], ppcDependencies[pos]);
	}

	// release memory
	LocalFree(jppDependencies);
	LocalFree(ppcDependencies);
	LocalFree(pcDependencies);

	CloseServiceHandle(hSCManager);
}


/*
 * Class:     ivyteam_server_NtServiceHandler
 * Method:    getExitCode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_ivyteam_server_NtServiceHandler_getExitCode
  (JNIEnv *jpEnv, jobject jpThis)
{
	SERVICE_STATUS serviceStatus;
	BOOL success;
			
	success = QueryServiceStatus(
					GetServiceHandle(jpEnv, jpThis),
					&serviceStatus);

	if (success == false)
	{
		ThrowWin32Msg(jpEnv, "ivyteam/server/NtServiceException", "Could not query service status");
		return 0;
	}

	return serviceStatus.dwWin32ExitCode;
}
