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
	config.setMainJavaClass("ch.ivyteam.ivy.server.configuration.ui.ServerConfigurationUILauncher");
	return config;
}

#endif
