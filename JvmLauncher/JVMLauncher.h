/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

  Exports classes to:
  - start an java virtual machine
  - execute an java programm
  - register, unregister, start and stops a NT Service
  - execute an java windows service
  - test if a program is already running

--------------------------------------------------------------------------------------

Project		: JVMLauncher
Version		: 1.0
Compiler	: VC++ 5
Java		: JDK1.2
Author		: Reto Weiss
Copyright	: Ivyteam, 2000

--------------------------------------------------------------------------------------
History:
		  3.8.2000 ReW created
--------------------------------------------------------------------------------------*/
#ifndef __JVMLAUNCHER_H__
#define __JVMLAUNCHER_H__

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

#include "JavaProgram.h"
#include "JavaWindowsService.h"
#include "LaunchException.h"
#include "LaunchConfiguration.h"
#include "WindowsService.h"
#include "Log.h"
#include "VmOptions.h"
#endif
