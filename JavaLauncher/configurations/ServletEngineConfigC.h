/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\ServletEngineConfig.ico"

static LPCSTR pcApplicationName = "ServletEngineConfigC";
static LPCSTR pcClassName = "ivyteam.webapp.servlets.ServletEngineConfigurationDlg";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClientHotspotJVM;
static bool bSingleton = true;
static int nStartMem=10;
static int nMaxMem=0;

#endif
