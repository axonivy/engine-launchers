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
#include <assert.h>
#include <io.h>

#define MAX_CLASSPATH_LENGTH 20000

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

CJavaProgram* CJavaProgram::createJavaProgram(LPCSTR pcMainJavaClass)
{
	CLaunchConfiguration defaultLaunchConfiguration;

	assert(pcMainJavaClass != NULL);
	defaultLaunchConfiguration.setMainJavaClass(pcMainJavaClass);
	return new CJavaProgram(defaultLaunchConfiguration);
}

CJavaProgram* CJavaProgram::createJavaProgram()
{
	return new CJavaProgram(
		CLaunchConfigurationFile::getForExecutable().readLaunchConfiguration());
}

CJavaProgram* CJavaProgram::createJavaProgram(CLaunchConfigurationFile launchConfigurationFile)
{
	return new CJavaProgram(launchConfigurationFile.readLaunchConfiguration());
}

CJavaProgram* CJavaProgram::createJavaProgram(CLaunchConfiguration launchConfiguration)
{
	return new CJavaProgram(launchConfiguration);
}

CJavaProgram* CJavaProgram::createJavaProgram(CLaunchConfiguration defaultLaunchConfiguration, bool readLaunchConfigurationFileIfExists)
{
	if (readLaunchConfigurationFileIfExists)
	{
		CLaunchConfigurationFile launchConfigurationFile;

		launchConfigurationFile = CLaunchConfigurationFile::getForExecutable();
		if (launchConfigurationFile.exists())
		{
			CLaunchConfiguration launchConfigurationFromFile, launchConfiguration;

			launchConfigurationFromFile = launchConfigurationFile.readLaunchConfiguration();
			launchConfiguration = defaultLaunchConfiguration.overwrite(launchConfigurationFromFile);
			return createJavaProgram(launchConfiguration);
		}
		else
		{
			return createJavaProgram(defaultLaunchConfiguration);
		}
	}
	else
	{
		return createJavaProgram(defaultLaunchConfiguration);
	}
}

CJavaProgram* CJavaProgram::createJavaProgram(CLaunchConfiguration defaultLaunchConfiguration, CLaunchConfigurationFile launchConfigurationFile)
{
	CLaunchConfiguration launchConfiguration;

	launchConfiguration = defaultLaunchConfiguration.overwrite(launchConfigurationFile.readLaunchConfiguration());
	return createJavaProgram(launchConfiguration);
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


void CJavaProgram::main(int argc, LPSTR argv[])
{
	CJavaVirtualMaschine* pJvm;
	CVmOptions vmOptions;
	char pcApplicationDirectory[MAX_PATH];
	char pcJvmPath[MAX_PATH];

	if ((m_launchConfiguration.isSingleton())&&(m_launchConfiguration.getApplicationName()!=NULL))
	{
		if (checkIsAlreadyRunning())
		{
			CLog::info("Program already running");
			return;
		}
	}

	CLog::info("=============================================================");
	CLog::info("Starting java programm in class");
	CLog::info("%s", m_launchConfiguration.getMainJavaClass());
	CLog::info("with ivyteam's java launcher"); 
	CLog::info("=============================================================");

	getApplicationDirectory(pcApplicationDirectory, MAX_PATH);
	
	CLog::info("Setting current directory to '%s'.", pcApplicationDirectory);
	/* Set the current directory */
	SetCurrentDirectory(pcApplicationDirectory);

	initializeVmOptions(vmOptions, pcApplicationDirectory);

	pJvm = CJavaVirtualMaschine::createJavaVirtualMaschine(
		getJvmPath(pcApplicationDirectory, pcJvmPath, MAX_PATH), 
		vmOptions);

	try
	{
		// call java main
		callJavaMain(pJvm, argc, argv);
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

	// Prepare the jre and the JVM path
	if (m_launchConfiguration.getJreDirectory()!=NULL)
	{
		if (strchr(m_launchConfiguration.getJreDirectory(), ':')!=NULL)  //c:\, d:\ etc
		{
			// absolute path
			strcpy_s(pcJvmPathBuffer, dwBufferLength, m_launchConfiguration.getJreDirectory());
		}
		else
		{
			// relative path
			strcpy_s(pcJvmPathBuffer, dwBufferLength, pcApplicationDirectory);
			strcat_s(pcJvmPathBuffer, dwBufferLength, "\\");
			strcat_s(pcJvmPathBuffer, dwBufferLength, m_launchConfiguration.getJreDirectory());
		}
	}
	else
	{
		// default path
		strcpy_s(pcJvmPathBuffer, dwBufferLength, pcApplicationDirectory);
		strcat_s(pcJvmPathBuffer, dwBufferLength, "\\jre");
	}
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
			// in < jdk 1.4 the client hotspot vm is located in the bin\hotspot directory
			strcat_s(pcJvmPathBuffer, dwBufferLength, "\\bin\\hotspot\\jvm.dll");
		}
	}
	else if (m_launchConfiguration.getJvmType()==ServerHotspotJVM)
	{
		strcat_s(pcJvmPathBuffer, dwBufferLength, "\\bin\\server\\jvm.dll");
	}
	else
	{
		pcEndOfJrePath = pcJvmPathBuffer+strlen(pcJvmPathBuffer);
		strcat_s(pcJvmPathBuffer, dwBufferLength, "\\bin\\classic\\jvm.dll");
		if (_access(pcJvmPathBuffer, 0)!=0)
		{
			*pcEndOfJrePath = '\0';
			// no
			// in > jdk 1.4 the default vm is the hotspot vm
			strcat_s(pcJvmPathBuffer, dwBufferLength, "\\bin\\client\\jvm.dll");
		}
	}
	return pcJvmPathBuffer;
}

void CJavaProgram::callJavaMain(CJavaVirtualMaschine* pJvm, int argc, LPSTR argv[])
{
	jclass mainClass;
    jmethodID mainMethod;
    jobjectArray mainArguments;
	
	mainClass = getJavaClass(pJvm->getJavaNativeInterface(), m_launchConfiguration.getMainJavaClass());

	mainMethod = getJavaStaticMethod(
		pJvm->getJavaNativeInterface(), 
		mainClass, 
		(m_launchConfiguration.getMainJavaMethod()==NULL) ? "main" : m_launchConfiguration.getMainJavaMethod(),
		"([Ljava/lang/String;)V");
	
    /* Build argument array */

	if (argc>0)
	{
		CLog::info("Starting java class '%s' with arguments:", m_launchConfiguration.getMainJavaClass() );
	}
	else
	{
		CLog::info("Starting java class '%s'", m_launchConfiguration.getMainJavaClass() );
	}
	for (int pos = 0; pos < argc; pos++)
	{
		CLog::info(" [%i] %s", pos, argv[pos]);
	}
	mainArguments = convert2JavaStringArray(pJvm->getJavaNativeInterface(), argc, argv);

    /* Invoke main method. */
    pJvm->getJavaNativeInterface()->CallStaticVoidMethod(mainClass, mainMethod, mainArguments);
    if (pJvm->getJavaNativeInterface()->ExceptionCheck()) {
		throw CLaunchException(JVMLauncherErrorCodes.EXCEPTION_IN_MAIN_METHOD,
			pJvm->getJavaNativeInterface(),
			"Java main method throws Exception");
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
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_FIND_MAIN_METHOD,
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
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_FIND_MAIN_METHOD,
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

void CJavaProgram::initializeVmOptions(CVmOptions& options, LPCSTR pcApplicationDirectory)
{
	initializeMemoryOptions(options);
	initializeGarbageCollectorOptions(options);
	initializeClassPathOption(options, pcApplicationDirectory);
	initializeAdditionalVmOptions(options);
}

void CJavaProgram::initializeMemoryOptions(CVmOptions& options)
{
	DWORD dwHeapMaxSize;
	DWORD dwHeapStartSize;
	DWORD dwHeapFreeMaxRatio;
	DWORD dwHeapYoungMaxSize;
	MEMORYSTATUSEX memInfo;

	// get global memory status of system
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	printMemoryOverview(memInfo);

	// initialize heap max memory
	dwHeapMaxSize = initializeHeapMaxSizeMemoryOption(memInfo, options);

	// initialize heap start memory
	dwHeapStartSize = initializeHeapStartSizeMemoryOption(memInfo, dwHeapMaxSize, options);

	// initialize heap free max ratio 
	dwHeapFreeMaxRatio = initializeHeapFreeMaxRatioMemoryOption(options);

	// initialize heap free min ratio 
	initializeHeapFreeMinRatioMemoryOption(dwHeapFreeMaxRatio, options);

	// initialize heap young max size
	dwHeapYoungMaxSize = initializeHeapYoungMaxSizeMemoryOption(dwHeapMaxSize, options);
	
	// initialize heap young min size
	initializeHeapYoungMinSizeMemoryOption(dwHeapYoungMaxSize, dwHeapStartSize,  options);

	// initialize heap eden survivor ratio
	initializeHeapEdenSurvivorRatioMemoryOption(options);

	// initialize heap tenured young ratio
	initializeHeapTenuredYoungRatioMemoryOption(options);

	// initialize heap max perm size
	initializeHeapPermMaxSizeMemoryOption(dwHeapMaxSize, options);
}

DWORD CJavaProgram::initializeHeapMaxSizeMemoryOption(MEMORYSTATUSEX& memInfo, CVmOptions& options)
{
	DWORD dwMaxMemory;
	DWORD dwTotalPhysicalMemory;
	DWORD dwLimitMemory;
	char pcOption[100];

	if (m_launchConfiguration.getHeapMaxSize() != CLaunchConfiguration::DISABLED)
	{
		dwMaxMemory = m_launchConfiguration.getHeapMaxSize();
	}
	else if (m_launchConfiguration.getHeapMaxRatio() != CLaunchConfiguration::DISABLED)
	{
		dwTotalPhysicalMemory = static_cast<DWORD>(memInfo.ullTotalPhys/1024/1024+1);
		dwMaxMemory = m_launchConfiguration.getHeapMaxRatio()*dwTotalPhysicalMemory/100;
		// Bug use dwAvailPageFile instaed of dwTotalPageFile
		dwLimitMemory = static_cast<DWORD>(memInfo.ullAvailPageFile / 1024 /1024 + 1);
		
#ifndef WIN64 //  limit max memory only on 32 bit windows systems		
		if (m_launchConfiguration.getJvmType() == ClientHotspotJVM)
		{
			// With the client jvm the limit is smaller than with the server jvm. Limit max mem to 1 GByte.
			// See Bug #2648.
			if (dwLimitMemory > 1000)
			{
				dwLimitMemory = 1000;
			}
		}
		else
		{
			// ReW 15.10.2003
			// does not seem to work with large values -> limit to 1.2 GByte
			// ReW 30.3.2005 Spezial version for SWM with 1.45 GByte because they 
			// have the 3GB Switch of Windows Server 2000 activated
			if (dwLimitMemory > 1200)
			{
				dwLimitMemory = 1200;
			}
		} 
#endif

		// ReW 2.5.2001
		// Max memory must be lower than total available virtual memory in system, 
		// otherwise java would not start. 
		if (dwMaxMemory >= dwLimitMemory)
		{
			dwMaxMemory = dwLimitMemory;
		}
	}
	else
	{
		return CLaunchConfiguration::DISABLED;
	}

	sprintf_s(pcOption, 100, "-Xmx%lum", dwMaxMemory);
	options.addOption(pcOption, NULL);
	return dwMaxMemory;
}

DWORD CJavaProgram::initializeHeapStartSizeMemoryOption(MEMORYSTATUSEX& memInfo, DWORD dwMaxHeapMemory, CVmOptions& options)
{
	DWORD dwHeapStartMemory;
	char pcOption[100];

	if (m_launchConfiguration.getHeapStartSize()!= CLaunchConfiguration::DISABLED)
	{
		dwHeapStartMemory = m_launchConfiguration.getHeapStartSize();
		if ((dwMaxHeapMemory != CLaunchConfiguration::DISABLED)&&
			(dwHeapStartMemory > dwMaxHeapMemory))
		{
			dwHeapStartMemory = dwMaxHeapMemory;
			CLog::warn("ivy.heap.start.size was higher than ivy.heap.max.size. Set it to same value as ivy.heap.max.size");
		}
		sprintf_s(pcOption, 100, "-Xms%lum", dwHeapStartMemory);
		options.addOption(pcOption, NULL);
		return dwHeapStartMemory;
	}
	else
	{
		return CLaunchConfiguration::DISABLED;
	}
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

void CJavaProgram::initializeGarbageCollectorOptions(CVmOptions& options)
{
	LPCSTR pcGarbageCollectorOptions;
	LPCSTR pcFound;

	pcGarbageCollectorOptions = m_launchConfiguration.getGarbageCollectorOptions();
	if (pcGarbageCollectorOptions != NULL)
	{
		pcFound = strchr(pcGarbageCollectorOptions, ' ');
		while (pcFound != NULL)
		{
			if (pcFound-pcGarbageCollectorOptions>0)
			{
				options.addOption(pcGarbageCollectorOptions, pcFound, NULL);
			}
			pcGarbageCollectorOptions = pcFound+1;
			pcFound = strchr(pcGarbageCollectorOptions, ' ');
		}
		if (strlen(pcGarbageCollectorOptions)>0)
		{
			options.addOption(pcGarbageCollectorOptions, NULL);
		}
	}
}

void CJavaProgram::initializeClassPathOption(CVmOptions& options, LPCSTR pcApplicationDirectory)
{
	char pcClassesDir[MAX_PATH];
	char pcClassPath[MAX_CLASSPATH_LENGTH];
	LPCSTR pcAuxDirectory;

	/* Prepare a string which takes the classpath */
    strcpy_s(pcClassPath, MAX_CLASSPATH_LENGTH, "-Djava.class.path=");

	/* Add the classes directory first */
	strcpy_s(pcClassesDir, MAX_PATH, pcApplicationDirectory);
	strcat_s(pcClassesDir, MAX_PATH, "\\lib\\classes");
	CLog::info("Adding '%s' directory to classpath", pcClassesDir);
	strcat_s(pcClassPath, MAX_CLASSPATH_LENGTH, pcClassesDir); 
	strcat_s(pcClassPath, MAX_CLASSPATH_LENGTH, ";"); 

	// Add jars in patch directory
	addJarsToClasspath(pcClassPath, MAX_CLASSPATH_LENGTH, pcApplicationDirectory, "lib\\patch");
	// Add jars in shared directory
	addJarsToClasspath(pcClassPath, MAX_CLASSPATH_LENGTH, pcApplicationDirectory, "lib\\shared");
	// Add jars in aux directory
	pcAuxDirectory = m_launchConfiguration.getAuxDirectory();
	if (pcAuxDirectory == NULL)
	{
		pcAuxDirectory = "lib\\ivy";
	}
	addJarsToClasspath(pcClassPath, MAX_CLASSPATH_LENGTH, pcApplicationDirectory, pcAuxDirectory);

	options.addOption(pcClassPath, NULL);
}

void CJavaProgram::addJarsToClasspath(LPSTR pcClasspath, DWORD dwClasspathLength, LPCSTR pcApplicationDirectory, LPCSTR pcRelativeLibDirectory)
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
		strcat_s(pcClasspath, dwClasspathLength, pcLibDirectory);
		strcat_s(pcClasspath, dwClasspathLength, ppcJarFiles[dwPos]);
		strcat_s(pcClasspath, dwClasspathLength, ";");
		CLog::info("  Adding %s", ppcJarFiles[dwPos]);
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

DWORD CJavaProgram::initializeHeapFreeMaxRatioMemoryOption(CVmOptions& options)
{
	DWORD dwHeapFreeMaxRatio;
	char pcOption[100];

	if (m_launchConfiguration.getHeapMaxFreeRatio()!= CLaunchConfiguration::DISABLED)
	{
		dwHeapFreeMaxRatio = m_launchConfiguration.getHeapMaxFreeRatio();
		sprintf_s(pcOption, 100, "-XX:MaxHeapFreeRatio=%lu", dwHeapFreeMaxRatio);
		options.addOption(pcOption, NULL);
		return dwHeapFreeMaxRatio;
	}
	else
	{
		return CLaunchConfiguration::DISABLED;
	}
}

void CJavaProgram::initializeHeapFreeMinRatioMemoryOption(DWORD dwHeapFreeMaxRatio, CVmOptions& options)
{
	DWORD dwHeapFreeMinRatio;
	char pcOption[100];

	if (m_launchConfiguration.getHeapMinFreeRatio()!= CLaunchConfiguration::DISABLED)
	{
		dwHeapFreeMinRatio = m_launchConfiguration.getHeapMinFreeRatio();
		if (dwHeapFreeMinRatio > dwHeapFreeMaxRatio)
		{
			dwHeapFreeMinRatio = dwHeapFreeMaxRatio;
			CLog::warn("ivy.heap.free.min.ratio was higher than ivy.heap.free.max.ratio. Set it to same value as ivy.heap.free.max.ratio");
		}
		sprintf_s(pcOption, 100, "-XX:MinHeapFreeRatio=%lu", dwHeapFreeMinRatio);
		options.addOption(pcOption, NULL);
	}
}

DWORD CJavaProgram::initializeHeapYoungMaxSizeMemoryOption(DWORD dwHeapMaxSize, CVmOptions& options)
{
	DWORD dwHeapYoungMaxSize;
	char pcOption[100];

	if (m_launchConfiguration.getHeapMaxYoungSize()!= CLaunchConfiguration::DISABLED)
	{
		dwHeapYoungMaxSize = m_launchConfiguration.getHeapMaxYoungSize();
		if (dwHeapYoungMaxSize > dwHeapMaxSize)
		{
			dwHeapYoungMaxSize = dwHeapMaxSize;
			CLog::warn("ivy.heap.young.max.size was higher than ivy.heap.max.size. Set it to same value as ivy.heap.max.size");

		}
		sprintf_s(pcOption, 100, "-XX:MaxNewSize=%lum", dwHeapYoungMaxSize);
		options.addOption(pcOption, NULL);
		return dwHeapYoungMaxSize;
	}
	else
	{
		return CLaunchConfiguration::DISABLED;
	}
}

void CJavaProgram::initializeHeapYoungMinSizeMemoryOption(DWORD dwHeapYoungMaxSize, DWORD dwHeapStartSize, CVmOptions& options)
{
	DWORD dwHeapYoungMinSize;
	char pcOption[100];

	if (m_launchConfiguration.getHeapMinYoungSize()!= CLaunchConfiguration::DISABLED)
	{
		dwHeapYoungMinSize = m_launchConfiguration.getHeapMinYoungSize();
		if (dwHeapYoungMinSize > dwHeapYoungMaxSize)
		{
			dwHeapYoungMinSize = dwHeapYoungMaxSize;
			CLog::warn("ivy.heap.young.min.size was higher than ivy.heap.young.max.size. Set it to same value as ivy.heap.young.max.size");
		}
		sprintf_s(pcOption, 100, "-XX:NewSize=%lum", dwHeapYoungMinSize);
		options.addOption(pcOption, NULL);
	}
}

void CJavaProgram::initializeHeapEdenSurvivorRatioMemoryOption( CVmOptions& options)
{
	DWORD dwHeapEdenSurvivorRatio;
	char pcOption[100];

	if (m_launchConfiguration.getHeapEdenToSurvivorRatio()!= CLaunchConfiguration::DISABLED)
	{
		dwHeapEdenSurvivorRatio = m_launchConfiguration.getHeapEdenToSurvivorRatio();
		sprintf_s(pcOption, 100, "-XX:SurvivorRatio=%lu", dwHeapEdenSurvivorRatio);
		options.addOption(pcOption, NULL);
	}
}

void CJavaProgram::initializeHeapTenuredYoungRatioMemoryOption( CVmOptions& options)
{
	DWORD dwHeapTenuredYoungRatio;
	char pcOption[100];

	if (m_launchConfiguration.getHeapTenuredToYoungRatio()!= CLaunchConfiguration::DISABLED)
	{
		dwHeapTenuredYoungRatio = m_launchConfiguration.getHeapTenuredToYoungRatio();
		sprintf_s(pcOption, 100, "-XX:NewRatio=%lu", dwHeapTenuredYoungRatio);
		options.addOption(pcOption, NULL);
	}
}

void CJavaProgram::initializeHeapPermMaxSizeMemoryOption(DWORD dwHeapMaxSize, CVmOptions& options)
{
	DWORD dwHeapPermMaxSize;
	char pcOption[100];

	if (m_launchConfiguration.getHeapMaxPermSize()!= CLaunchConfiguration::DISABLED)
	{
		dwHeapPermMaxSize = m_launchConfiguration.getHeapMaxPermSize();
		if (dwHeapPermMaxSize > dwHeapMaxSize)
		{
			dwHeapPermMaxSize = dwHeapMaxSize;
			CLog::warn("ivy.heap.perm.max.size was higher than ivy.heap.max.size. Set it to same value as ivy.heap.max.size");
		}
		sprintf_s(pcOption, 100, "-XX:MaxPermSize=%lum", dwHeapPermMaxSize);
		options.addOption(pcOption, NULL);
	}
}