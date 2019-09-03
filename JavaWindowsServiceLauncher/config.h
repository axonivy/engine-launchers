/*
 * Launch configuration for the Xpert.ivy Server Windows Service
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setMainJavaClass("ch.ivyteam.ivy.boot.osgi.win.WindowsServiceLauncher");
	config.setOsgiApplicationName("ch.ivyteam.ivy.server.exec.engine.service");
	return config;
}

#endif
