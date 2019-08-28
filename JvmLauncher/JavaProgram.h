/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

A java programm

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
#ifndef __JAVA_PROGRAM_H__
#define __JAVA_PROGRAM_H__

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

#include <jni.h>
#include "LaunchConfiguration.h"
#include "JavaVirtualMaschine.h"
#include "LaunchException.h"
#include "JavaMainArguments.h"

class JVMLauncherExport CJavaProgram
{
private:
	/*
	 * Calls the java main method
	 * @param pJvm a pointer to the java virtual maschine to use to call the main function
	 * @param javaMainArguments pointer to java main arguments
	 */
	void callJavaMain(CJavaVirtualMaschine* pJvm, JavaMainArguments& javaMainArguments);

	/*
	 * Initializes command vm options
	 * @param options the vm options to initialize with the additional vm Options
	 * @param javaMainArguments pointer to java main arguments
	 */
	void initializeCommandVmOptions(CVmOptions& options, JavaMainArguments& javaMainArguments);

	/*
	 * Initializes the class path option
	 * @param options the java vm options to initialize with the classpath
	 * @param pcApplicationDirectory the application directory
	 */
	void initializeClassPathOption(CVmOptions& options, LPCSTR pcApplicationDirectory);

	/*
	 * Adds all jars found in the relative lib directory to the classpath 
	 * @param pcClasspath the classpath buffer 
	 * @param dwClasspathLength the length of the classpath buffer
	 * @param pcApplicationDirectory the application directory
	 * @param pcRelativeLibDirectory the lib directory relative to the application directory
	 */
	void addJarsToClasspath(LPSTR& pcClasspath, DWORD& dwClasspathLength, LPCSTR pcApplicationDirectory, LPCSTR pcRelativeLibDirectory);

	/*
	 * Adds a path to the classpath
	 * @param pcClasspath the classpath buffer
	 * @param dwClasspathLength the length of the classpath buffer
	 * @param pcPathToAdd the path to add
	 * @param pcFiltToAdd the file to add. Maybe NULL
	 */
	void addToClasspath(LPSTR& pcClasspath, DWORD& dwClasspathLength, LPCSTR pcPathToAdd, LPCSTR pcFileToAdd);

	/*
	 * Initializes the osgi vm options
	 * @param options the java vm options
	 * @param pcApplicationDirectory the application directory
     */
	void initializeOsgiVmOptions(CVmOptions& options, LPCSTR pcApplicationDirectory);

	/*
	 * Initializes the java module system vm options
	 * @param options the java vm options
	 */
	void initializeJavaModuleSystemVmOptions(CVmOptions& options);

	void initializeJvmOptions(CVmOptions& options, LPCSTR pcApplicationDirectory);

	/*
	 * Checks if there is already an instance of this application program running 
	 * @return true if there is already an instance running, otherwise false
	 * @throws CLaunchException if an error occurs 
	 */
	bool checkIsAlreadyRunning();

	/*
	 * Checks if the application must run as osgi application
	 * @return true if the application must run as osgi application
	 */
	bool launchAsOsgiApplication();

	/*
	 * Converts a c-style string to a java string
	 * @param pJavaNativeInterface the java native interface
	 * @param pcString the c-style string to convert
	 * @return java string
	 * @throws CLaunchException if conversion fails
	 */
	jstring convert2JavaString(JNIEnv* pJavaNativeInterface, LPCSTR pcString);

	/*
	 * Register an event source for the event log
	 * @param pcEventSourceName the name of the event source to register
	 */
	void registerEventSource(LPCSTR pcEventSourceName);

	/*
	 * Adds the OSGI java main arguments to java main arguments.
	 * @param pJavaMainArguments out variable, javaMainArguments
	 */
	void addOsgiJavaMainArguments(JavaMainArguments* pJavaMainArguments);

	/*
	 * Logs the java main class with its arguments.
	 * @param mainJavaClass java main class
	 * @param javaMainArguments point to java main arguments
	 */
	void logStartingJavaClass(LPCSTR pcMainJavaClass, JavaMainArguments& javaMainArguments);

protected:
	/* The launch configuration that defines the options of the java virtual maschine and the main class */
	CLaunchConfiguration m_launchConfiguration;
	
	/*
	 * Gets the jvm path 
	 * @param pcApplicationDirectory the application directory
	 * @param pcJvmPathBuffer buffer where to store the jvm path
	 * @param dwBufferLength the length of the jvm path buffer 
	 * @return the jvm path
	 */
	LPSTR getJvmPath(LPCSTR pcApplicationDirectory, LPSTR pcJvmPathBuffer, DWORD dwBufferLength);

	/*
	 * Evaluates the JRE directory (the bundled or from the System properties)
	 */
	void CJavaProgram::evaluateJreDirectory(LPSTR pcJvmPathBuffer, DWORD dwBufferLength);

	/*
	 * Gets the application directory
	 * @param pcApplicationDirectoryBuffer the buffer where to store the application directory
	 * @param dwBufferLength the length of the bufffer
	 * @return the application directory 
	 * @throws CLaunchException if application directory can not be evaluated
	 */
	LPSTR getApplicationDirectory(LPSTR pcApplicationDirectoryBuffer, DWORD dwBufferLength);

	/*
	 * Gets a java static method of a java class
	 * @param pJavaNativeInterface the java native interface
	 * @param clazz the java class to get a method of 
	 * @param pcMethodName the java method name
	 * @param pcSignature the signature of the jaa main method
	 * @return method id of the method 
	 * @throw CLaunchException if java method could not be found 
	 */
	jmethodID getJavaStaticMethod(JNIEnv* pJavaNativeInterface, jclass clazz, LPCSTR pcMethodName, LPCSTR pcSignature);

	/*
	 * Initializes the vm options
	 * @param otions the vm options to initialize
	 * @param pcApplicationDirectory the application directory 
	 * @param javaMainArguments pointer to java main arguments
	 */
	void initializeVmOptions(CVmOptions& options, LPCSTR pcApplicationDirectory, JavaMainArguments& javaMainArguments);

	/*
	 * Checks if the given arguments contains the server stop argument 
	 * @param argc the number of arguments
	 * @param argv the arguments
	 * @return true / false
	 */
	bool CJavaProgram::containsServerStopArgument(int argc, LPSTR argv[]);

	/*
	 * Loads a java class
	 * @param pJavaNativeInterface the java native interface to use to load the class
	 * @param pcClassName the name of the class 
	 * @return pointer to the java class loaded
	 * @throw CLaunchException if java class could not be loaded
	 */
	jclass getJavaClass(JNIEnv *pJavaNativeInterface, LPCSTR pcClassName);
	
	/*
	 * Gets a java method of a java class
	 * @param pJavaNativeInterface the java native interface
	 * @param clazz the java class to get a method of 
	 * @param pcMethodName the java method name
	 * @param pcSignature the signature of the jaa main method
	 * @return method id of the method 
	 * @throw CLaunchException if java method could not be found 
	 */
	jmethodID getJavaMethod(JNIEnv* pJavaNativeInterface, jclass clazz, LPCSTR pcMethodName, LPCSTR pcSignature);

	/*
	 * Converts the c-style option into an java string array
	 * @param pJavaNativeInterface the java native interface
	 * @param argc the number of Options
	 * @param argv the option
	 * @return java string array with the option
	 * @throws CLaunchException if conversion fails
	 */
	jobjectArray convert2JavaStringArray(JNIEnv* pJavaNativeInterface, int argc, LPSTR argv[]);

public:
	/*
	 * Constructor
	 * @param launchConfiguration the launch configuration 
	 */
	CJavaProgram(CLaunchConfiguration& launchConfiguration);

	/*
	 * Creates a java program.
	 * Uses the launch configuration given
	 * @param launchConfiguration the launch configuration to used
 	 * @return the created java program instance
	 */
	static CJavaProgram* createJavaProgram(CLaunchConfiguration launchConfiguration);

	/*
	 * Calls the main method of the java program
	 * @param argc the number of arguments passed to the java program
	 * @param argv the arguments passed to the java program
	 */
	virtual void main(int argc, LPSTR argv[]);

	/*
	 * Reports an error to the system event log
	 * @param ex the exception to report
	 */ 
	void reportError(CLaunchException ex);

	/*
	 * Destructor
	 */
	virtual ~CJavaProgram(void);
};

#endif