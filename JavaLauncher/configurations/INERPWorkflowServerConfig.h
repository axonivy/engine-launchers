/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\INERPWorkflowServerConfig.ico"

static LPCSTR pcApplicationName = "ivyGridServerConfig";
static LPCSTR pcClassName = "ivyteam.webapp.system.ConfigurationDlg";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClientHotspotJVM;
static bool bSingleton = true;
static int nStartMem=10;
static int nMaxMem=0;

#endif
