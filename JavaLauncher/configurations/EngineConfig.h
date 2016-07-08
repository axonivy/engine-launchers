/*
 * WebAppC Configuration
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\EngineConfig.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setApplicationName("EngineConfig");
	config.setMainJavaClass("ch.ivyteam.ivy.server.configuration.ServerConfigurationTool");
	return config;
}

#endif
