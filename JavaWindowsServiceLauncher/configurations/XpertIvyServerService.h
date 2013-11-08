/*
 * Launch configuration for the Xpert.ivy Server Windows Service
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setWindowsServiceName("Xpert.ivy Server");
	config.setMainJavaClass("ch.ivyteam.ivy.server.WindowsService");
	return config;
}

#endif
