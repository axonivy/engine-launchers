/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

Java virtual maschine

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
#ifndef __JAVA_VIRTUAL_MASCHINE_H__
#define __JAVA_VIRTUAL_MASCHINE_H__

#include <jni.h>
#include <windows.h>
#include "VmOptions.h"

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

struct JavaVirtualMaschineInvocationFunctions;

/*
 A java virtual maschine
 */
class JVMLauncherExport CJavaVirtualMaschine
{
private:
	/* The java native interface */
	JNIEnv* m_pJavaNativeInterface;

	/* The java virtual maschine */
	JavaVM* m_pJavaVirtualMaschine;

public:
	/*
	 Destructor
	 */
	~CJavaVirtualMaschine();

	/*
	 * Creates a java virtual maschine 
	 * @param pcJvmPath Path to the java virtual machine enviroment
	 * @param options the java virtual maschine options to use
	 * @throws CLaunchException if java virtual maschine can not be created
	 */
	static CJavaVirtualMaschine* createJavaVirtualMaschine(
		LPCSTR pcJvmPath, 
		CVmOptions &options);

	/*
	 * This method blocks until all java threads has ended.
	 * It then destroys the java vm.
	 * @throws CLaunchException if vm can not be destroyed
	 */
	void waitForAndDestroy();

	/*
	 * Attaches a current thread to the vm. Note that the returned java native interface must not
	 * be used by any other thread than the current thread calling this method.
	 * @param pcThreadName the name of the thread in the vm
	 * @return the java native interface that must be used by the attached thread
 	 * @throws CLaunchException if thread can not be attached
	 */
	JNIEnv* attachCurrentThread(LPCSTR pcThreadName);
	
	/*
	 * Detaches the current thread from the vm 
	 * @throws CLaunchException if thread can not be detached
	 */
	void detachCurrentThread();

	/*
	 * Gets the java native interface. Note that the return java native interface must not be 
	 * used by any other thread than the thread that created the vm. If another thread needs a 
	 * java native interface it should use the attachCurrentThread method to get a java native interface.
	 * @return the java native interface to use by the thread that created the vm
	 */
	JNIEnv* getJavaNativeInterface();
private:
	/*
	 * Constructor
	 * @param pJavaVirtualMaschine pointer to the jni java vm
	 * @param pJavaNativeInterface pointer to the jni environment
	 */
	CJavaVirtualMaschine(JavaVM* pJavaVirtualMaschine, JNIEnv* pJavaNativeInterface);

	/*
     * Loads the java virtual maschine into memory
	 * @param pcJvmPath the path to the java virtual maschine
	 * @param pVmInvocationFunctions pointer to a stucture that holds the functions of the vm after the vm was loaded
	 * @throws CLaunchException if java virtual maschine can not be loaded
	 */
	static void loadJavaVirtualMaschineLibrary(LPCSTR pcJvmPath, JavaVirtualMaschineInvocationFunctions *pVmInvocationFunctions);

	/*
     * Loads all libraries that are required by the java virtual maschine library
	 * @param pcJvmPath the path to the java virtual maschine
	 * @throws CLaunchException if java virtual maschine can not be loaded
	 */
	static void loadPrerequiredLibraries(LPCSTR pcJvmPath);

	/*
	 * Add the jre/bin to environment variable %PATH%, so that we can load *.dll within.
	 * @param pcJvmPath the path to the java virtual maschine
	 * @throws CLaunchException if java virtual maschine can not be loaded
	 */
	static void addJreBinToEnvPath(LPCSTR pcJvmPath);
};

#endif
