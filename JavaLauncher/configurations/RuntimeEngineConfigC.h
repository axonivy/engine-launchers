/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\RuntimeEngineConfig.ico"

static LPCSTR pcApplicationName = "RuntimeEngineConfigC";
static LPCSTR pcClassName = "ivyteam.webapp.engine.ConfigurationDlg";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClientHotspotJVM;
static bool bSingleton = true;
static int nStartMem=10;
static int nMaxMem=0;


#endif
