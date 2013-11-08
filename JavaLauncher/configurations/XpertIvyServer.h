/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\XpertIvyServer.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setApplicationName("XpertIvyServer");
	config.setMainJavaClass("ch.ivyteam.ivy.server.ServerLauncher");
	return config;
}

#endif
