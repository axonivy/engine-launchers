/*
 * WebApp Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\ReqDisp.ico"

static LPCSTR pcApplicationName = "ReqDisp_ClassicVM";
static LPCSTR pcClassName = "ivyteam.webapp.reqdisp.RequestDispatcher";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClassicJVM;
static bool bSingleton = true;
static int nStartMem=20;
static int nMaxMem=1500;


#endif
