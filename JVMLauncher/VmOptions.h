/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

Java virtual maschine arguments

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
#ifndef __VM_ARGUMENTS_H__
#define __VM_ARGUMENTS_H__

#include <jni.h>
#include <windows.h>

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

/*
 * Java virtual maschine arguments
 */
class JVMLauncherExport CVmOptions
{
private:
	/* The java vm option array */
	JavaVMOption* m_pOptions;
	/* The size of the options array */
	DWORD m_dwSize;
	/* The option count */
	DWORD m_dwCount;

public:
	/*
	 * Constructor 
	 */
	CVmOptions(void);

	/*
	 * Destructor 
	 */
	~CVmOptions(void);

	/*
	 * Add a new option 
	 * @param pcOption the option to add 
	 * @param pvExtraInfo the extra information about the option. Maybe NULL.
	 */
	void addOption(LPCSTR pcOption, void* pvExtraInfo);

	/*
	 * Add a new option 
	 * @param pcOption the option to add 
	 * @param pvExtraInfo the extra information about the option. Maybe NULL.
	 */
	void addOption(LPCSTR pcBeginOption, LPCSTR pcEndOption, void* pvExtraInfo);

	/*
	 * Get the number of options
	 * @return number of options
	 */
	DWORD getCount();

	/*
	 * Gets the java virtual maschine options
	 * @return vm options
	 */
	JavaVMOption* getOptions();
};

#endif