/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\AxonIvyEngineConfig.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;
	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setApplicationName("AxonIvyEngineConfig");
	config.setOsgiApplicationName("ch.ivyteam.ivy.server.configuration.ui");
#ifndef _WINDOWS
	config.setConsole(true);
#endif
	return config;
}

#endif
