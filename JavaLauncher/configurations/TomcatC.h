/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\Tomcat.ico"

static LPCSTR pcApplicationName = "TomcatC";
static LPCSTR pcClassName = "ivyteam.webapp.servlets.Tomcat";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ServerHotspotJVM;
static bool bSingleton = false;
static int nStartMem=10;
static int nMaxMem=1500;

#endif
