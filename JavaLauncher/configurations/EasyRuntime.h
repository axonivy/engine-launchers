/*
 * EasyRuntime Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\EasyRuntime.ico"

static LPCSTR pcApplicationName = "EasyRuntime";
static LPCSTR pcClassName = "ivyteam.cs.webapp.runtime.EasyRuntimeApp";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClientHotspotJVM;
static bool bSingleton = true;
static int nStartMem=0;
static int nMaxMem=0;


#endif
