/*
 * Runtime Configuration
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setWindowsServiceName("Xpert.ivy Server");
	config.setMainJavaClass("ivyteam.webapp.system.NtService");
	return config;
}

#endif
