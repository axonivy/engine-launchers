/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\ivyGridServer.ico"

static LPCSTR pcApplicationName = "ivyGridServer_HotSpot";
static LPCSTR pcClassName = "ivyteam.webapp.system.IvyGridServer";
static LPCSTR pcAuxDir = "lib\\ivy";
// ReW 11.1.2001 because JDK1.3 Hotspot JVM has memory leaks and other problems run with classic JVM
static JVMType jvmType = ServerHotspotJVM;
static bool bSingleton = false;
static int nStartMem=10;
static int nMaxMem=1500;

#endif
