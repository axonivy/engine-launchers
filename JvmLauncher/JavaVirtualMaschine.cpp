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
#define JVMLauncher

#include "JavaVirtualMaschine.h"
#include "LaunchException.h"
#include "Log.h"
#include <assert.h>

#define MAX_ENV_VALUE 32767


/**
 * Pointers to the needed JNI invocation API, initialized by LoadJavaVM.
 */
typedef jint (JNICALL *CreateJavaVM_t)(JavaVM **pvm, void **env, void *args);
typedef jint (JNICALL *GetDefaultJavaVMInitArgs_t)(void *args);

struct JavaVirtualMaschineInvocationFunctions
{
    CreateJavaVM_t pFctCreateJavaVM;
    GetDefaultJavaVMInitArgs_t pFctGetDefaultJavaVMInitArgs;
};

CJavaVirtualMaschine::CJavaVirtualMaschine(JavaVM* pJavaVirtualMaschine, JNIEnv* pJavaNativeInterface)
: m_pJavaVirtualMaschine(pJavaVirtualMaschine), m_pJavaNativeInterface(pJavaNativeInterface)
{
}

CJavaVirtualMaschine::~CJavaVirtualMaschine(void)
{
	m_pJavaNativeInterface = NULL;
	m_pJavaVirtualMaschine = NULL;
}

CJavaVirtualMaschine* CJavaVirtualMaschine::createJavaVirtualMaschine(
		LPCSTR pcJvmPath, 
		CVmOptions &options)
{
	JavaVirtualMaschineInvocationFunctions vmInvocationFunctions;
    JavaVMInitArgs args;
	jint result;
	JavaVM* pJavaVirtualMaschine;
	JNIEnv* pJavaNativeInterface;
	LPSTR pcNextPath;
	LPSTR pcLastPath;
    
	// Load and initialize the JVM
    vmInvocationFunctions.pFctCreateJavaVM = NULL; 
	vmInvocationFunctions.pFctGetDefaultJavaVMInitArgs = NULL;

	addJreBinToEnvPath(pcJvmPath);

	CLog::info("Loading java virtual maschine %s", pcJvmPath);

	loadJavaVirtualMaschineLibrary(pcJvmPath, &vmInvocationFunctions);

    memset(&args, 0, sizeof(args));
    args.version  = JNI_VERSION_1_2;
    args.nOptions = options.getCount();
    args.options  = options.getOptions();
    args.ignoreUnrecognized = JNI_FALSE;

	CLog::info("Starting java virtual maschine with options:");
	for (DWORD i = 0; i < options.getCount(); i++)
	{
		if (strstr(options.getOptions()[i].optionString, "-Djava.class.path=")==options.getOptions()[i].optionString)
		{
			pcLastPath = options.getOptions()[i].optionString;
			pcNextPath = strchr(pcLastPath, ';');
			while (pcNextPath != NULL)
			{
				*pcNextPath = '\0';
				if (pcLastPath == options.getOptions()[i].optionString)
				{
					CLog::info("  %s;", pcLastPath);
				}
				else
				{
					CLog::info("    %s;", pcLastPath);
				}
				*pcNextPath = ';';
				pcLastPath = pcNextPath+1;
				pcNextPath = strchr(pcLastPath, ';');
			}
		}
		else
		{
			CLog::info("  %s", options.getOptions()[i].optionString);
		}
	}

    result = vmInvocationFunctions.pFctCreateJavaVM(&pJavaVirtualMaschine, (void **)&pJavaNativeInterface, &args);
	if (result != 0)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_INIT_JVM, 
			"Could not initialize java virtual maschine (%ld)", result);
	}
	return new CJavaVirtualMaschine(pJavaVirtualMaschine, pJavaNativeInterface);
}

void CJavaVirtualMaschine::loadJavaVirtualMaschineLibrary(LPCSTR pcJvmPath, JavaVirtualMaschineInvocationFunctions *pVmInvocationFunctions)
{
    HINSTANCE handle;

    /* Load the Java VM DLL */
	handle = LoadLibrary(pcJvmPath);
    if (handle == 0) 
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_LOAD_JVM, GetLastError(),
			"Could not load java virtual maschine library '%s'", pcJvmPath);
    }

    /* Now get the function addresses */
    pVmInvocationFunctions->pFctCreateJavaVM =
		(CreateJavaVM_t)GetProcAddress(handle, "JNI_CreateJavaVM");
	if(pVmInvocationFunctions->pFctCreateJavaVM == NULL)
	{
		FreeLibrary(handle);
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_LOAD_JVM, GetLastError(),
			"Can not find address of function JNI_CreateJavaVM in java virtual maschine library %s", pcJvmPath);
	}
	
    pVmInvocationFunctions->pFctGetDefaultJavaVMInitArgs =
		(GetDefaultJavaVMInitArgs_t)GetProcAddress(handle, "JNI_GetDefaultJavaVMInitArgs");
    if (pVmInvocationFunctions->pFctGetDefaultJavaVMInitArgs == NULL) 
	{
		FreeLibrary(handle);
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_LOAD_JVM, GetLastError(),
			"Can not find address of function JNI_GetDefaultJavaVMInitArgs in java virtual maschine library %s", pcJvmPath);
    }
}

void CJavaVirtualMaschine::addJreBinToEnvPath(LPCSTR pcJvmPath)
{
	char pcPrerequiredLibraryPath[MAX_PATH];
	LPSTR pcSearch;

	strncpy_s(pcPrerequiredLibraryPath, MAX_PATH, pcJvmPath, strnlen(pcJvmPath, MAX_PATH)+1);

	pcSearch = strrchr(pcPrerequiredLibraryPath, '\\');
	if (pcSearch !=  NULL)
	{
		*pcSearch='\0';
		pcSearch = strrchr(pcPrerequiredLibraryPath, '\\');
		if (pcSearch !=  NULL)
		{
			*pcSearch='\0';
			char pcPath[MAX_ENV_VALUE];
			GetEnvironmentVariable("PATH", pcPath, MAX_ENV_VALUE);
			strcat_s(pcPath, MAX_ENV_VALUE, ";");
			strcat_s(pcPath, MAX_ENV_VALUE, pcPrerequiredLibraryPath);

			CLog::info("Adding directory '%s' to environment PATH to ensure JVM *.dll libraries can be loaded", pcPrerequiredLibraryPath);
			SetEnvironmentVariable("PATH", pcPath);
			return;
		}
	}

	CLog::warn("Failed to extend environment PATH with '%s' in order to load *.dll", pcJvmPath);
}

void CJavaVirtualMaschine::waitForAndDestroy()
{
	jint result;
	assert(m_pJavaVirtualMaschine != NULL);
	assert(m_pJavaNativeInterface != NULL);
	
	if (m_pJavaVirtualMaschine != NULL)
	{
		result = m_pJavaVirtualMaschine->DestroyJavaVM();
		m_pJavaNativeInterface = NULL;
		m_pJavaVirtualMaschine = NULL;
		if (result != 0)
		{
			throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_DESTROY_JVM, 
				"Could not destroy java virtual maschine");
		}
	}
}

JNIEnv* CJavaVirtualMaschine::attachCurrentThread(LPCSTR pcThreadName)
{
	JNIEnv* pJavaNativeInterface;
	JavaVMAttachArgs args;
	JavaVMAttachArgs* pArgs = NULL;
	jint result;

	if (pcThreadName != NULL)
	{
		memset(&args, 0, sizeof(args));
		args.version = JNI_VERSION_1_2;
		args.name = const_cast<LPSTR>(pcThreadName);
		args.group = NULL;
		pArgs = &args;
	}
	result = m_pJavaVirtualMaschine->AttachCurrentThread((void**)&pJavaNativeInterface, pArgs);
	if (result != 0)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_ATTACH_THREAD_TO_JVM, 
			"Could not attach thread to java virtual maschine");
	}
	return pJavaNativeInterface;
}
	
void CJavaVirtualMaschine::detachCurrentThread()
{
	jint result;
	
	result = m_pJavaVirtualMaschine->DetachCurrentThread();
	if (result != 0)
	{
		throw CLaunchException(JVMLauncherErrorCodes.COULD_NOT_DETACH_THREAD_FROM_JVM, 
			"Could not detach thread from java virtual maschine");
	}
}	

JNIEnv* CJavaVirtualMaschine::getJavaNativeInterface()
{
	return m_pJavaNativeInterface;
}
