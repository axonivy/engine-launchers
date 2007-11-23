/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\LogServerConfig.ico"

static LPCSTR pcApplicationName = "LogServerConfig";
static LPCSTR pcClassName = "ivyteam.log.server.ConfigurationDlg";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClientHotspotJVM;
static bool bSingleton = true;
static int nStartMem=10;
static int nMaxMem=0;


#endif
