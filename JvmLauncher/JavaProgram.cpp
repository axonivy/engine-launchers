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
#define JVMLauncher
#include "JavaProgram.h"
#include "JavaVirtualMaschine.h"
#include "LaunchException.h"
#include "Log.h"
#include "VmOptions.h"
#include "StringUtils.h"
#include <assert.h>
#include <io.h>

#define MAX_CLASSPATH_LENGTH 10000
#define MODE_DIRECTORY_EXISTS 00

typedef struct 
{
	bool bWindowFound;
	DWORD dwProcessId;
} WindowEnumerationParameter;

/*
 * Callback of the enumWindows Function
 */
BOOL CALLBACK enumWindowsProc( HWND hwnd, LPARAM lParam ) 
{
	DWORD processId;
	WindowEnumerationParameter* pParam;

	pParam = reinterpret_cast<WindowEnumerationParameter*>(lParam);
	
	// get owner process id of hwnd
	GetWindowThreadProcessId(hwnd, &processId);

	// if owner process is equal to first process and window is visible 
	// (There are unvisible windows from the java runtime) the window will be 
	// uniconify and set to the foreground;
	if ((processId == pParam->dwProcessId)&&(IsWindowVisible(hwnd)==TRUE))
	{
		// Restore the window with the default size (uniconify, normalize)
		ShowWindow(hwnd, SW_RESTORE);
		// Set window to foreground
		SetForegroundWindow(hwnd);
		// yes we found a window
		pParam->bWindowFound = TRUE;
	}
	return TRUE;
}

/*
 * Compares filenames
 */
int compareFileName(const void* elem1, const void* elem2)
{
	return strcmp(*static_cast<const char* const *>(elem1), *static_cast<const char* const *>(elem2));
}


CJavaProgram::CJavaProgram(CLaunchConfiguration& launchConfiguration)
: m_launchConfiguration(launchConfiguration)
{
}

CJavaProgram* CJavaProgram::createJavaProgram(CLaunchConfiguration launchConfiguration)
{
	return new CJavaProgram(launchConfiguration);
}

bool CJavaProgram::checkIsAlreadyRunning()
{
	BOOL result;

	WindowEnumerationParameter param;

	// create named file in pagefile
	HANDLE hSharedMem = CreateFileMapping(INVALID_HANDLE_VALUE, 
		NULL, PAGE_READWRITE|SEC_COMMIT, 0, 8, m_launchConfiguration.getApplicationName()); 
	if (hSharedMem != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// map file to memory
			LPVOID memory = MapViewOfFile(hSharedMem, FILE_MAP_READ, 0, 0, 8);
			if (memory != NULL)
			{
				// read process id of first process from map file
				param.dwProcessId = *((DWORD*)(memory));				
				param.bWindowFound = FALSE;
				
				// iterate throug all top level windows searching a window
				// created by the first process
				result = EnumWindows((WNDENUMPROC)enumWindowsProc, reinterpret_cast<LPARAM>(&param));
				if (result == 0)
				{
					UnmapViewOfFile(memory);
					CloseHandle(hSharedMem);	
					throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_ENUMERATE_WINDOWS,
						GetLastError(),
						"Could not enumerate windows");
				}

				// window found?
				if (param.bWindowFound == FALSE)
				{
					// No -> window not yet created -> beep
					Beep(1000, 400);
				}
				// unmap file
				UnmapViewOfFile(memory);
			}
			else
			{
				// map file failed -> beep
				CloseHandle(hSharedMem);	
				throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_MAP_VIEW_OF_VIEW,
						GetLastError(),
						"Could not map view of file");
			}
			// close handle to shared mem
			CloseHandle(hSharedMem);	
			return true;
		}		
		else
		{
			// map file to memory
			LPVOID memory = MapViewOfFile(hSharedMem, FILE_MAP_WRITE, 0, 0, 8);
			if (memory != NULL)
			{
				// stored process id in shared memory
				*((DWORD*)(memory)) = GetCurrentProcessId();
				// unmap file
				UnmapViewOfFile(memory);
			}

			return false;
		}
	}
	else
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CREATE_FILE_MAPPING,
			GetLastError(),
			"Could not create file mapping");
	}
}

bool CJavaProgram::launchAsOsgiApplication()
{
	return !(StringUtils::isEmpty(m_launchConfiguration.getOsgiApplicationName()));
}

void CJavaProgram::main(int argc, LPSTR argv[])
{
	CJavaVirtualMaschine* pJvm;
	CVmOptions vmOptions;
	char pcApplicationDirectory[MAX_PATH];
	char pcJvmPath[MAX_PATH];

	CLog::info("=============================================================");
	CLog::info("Starting java programm in class");
	CLog::info("%s", m_launchConfiguration.getMainJavaClass());
	CLog::info("with ivyteam's java launcher"); 
	CLog::info("=============================================================");

	getApplicationDirectory(pcApplicationDirectory, MAX_PATH);
	
	CLog::info("Setting current directory to '%s'.", pcApplicationDirectory);
	/* Set the current directory */
	SetCurrentDirectory(pcApplicationDirectory);
	
	disableManagementOptionForServerStop(argc, argv);

	JavaMainArguments javaMainArguments(argc, argv);
	addOsgiJavaMainArguments(&javaMainArguments);

	initializeVmOptions(vmOptions, pcApplicationDirectory, javaMainArguments);

	pJvm = CJavaVirtualMaschine::createJavaVirtualMaschine(
		getJvmPath(pcApplicationDirectory, pcJvmPath, MAX_PATH), 
		vmOptions);

	try
	{
		// call java main
		callJavaMain(pJvm, javaMainArguments);
		// if main returns, detach current thread, so that threads waiting for the main thread to die are notified
		pJvm->detachCurrentThread();
		// wait for till vm dies (all user threads end) and then destroys it
		pJvm->waitForAndDestroy();
		delete pJvm;
	}
	catch(CLaunchException ex)
	{
		delete pJvm;
		throw ex;
	}
}

LPSTR CJavaProgram::getJvmPath(LPCSTR pcApplicationDirectory, LPSTR pcJvmPathBuffer, DWORD dwBufferLength)
{
	assert(pcApplicationDirectory != NULL);
	assert(pcJvmPathBuffer != NULL);
	LPSTR pcEndOfJrePath;

	strcpy_s(pcJvmPathBuffer, dwBufferLength, pcApplicationDirectory);
	strcat_s(pcJvmPathBuffer, dwBufferLength, "\\jre");
	evaluateJreDirectory(pcJvmPathBuffer, dwBufferLength);

	if (m_launchConfiguration.getJvmType()==ClientHotspotJVM)
	{
		// in >= jdk 1.4 the client hotspot vm is located in the bin\client directory
		pcEndOfJrePath = pcJvmPathBuffer+strlen(pcJvmPathBuffer);
		strcat_s(pcJvmPathBuffer, dwBufferLength, "\\bin\\client\\jvm.dll");
		// does the dll exists?
		if (_access(pcJvmPathBuffer, 0)!=0)
		{
			*pcEndOfJrePath = '\0';
			// no
			// use the server JRE
			strcat_s(pcJvmPathBuffer, dwBufferLength, "\\bin\\server\\jvm.dll");
		}
	}
	else
	{
		strcat_s(pcJvmPathBuffer, dwBufferLength, "\\bin\\server\\jvm.dll");
	}
	return pcJvmPathBuffer;
}

void CJavaProgram::evaluateJreDirectory(LPSTR pcJvmPathBuffer, DWORD dwBufferLength)
{
	if (_access(pcJvmPathBuffer, MODE_DIRECTORY_EXISTS) == 0)
	{
		//The default jre/ directory exists
		CLog::debug("Using JRE found in: %s", pcJvmPathBuffer);
		return;
	}
	CLog::info("JRE directory does not exist, looking for IVY_JAVA_HOME or JAVA_HOME environment variables.");

	// CHECK IVY_JAVA_HOME
	size_t lengthOfEnvVar;

	getenv_s(&lengthOfEnvVar, pcJvmPathBuffer, dwBufferLength, "IVY_JAVA_HOME");
	if (lengthOfEnvVar != 0)
	{
		CLog::info("IVY_JAVA_HOME is set, using JRE from %s.", pcJvmPathBuffer);
		return;
	}

	// CHECK JAVA_HOME
	getenv_s(&lengthOfEnvVar, pcJvmPathBuffer, dwBufferLength, "JAVA_HOME");

	if (lengthOfEnvVar != 0)
	{
		strcat_s(pcJvmPathBuffer, dwBufferLength, "\\jre");
		// Check if JAVA_HOME contains jre/ folder, if yes, we use this folder
		if (_access(pcJvmPathBuffer, MODE_DIRECTORY_EXISTS) == 0)
		{
			CLog::info("JAVA_HOME is set to JDK, using JRE from %s.", pcJvmPathBuffer);
			return;
		}
		getenv_s(&lengthOfEnvVar, pcJvmPathBuffer, dwBufferLength, "JAVA_HOME");
		CLog::info("JAVA_HOME is set to JRE, using JRE from %s.", pcJvmPathBuffer);
		return;
	}

	throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_FIND_JRE,
						GetLastError(),
						"No JRE has been found. Use an Engine bundled with a JRE or set the IVY_JAVA_HOME or JAVA_HOME environment variable.");
}

void CJavaProgram::callJavaMain(CJavaVirtualMaschine* pJvm, JavaMainArguments& javaMainArguments)
{
	jclass mainClass;
    jmethodID mainMethod;
    jobjectArray mainArguments;
	
	mainClass = getJavaClass(pJvm->getJavaNativeInterface(), m_launchConfiguration.getMainJavaClass());

	mainMethod = getJavaStaticMethod(
		pJvm->getJavaNativeInterface(), 
		mainClass, 
		"main",
		"([Ljava/lang/String;)V");

	logStartingJavaClass(m_launchConfiguration.getMainJavaClass(), javaMainArguments);

	mainArguments = convert2JavaStringArray(pJvm->getJavaNativeInterface(), javaMainArguments.getCount(), javaMainArguments.getArguments());
	
    /* Invoke main method. */
    pJvm->getJavaNativeInterface()->CallStaticVoidMethod(mainClass, mainMethod, mainArguments);
    if (pJvm->getJavaNativeInterface()->ExceptionCheck()) {
		throw CLaunchException(JVMLauncherErrorCodes.EXCEPTION_IN_MAIN_METHOD,
			pJvm->getJavaNativeInterface(),
			"Java main method throws Exception");
    }
}

void CJavaProgram::addOsgiJavaMainArguments(JavaMainArguments* pJavaMainArguments)
{
	if (launchAsOsgiApplication())
	{
		pJavaMainArguments->addArgument("-nl");
		pJavaMainArguments->addArgument("en");
		pJavaMainArguments->addArgument("-application");
		pJavaMainArguments->addArgument((LPSTR) m_launchConfiguration.getOsgiApplicationName());
		if (m_launchConfiguration.isConsole())
		{
			pJavaMainArguments->addArgument("-console");	
		}
	}
}

void CJavaProgram::logStartingJavaClass(LPCSTR pcMainJavaClass, JavaMainArguments& javaMainArguments)
{
	CLog::info("Starting java class '%s'", pcMainJavaClass);
	if (javaMainArguments.getCount() > 0)
	{
		CLog::info("With arguments:");
		for (int pos = 0; pos < javaMainArguments.getCount(); pos++)
		{
			CLog::info(" [%i] %s", pos, javaMainArguments.getArgument(pos));
		}
	}
}

jobjectArray CJavaProgram::convert2JavaStringArray(JNIEnv* pJavaNativeInterface, int argc, LPSTR argv[])
{
    jclass clazz;
    jobjectArray result;
	jstring str;

	clazz = getJavaClass(pJavaNativeInterface, "java.lang.String");
	
	result = pJavaNativeInterface->NewObjectArray(argc, clazz, 0);
	if (result == NULL)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CONVERT_PARAMS,
			pJavaNativeInterface,
			"Could not convert arguments");
	}
    for (int pos = 0; pos < argc; pos++)
	{
		str = convert2JavaString(pJavaNativeInterface, *argv++);
		pJavaNativeInterface->SetObjectArrayElement(result, pos, str);
		pJavaNativeInterface->DeleteLocalRef(str);
    }
    return result;
}

jstring CJavaProgram::convert2JavaString(JNIEnv* pJavaNativeInterface, LPCSTR pcString)
{
    size_t dwLength;
    jclass clazz;
    jmethodID constructorId;
    jbyteArray bytes;
	jstring result;

	assert(pcString != NULL);
	assert(pJavaNativeInterface != NULL);

	dwLength = strlen(pcString);

	clazz = getJavaClass(pJavaNativeInterface, "java.lang.String");

	constructorId = getJavaMethod(pJavaNativeInterface, clazz, "<init>", "([B)V");

	bytes = pJavaNativeInterface->NewByteArray((jsize)dwLength);
    if (bytes == NULL)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CONVERT_PARAMS,
			pJavaNativeInterface,
			"Could not convert argument string");
	}
	pJavaNativeInterface->SetByteArrayRegion(bytes, 0, (jsize)dwLength, (jbyte *)pcString);
	if (pJavaNativeInterface->ExceptionCheck())
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CONVERT_PARAMS,
			pJavaNativeInterface,
			"Could not convert argument string");
	}
	result = (jstring)pJavaNativeInterface->NewObject(clazz, constructorId, bytes);
	if (result == NULL)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_CONVERT_PARAMS,
			pJavaNativeInterface,
			"Could not convert argument string");
	}
	pJavaNativeInterface->DeleteLocalRef(bytes);
	return result;
}

jmethodID CJavaProgram::getJavaStaticMethod(JNIEnv* pJavaNativeInterface, jclass clazz, LPCSTR pcMethodName, LPCSTR pcSignature)
{
	jmethodID methodId;
    methodId = pJavaNativeInterface->GetStaticMethodID(clazz, pcMethodName, pcSignature);
    if (methodId == NULL)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_FIND_METHOD,
			pJavaNativeInterface,
			"Could not find static java method %s", pcMethodName);
    }
	return methodId;
}

jmethodID CJavaProgram::getJavaMethod(JNIEnv* pJavaNativeInterface, jclass clazz, LPCSTR pcMethodName, LPCSTR pcSignature)
{
	jmethodID methodId;
    methodId = pJavaNativeInterface->GetMethodID(clazz, pcMethodName, pcSignature);
    if (methodId == NULL)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_FIND_METHOD,
			pJavaNativeInterface,
			"Could not find java method %s", pcMethodName);
    }
	return methodId;
}

jclass CJavaProgram::getJavaClass(JNIEnv *pJavaNativeInterface, LPCSTR pcName)
{
	assert(pcName != NULL);
	assert(pJavaNativeInterface != NULL);

    char *pcBuffer = new char[strlen(pcName) + 1];
    char *pcDest = pcBuffer;
	char *pcSource = (LPSTR)pcName;
	char ch;
    jclass clazz;

    do {
        ch = *pcSource++;
		*pcDest++ = (ch == '.') ? '/' : ch;
    } while (ch != '\0');
    clazz = pJavaNativeInterface->FindClass(pcBuffer);
    delete pcBuffer;
	if (clazz == NULL)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_LOAD_JAVA_CLASS,
			pJavaNativeInterface,
			"Could not load java class %s", pcName);
	}
    return clazz;
}



CJavaProgram::~CJavaProgram(void)
{}

LPSTR CJavaProgram::getApplicationDirectory(LPSTR pcApplicationDirectoryBuffer, DWORD dwBufferLength)
{
	DWORD result;

	/* Get the full module filename and cut it after the last directory */
    result = GetModuleFileName(NULL, pcApplicationDirectoryBuffer, dwBufferLength);
	if (result == 0)
	{
		throw CLaunchException(JVMLauncherErrorCodes::COULD_NOT_EVALUATE_APPLICATION_DIRECTORY, GetLastError(),
			"Could not evaluate application directory");
	}
	// get current directory out of application file
    *(strrchr(pcApplicationDirectoryBuffer, '\\')) = '\0'; /* remove \xxx.exe */
    *(strrchr(pcApplicationDirectoryBuffer, '\\')) = '\0'; /* remove bin */
	return pcApplicationDirectoryBuffer;
}

void CJavaProgram::initializeVmOptions(CVmOptions& options, LPCSTR pcApplicationDirectory, JavaMainArguments& javaMainArguments)
{
	initializeMemoryOptions(options);	
	initializeClassPathOption(options, pcApplicationDirectory);
	initializeAdditionalVmOptions(options);
	initializeCommandVmOptions(options, javaMainArguments);
	initializeJavaModuleSystemVmOptions(options);
	initializeOsgiVmOptions(options, pcApplicationDirectory);
}

void CJavaProgram::disableManagementOptionForServerStop(int argc, LPSTR argv[])
{	
	if (containsServerStopArgument(argc, argv))
	{
		// TODO Do not load jvm.options
		return;
	}
}

bool CJavaProgram::containsServerStopArgument(int argc, LPSTR argv[])
{
	for (int pos = 0; pos < argc; pos++)
	{
		LPSTR arg = argv[pos];
		if (strncmp(arg, "-", 1)==0)
		{
			arg = arg+1;
		}
		if (strncmp(arg, "-", 1)==0)
		{
			arg = arg+1;
		}
		if (_stricmp(arg, m_launchConfiguration.getServerStopArgument())==0)
		{
			return true;
		}		
	}
	return false;
}

/*
 * This enables JPS and VisualVM to display the main class for the jvm process instance
 */
void CJavaProgram::initializeCommandVmOptions(CVmOptions& options, JavaMainArguments& javaMainArguments)
{
	char pcCommand[10000];

	strcpy_s(pcCommand, 10000, "-Dsun.java.command=");
	strcat_s(pcCommand, 10000, m_launchConfiguration.getMainJavaClass());

	for (int i = 0; i < javaMainArguments.getCount(); i = i + 1) {
		strcat_s(pcCommand, 10000, " ");
		strcat_s(pcCommand, 10000, javaMainArguments.getArgument(i));
	}

	options.addOption(pcCommand, NULL);
}

void CJavaProgram::initializeMemoryOptions(CVmOptions& options)
{
	MEMORYSTATUSEX memInfo;

	// get global memory status of system
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	printMemoryOverview(memInfo);
}

void CJavaProgram::initializeOsgiVmOptions(CVmOptions& options, LPCSTR pcApplicationDirectory)
{
	if (launchAsOsgiApplication())
	{
		char jvmOptionOsgiInstallArea[MAX_PATH];
		strcpy_s(jvmOptionOsgiInstallArea, MAX_PATH, "-Dosgi.install.area=");
		strcat_s(jvmOptionOsgiInstallArea, MAX_PATH, pcApplicationDirectory);
		strcat_s(jvmOptionOsgiInstallArea, MAX_PATH, "\\system");
		options.addOption(jvmOptionOsgiInstallArea, NULL);

		char jvmOptionInstanceArea[MAX_PATH];
		strcpy_s(jvmOptionInstanceArea, MAX_PATH, "-Dosgi.instance.area=");
		strcat_s(jvmOptionInstanceArea, MAX_PATH, pcApplicationDirectory);
		strcat_s(jvmOptionInstanceArea, MAX_PATH, "\\system\\workspace");
		options.addOption(jvmOptionInstanceArea, NULL);
	}
}

void CJavaProgram::initializeJavaModuleSystemVmOptions(CVmOptions& options)
{
	// ignore illegal reflective access warning at startup because guice
	options.addOption("--add-opens=java.base/java.lang=ALL-UNNAMED", NULL);

	// allow ZipFileSystem readonly feature on engine with Java 11
	options.addOption("--add-opens=jdk.zipfs/jdk.nio.zipfs=ALL-UNNAMED", NULL);
}

void CJavaProgram::printMemoryOverview(MEMORYSTATUSEX& memInfo)
{
	CLog::info("Memory Overview:");
	CLog::info("  Memory load:               %lu%%", memInfo.dwMemoryLoad);
	CLog::info("  Total Physical Memory:     %I64u kBytes", memInfo.ullTotalPhys/1024);
	CLog::info("  Available Physical Memory: %I64u kBytes", memInfo.ullAvailPhys/1024);
	CLog::info("  Total Virtual Memory:      %I64u kBytes", memInfo.ullTotalVirtual/1024);
	CLog::info("  Available Virtual Memory:  %I64u kBytes", memInfo.ullAvailVirtual/1024);
	CLog::info("  Total Page File:           %I64u kBytes", memInfo.ullTotalPageFile/1024);
	CLog::info("  Available Page File:       %I64u kBytes", memInfo.ullAvailPageFile/1024);
}

void CJavaProgram::addToClasspath(LPSTR& pcClasspath, DWORD& dwClasspathLength, LPCSTR pcPathToAdd, LPCSTR pcFileToAdd)
{
	LPSTR pcNewClasspath;
	DWORD dwNeededPlace;

	dwNeededPlace = 1+1+strlen(pcPathToAdd);  // 1 for ; and 1 for the ending zero
	if (pcFileToAdd != NULL)
	{
		dwNeededPlace = dwNeededPlace+strlen(pcFileToAdd); 
		CLog::info("  Adding '%s'", pcFileToAdd);
	}
	else
	{
		CLog::info("Adding '%s' directory to classpath", pcPathToAdd);
	}

	if ((strlen(pcClasspath)+dwNeededPlace) > dwClasspathLength)
	{
		CLog::info("Expanding classpath buffer form {%d}", dwClasspathLength);
		pcNewClasspath = new char[dwClasspathLength*2];
		strcpy_s(pcNewClasspath, dwClasspathLength, pcClasspath);
		dwClasspathLength = dwClasspathLength*2;
		delete pcClasspath;
		pcClasspath = pcNewClasspath;
	}
	strcat_s(pcClasspath, dwClasspathLength, pcPathToAdd);
	if (pcFileToAdd != NULL)
	{
		strcat_s(pcClasspath, dwClasspathLength, pcFileToAdd);
	}
	strcat_s(pcClasspath, dwClasspathLength, ";");
}

void CJavaProgram::initializeClassPathOption(CVmOptions& options, LPCSTR pcApplicationDirectory)
{
	char pcClassesDir[MAX_PATH];
	DWORD maxClassPathLength = MAX_CLASSPATH_LENGTH;
	LPSTR pcClassPath = new char[MAX_CLASSPATH_LENGTH];

	/* Prepare a string which takes the classpath */
    strcpy_s(pcClassPath, maxClassPathLength, "-Djava.class.path=");

	if (launchAsOsgiApplication())
	{
		CLog::debug("Start OSGI application: %s", m_launchConfiguration.getOsgiApplicationName());
		addJarsToClasspath(pcClassPath, maxClassPathLength, pcApplicationDirectory, "system\\lib\\boot");
	}
	else
	{		
		/* Add the classes directory first */
		strcpy_s(pcClassesDir, MAX_PATH, pcApplicationDirectory);
		strcat_s(pcClassesDir, MAX_PATH, "\\lib\\classes");

		addToClasspath(pcClassPath, maxClassPathLength, pcClassesDir, NULL);

		// Add jars in patch directory
		addJarsToClasspath(pcClassPath, maxClassPathLength, pcApplicationDirectory, "lib\\patch");
		// Add jars in shared directory
		addJarsToClasspath(pcClassPath, maxClassPathLength, pcApplicationDirectory, "lib\\shared");		
	}

	options.addOption(pcClassPath, NULL);
}

void CJavaProgram::addJarsToClasspath(LPSTR& pcClasspath, DWORD& dwClasspathLength, LPCSTR pcApplicationDirectory, LPCSTR pcRelativeLibDirectory)
{
	char pcLibDirectory[MAX_PATH];
	char pcJarFileFilter[MAX_PATH];
    struct _finddata_t jar_file;    
	intptr_t hFile;
	LPSTR* ppcJarFiles;
	DWORD dwCount=0;
	DWORD index = 0;

	/* Create the library directory */
	strcpy_s(pcLibDirectory, MAX_PATH, pcApplicationDirectory);
	strcat_s(pcLibDirectory, "\\");
	strcat_s(pcLibDirectory, MAX_PATH, pcRelativeLibDirectory);

	CLog::info("Adding jars from '%s' to classpath:", pcLibDirectory);

	strcat_s(pcLibDirectory, MAX_PATH, "\\");
	
	/* Create the *.jar search string */
	strcpy_s(pcJarFileFilter, MAX_PATH,pcLibDirectory);
	strcat_s(pcJarFileFilter, MAX_PATH, "*.jar");

	/* count the *.jar files in the lib directory */
    hFile = _findfirst( pcJarFileFilter, &jar_file );
	if (hFile != -1L )
	{
		dwCount++;
		while (_findnext(hFile, &jar_file) == 0)
		{
			dwCount++;
		}
		_findclose( hFile );
	}

	// allocate memory to hold jar file names
	ppcJarFiles = new char*[dwCount];
	for (DWORD dwPos = 0; dwPos < dwCount; dwPos++)
	{
		ppcJarFiles[dwPos] = new char[MAX_PATH];
	}

	/* find the *jar files in the lib directory */
    hFile = _findfirst( pcJarFileFilter, &jar_file );
	if (hFile != -1L)
	{
		strcpy_s(ppcJarFiles[index++], MAX_PATH, jar_file.name);		
        while(( _findnext( hFile, &jar_file ) == 0 )&&(index < dwCount))            
		{
			strcpy_s(ppcJarFiles[index++], MAX_PATH, jar_file.name);
        }
		_findclose( hFile );
	}

	// ReW 18.4.2002 sort the jar files so that order in the classpath is 
	// order by the file name
	qsort(ppcJarFiles, dwCount, sizeof(char*), &compareFileName);
	
	// Add to classpath
	for(DWORD dwPos = 0; dwPos < dwCount; dwPos++)
	{
		addToClasspath(pcClasspath, dwClasspathLength, pcLibDirectory, ppcJarFiles[dwPos]);
	}
	// release allocated memory
	for (DWORD dwPos = 0; dwPos < dwCount; dwPos++)
	{
		delete ppcJarFiles[dwPos];
	}
	delete ppcJarFiles;
};

void CJavaProgram::initializeAdditionalVmOptions(CVmOptions& options)
{
	LPCSTR pcAdditionalVmOptions;
	LPCSTR pcFound;

	pcAdditionalVmOptions = m_launchConfiguration.getAdditionalVmOptions();
	if (pcAdditionalVmOptions != NULL)
	{
		pcFound = strchr(pcAdditionalVmOptions, ' ');
		while (pcFound != NULL)
		{
			if (pcFound-pcAdditionalVmOptions>0)
			{
				options.addOption(pcAdditionalVmOptions, pcFound, NULL);
			}
			pcAdditionalVmOptions = pcFound+1;
			pcFound = strchr(pcAdditionalVmOptions, ' ');
		}
		if (strlen(pcAdditionalVmOptions)>0)
		{
			options.addOption(pcAdditionalVmOptions, NULL);
		}
	}
}

/*
 * Reports an error to the system event log
 * @param ex the exception to report
 */ 
void CJavaProgram::reportError(CLaunchException ex)
{
	HANDLE hEventSource;
	char pcEventSourceName[256];
	LPCSTR pcMessage;
	CLog::debug("Report error to windows event log"); 

	if (m_launchConfiguration.getApplicationName() != NULL)
	{
		strcpy_s(pcEventSourceName, 256, m_launchConfiguration.getApplicationName());
		strcat_s(pcEventSourceName, 256, " Launchers");
	}
	else if (m_launchConfiguration.getWindowsServiceName() != NULL)
	{
		strcpy_s(pcEventSourceName, 256, m_launchConfiguration.getWindowsServiceName());
		strcat_s(pcEventSourceName, 256, " Service Launcher");
	}
	else
	{
		strcpy_s(pcEventSourceName, 256, "JVMLauncher");
	}
	registerEventSource(pcEventSourceName);
	hEventSource = RegisterEventSource(NULL, pcEventSourceName);
	if (hEventSource == NULL)
	{
		CLog::error("Could not register an event source (windows error: 0x%08X)", GetLastError());
		return;
	}

	pcMessage = ex.getMessage();
	if (!ReportEvent(
		hEventSource, 
		EVENTLOG_ERROR_TYPE, 
		0, 
		ex.getErrorCode(), 
		NULL, 
		1,  
		0, 
		(LPCSTR*)&pcMessage, 
		NULL))
	{
		CLog::error("Could not report event (windows error: 0x%08X)", GetLastError());	
	}
	DeregisterEventSource(hEventSource);
}

void CJavaProgram::registerEventSource(LPCSTR pcEventSourceName)
{
  LPCSTR prefix = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\";
  DWORD disposition;
  HKEY hKey = 0;
  char subkey[256];
  DWORD value;
  
  strcpy_s(subkey, 256, prefix);
  strcat_s(subkey, 256, pcEventSourceName);
 
  CLog::info("Create registry entry '%s' for event log if it does not exists", subkey);

  if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL,
	  &hKey, &disposition) != ERROR_SUCCESS)
  {
	  CLog::error("Could not create registry entry %s", subkey);
	  return;
  }
  		
  if (disposition == REG_CREATED_NEW_KEY)
  {
    HMODULE hmodule = GetModuleHandle("JVMLauncher.dll");
    if (hmodule == NULL) 
	{
		CLog::error("Could not evaluate module handle");
		RegCloseKey(hKey);
		return;
	}
    char modpath[_MAX_PATH];
    DWORD modlen = GetModuleFileName(hmodule, modpath, _MAX_PATH - 1);
    if (modlen > 0) {
        modpath[modlen] = 0;
		if (RegSetValueEx(hKey, "EventMessageFile", 0, REG_EXPAND_SZ, 
			(LPBYTE)modpath, (strlen(modpath) + 1)) != ERROR_SUCCESS)
		{
			CLog::error("Could not set registry value EventMessageFile");
		}
		if (RegSetValueEx(hKey, "CategoryMessageFile", 0, REG_EXPAND_SZ, 
			(LPBYTE)modpath, (strlen(modpath) + 1)) != ERROR_SUCCESS)
		{
			CLog::error("Could not set registry value CategoryMessageFile");
		}
    }
	value=7;
	if (RegSetValueEx(hKey, "TypesSupported", 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD))!= ERROR_SUCCESS)
	{
		CLog::error("Could not set registry value TypesSupported");
	}
	value=6;
	if (RegSetValueEx(hKey, "CategoryCount", 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD)) != ERROR_SUCCESS)
	{
		CLog::error("Could not set registry value CategoryCount");
	}
  }
  RegCloseKey(hKey);
  return;
}
