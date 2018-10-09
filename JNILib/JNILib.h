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
#ifndef __JNILIB_H__
#define __JNILIB_H__

#include <jni.h>

/*
 * Filters Carage Return charakter '\r' out of a string
 */
void FilterCrString(const char* lpszString);

/* 
 * Throws an exception of the class pcExceptionClassName with the message pcMessage
 */
void Throw(JNIEnv* env, const char* pcExceptionClassName, const char* pcMessage);

/* 
 * Throws an exception of the class pcExceptionClassName with the message pcMessage 
 * and the last win 32 error code
 */
void ThrowWin32(JNIEnv* env, const char* pcExceptionClassName, const char* pcMessage);

/*
 * Throws an exception of the class pcExceptionClassName with the message pcMessage,
 * the last win32 error code and the win32 error message
 */
void ThrowWin32Msg(JNIEnv* env, const char* pcExceptionClassName, const char* pcMessage);

/*
 * Create a local object of the class pcClassName no constructor is called
 */
jobject CreateUninitObject(JNIEnv* env, const char* pcClassName);

/*
 * Create a local object of the class pcClassName and calls the default constructor
 */
jobject CreateObject(JNIEnv* env, const char* pcClassName);

/*
 * Create a global object of the class pcClassName and calls the default constructor
 */
jobject CreateGblObject(JNIEnv* env, const char* pcClassName);

/*
 * Gets a method id
 */
jmethodID GetMethod(JNIEnv* env, jobject object, const char* pcMethodName, const char* pcSignature);

/*
 * Gets a method id of the constructor
 */
jmethodID GetConstructor(JNIEnv* env, jobject object, const char* pcSignature);

/*
 * Gets a field id 
 */
jfieldID GetField(JNIEnv* env, jobject object, const char* pcFieldName, const char* pcSignature);

/*
 * Garbage Collection
 */
void GarbageCollection(JNIEnv* env);

#endif
