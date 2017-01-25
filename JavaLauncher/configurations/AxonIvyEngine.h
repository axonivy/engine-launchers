/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\AxonIvyEngine.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;
	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setApplicationName("AxonIvyEngine");
	config.setOsgiApplicationName("ch.ivyteam.ivy.server.exec.engine");
	config.setServerStopArgument("stop");
#ifndef _WINDOWS
	config.setConsole(true);
#endif
	return config;
}

#endif
