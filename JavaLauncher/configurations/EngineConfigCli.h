/*
 * WebAppC Configuration
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\EngineConfigCli.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config();
	config.setMainJavaClass("org.eclipse.equinox.launcher.Main");
	config.setOsgiApplicationName("ch.ivyteam.ivy.server.configuration.cli");
	config.useLowMemoryJvmSettings(true);
	return config;
}

#endif
