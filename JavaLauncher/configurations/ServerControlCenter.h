/*
 * Launch configuration of the server control center application
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\ServerControlCenter.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getGuiApplicationDefault();
	config.setApplicationName("ServerControlCenter");
	config.setMainJavaClass("ch.ivyteam.server.control.center.ServerControlCenter");
	return config;
}

#endif
