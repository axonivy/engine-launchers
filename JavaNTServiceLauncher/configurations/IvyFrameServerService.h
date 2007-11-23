/*
 * IvyFrame Server Configuration
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

static LPCSTR pcServiceName = "ivyFrame Server";
static LPCSTR pcClassName = "ivyteam.cs.server.NTService";
static LPCSTR pcAuxDir = "lib\\ivy";
static JVMType jvmType = ClassicJVM;
static int nStartMem = 5;
static int nMaxMem = 1500;

#endif
