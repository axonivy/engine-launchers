/*
 * ServerAdminC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\ivyFrameServerAdmin.ico"

static LPCSTR pcApplicationName = "ivyFrameServerAdminC";
static LPCSTR pcClassName = "ivyteam.cs.server.admin.Administrator";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClientHotspotJVM;
static bool bSingleton = false;
static int nStartMem=0;
static int nMaxMem=0;


#endif
