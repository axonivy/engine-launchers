/*
 * Launch configuration for the Xpert.ivy Server Windows Service
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config();
	config.setMainJavaClass("ch.ivyteam.ivy.boot.osgi.win.WindowsServiceLauncher");
	config.setOsgiApplicationName("ch.ivyteam.ivy.server.exec.engine.service");
    config.setEnableLoggingInfo(true);
	return config;
}

#endif
