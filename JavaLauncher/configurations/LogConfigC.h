/*
 * WebApp Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\LogServerConfig.ico"

static LPCSTR pcApplicationName = "LogConfig";
static LPCSTR pcClassName = "ivyteam.log.LogConfigurationDlg";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClientHotspotJVM;
static bool bSingleton = true;
static int nStartMem=0;
static int nMaxMem=0;


#endif
