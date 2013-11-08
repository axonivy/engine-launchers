/*
 * WebAppC Configuration 
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define APPICON "icons\\RichDialogApplicationLauncher.ico"

CLaunchConfiguration getLaunchConfiguration()
{
	CLaunchConfiguration config;

	config = CLaunchConfiguration::getServerApplicationDefault();
	config.setApplicationName("RichDialogApplicationLauncher");
	config.setMainJavaClass("ch.ivyteam.ivy.server.richdialog.application.launcher.RichDialogApplicationLauncher");
	return config;
}

#endif
