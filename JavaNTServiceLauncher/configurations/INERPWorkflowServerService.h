/*
 * Runtime Configuration
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

static LPCSTR pcServiceName = "INERP Workflow Server";
static LPCSTR pcClassName = "ivyteam.webapp.system.NtService";
static LPCSTR pcAuxDir = "lib\\ivy";
// ReW 11.1.2001 because JDK1.3 Hotspot JVM has memory leaks and other problems run with classic JVM
static JVMType jvmType = ClassicJVM;
static int nStartMem = 10;
static int nMaxMem = 1500;

#endif
