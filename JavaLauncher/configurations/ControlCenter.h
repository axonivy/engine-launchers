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
	config.setApplicationName("ControlCenter");
	config.setMainJavaClass("ch.ivyteam.server.control.center.ServerControlCenter");
	return config;
}

#endif
