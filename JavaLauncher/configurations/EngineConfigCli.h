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
	config.setOsgiApplicationName("ch.ivyteam.ivy.server.configuration.cli");
	config.setEnableLoggingInfo(false);
	config.useLowMemoryJvmSettings(true);
	return config;
}

#endif
