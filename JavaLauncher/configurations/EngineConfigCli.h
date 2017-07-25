/*
 * WebAppC Configuration
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\EngineConfigCli.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setApplicationName("EngineConfigCli");
	config.setOsgiApplicationName("ch.ivyteam.ivy.server.configuration.cli");
	return config;
}

#endif
