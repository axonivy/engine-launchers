/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\LogServer.ico"

static LPCSTR pcApplicationName = "LogServerC";
static LPCSTR pcClassName = "ivyteam.log.server.LogServer";
static LPCSTR pcAuxDir = "lib\\ivy";
// ReW 11.1.2001 because JDK1.3 Hotspot JVM has memory leaks and other problems run with classic JVM
static JVMType jvmType = ClassicJVM;
static bool bSingleton = false;
static int nStartMem=5;
static int nMaxMem=1500;


#endif