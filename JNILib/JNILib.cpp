/* ======================================================================
 * 
 * Java Native Interface Helper Library
 *
 * ======================================================================
 *
 * Provides some useful functions to use with the java native interface
 *
 * ======================================================================
 *
 * Author    : Reto Weiss
 * Copyright : AXON Ivy 
 * Date      : 20. Sept. 2000
 * Version   : 1.0
 *
 * =======================================================================
 * 
 * 20.9.2000  1.0  ReW Created
 *
 * ======================================================================*/
#include "JNILib.h"
#include <windows.h>

const char CONSTRUCTOR[] = "<init>";
const char DEFAULT_SIG[] = "()V";

/*
 * Gets the last error message
 */
const char* GetLastErrorMessage(char* lpszBuffer, DWORD dwBufferLength)
{
	FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		lpszBuffer,
		dwBufferLength,
		NULL 
	);
	return reinterpret_cast<const char*>(lpszBuffer);
}

/*
 * Filters Carage Return charakter '\r' out of a string
 */
void FilterCrString(const char* lpszString)
{
	DWORD index = 0;
	while(index < strlen(lpszString))
	{
		if  (lpszString[index] == '\r')
		{
			memmove((void*)(lpszString+index), (void*)(lpszString+index+1), strlen(lpszString)-index);
		}
		else
		{
			index++;
		}
	}
}


/* 
 * Throws an exception of the class pcExceptionClassName with the message pcMessage
 */
void Throw(JNIEnv* env, const char* pcExceptionClassName, const char* pcMessage)
{
	jclass clazz = env->FindClass(pcExceptionClassName);
	if (clazz==NULL) return;
	FilterCrString(pcMessage);
	env->ThrowNew(clazz, pcMessage);
}

void ThrowWin32(JNIEnv* env, const char* pcExceptionClassName, const char* pcMessage)
{
	char buffer[1000];
	sprintf_s(buffer, 1000, "%s (Error code: 0x%08X)", pcMessage, GetLastError());
	Throw(env, pcExceptionClassName, buffer);

}

/*
 * Throws an exception of the class pcExceptionClassName with the message pcMessage,
 * the last win32 error code and the win32 error message
 */
void ThrowWin32Msg(JNIEnv* env, const char* pcExceptionClassName, const char* pcMessage)
{
	char buffer[1000];
	char buffer2[1000];
	if (GetLastError()==S_OK)
	{
		Throw(env, pcExceptionClassName, pcMessage);
	}
	else
	{
		sprintf_s(buffer, 1000, "%s\nError code: 0x%08X\nError message: %s", pcMessage, GetLastError(), GetLastErrorMessage(buffer2, 1000));
	}
	Throw(env, pcExceptionClassName, buffer);
}

/*
 * Create a local object of the class pcClassName no constructor is called
 */
jobject CreateUninitObject(JNIEnv* env, const char* pcClassName)
{
	jclass clazz;
		
	clazz= env->FindClass(pcClassName);
	if (clazz == NULL) return NULL;
	return env->AllocObject(clazz);
}

/*
 * Create a local object of the class pcClassName and calls the default constructor
 */
jobject CreateObject(JNIEnv* env, const char* pcClassName)
{
	jclass clazz;
	jmethodID methodId;
	
	clazz = env->FindClass(pcClassName);
	if (clazz == NULL) return NULL;
	methodId = env->GetMethodID(clazz, CONSTRUCTOR, DEFAULT_SIG);
	if (methodId == NULL) return NULL;
	return env->NewObject(clazz, methodId);

};

/*
 * Create a global object of the class pcClassName and calls the default constructor
 */
jobject CreateGblObject(JNIEnv* env, const char* pcClassName)
{
	jobject object;
	object = CreateObject(env, pcClassName);
	if (object == NULL) return NULL;
	object = env->NewGlobalRef(object);
	return object;
}

/*
 * Get method
 */
jmethodID GetMethod(JNIEnv* env, jobject object, const char* pcMethodName, const char* pcSignature)
{
	jclass clazz = env->GetObjectClass(object);
	return env->GetMethodID(clazz, pcMethodName, pcSignature);
}

/*
 * Gets a method id of the constructor
 */
jmethodID GetConstructor(JNIEnv* env, jobject object, const char* pcSignature)
{
	return GetMethod(env, object, CONSTRUCTOR, pcSignature);
}

/*
 * Gets a field id 
 */
jfieldID GetField(JNIEnv* env, jobject object, const char* pcFieldName, const char* pcSignature)
{
	jclass clazz = env->GetObjectClass(object);
	return env->GetFieldID(clazz, pcFieldName, pcSignature);
}

/*
 * Garbage Collection
 */
void GarbageCollection(JNIEnv* env)
{
	jmethodID method;
	jclass clazz = env->FindClass("java/lang/System");
	if (clazz==NULL) return;
	method = env->GetStaticMethodID(clazz, "gc", "()V");
	env->CallStaticVoidMethod(clazz, method);
}



