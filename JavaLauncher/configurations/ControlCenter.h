/*
 * Launch configuration of the server control center application
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\ControlCenter.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;
	config = CLaunchConfiguration::getGuiApplicationDefault();
	config.setOsgiApplicationName("ch.ivyteam.ivy.server.control.center.ui");
	config.useLowMemoryJvmSettings(true);
#ifndef _WINDOWS
	config.setConsole(true);
#endif
	return config;
}

#endif
