/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\AxonIvyEngine.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setApplicationName("AxonIvyEngine");
	config.setMainJavaClass("ch.ivyteam.ivy.server.ServerLauncher");
	config.setServerStopArgument("stop");
	return config;
}

#endif
