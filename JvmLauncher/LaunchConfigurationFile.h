/*-----------------------------------------------------------------------------------

   Java Virtual Maschine Launcher

-------------------------------------------------------------------------------------

- Read an ivy launch configuration file

--------------------------------------------------------------------------------------

Project		: JVMLauncher
Version		: 1.0
Compiler	: VC++ 6
Java		: JDK1.4
Author		: Reto Weiss
Copyright	: Ivyteam, 2006

--------------------------------------------------------------------------------------
History:
		  27.3.2006 ReW created
--------------------------------------------------------------------------------------*/

#ifndef __LAUNCH_CONFIGURATION_FILE_H__
#define __LAUNCH_CONFIGURATION_FILE_H__

#ifdef JVMLauncher
#define JVMLauncherExport __declspec(dllexport)
#else
#define JVMLauncherExport __declspec(dllimport)
#endif

#include "LaunchConfiguration.h"

/*
 Class to load a Launch configuration file 
 */
class JVMLauncherExport CLaunchConfigurationFile  
{
private: 
	/* The launch configuration file name */
	LPSTR m_pcLauncherConfigurationFileName;

public:
	/*
	 * Constructor
	 * @param pcLauncherConfigurationFileName the file name of the lancher configuration file
	 */
	CLaunchConfigurationFile(LPCSTR pcLauncherConfigurationFileName);

	/*
	 * Default Constructor
	 */
	CLaunchConfigurationFile();

	/*
	 * Copy constructor
	 * @param copy the object to copy
	 */
	CLaunchConfigurationFile(const CLaunchConfigurationFile& copy);

	/*
	 * Destructor
	 */
	~CLaunchConfigurationFile();

	/*
	 * Reads the ivy launch configuration file for the specified launcher.
	 * @return a launch configuration with the information read from the file. 
	 *		 If no launch configuration file exists null is returned.
	 * @thows CLaunchException if file has wrong format
	 */
	CLaunchConfiguration readLaunchConfiguration();

	/*
	 * Checks if the launch configuration file exists
	 * @return true if launch configuration file exists, otherwise false
	 */
	bool exists();

	/*
	 * Assign operator
	 * @param rightValue the right value of the assign operator
	 * @return the result of the assign operation
	 */
	CLaunchConfigurationFile& operator=(const CLaunchConfigurationFile& rightValue);

	/*
	 * Gets the launch configuration file for the executable (*.exe) in within this method is called. 
	 * The launch configuration file must be located at the same location and must have the same name 
	 * as the executable but the extension .ilc instead of .exe
	 * @return the launch configuration file for an executable
	 * @throws CLaunchException if executable can not be evaluated
	 */
	static CLaunchConfigurationFile getForExecutable();
private:
	/*
	 * Analyse a name value pair (property) in the launch configuration file 
	 * @param pLaunchConfiguration the launch configuration where to store the the information in the name value pair
	 * @param pcName the name of the launch configuration property
	 * @param pcValue the value of the launch configuration property
	 * @throws CLaunchException if value has not the correct format, or the name is unknown
	 */
	void analyseNameValuePair(CLaunchConfiguration* pLaunchConfiguration, LPCSTR pcName, LPCSTR pcValue);

	/* 
	 * Converts a property value to a number in the range 0..1000
	 * @param pcName the name of the property
	 * @param pcValue the value of the property
	 * @return value in the range [0..1000]
	 * @throws CLaunchException if pcValue is not a number or is not in the range 0..1000
	 */
	DWORD toPermille(LPCSTR pcName, LPCSTR pcValue);
	/* 
	 * Converts a property value to a number in the range 0..100
	 * @param pcName the name of the property
	 * @param pcValue the value of the property
	 * @return value in the range [0..100]
	 * @throws CLaunchException if pcValue is not a number or is not in the range 0..100
	 */
	DWORD toPercentage(LPCSTR pcName, LPCSTR pcValue);

	/* 
	 * Converts a property value to a number 
	 * @param pcName the name of the property
	 * @param pcValue the value of the property
	 * @return value as number
	 * @throws CLaunchException if pcValue is not a valid number
	 */
	DWORD toNumber(LPCSTR pcName, LPCSTR pcValue);

	/* 
	 * Converts a property value to a boolean 
	 * @param pcName the name of the property
	 * @param pcValue the value of the property
	 * @return value as boolean
	 * @throws CLaunchException if pcValue is not a boolean
	 */
	bool toBoolean(LPCSTR pcName, LPCSTR pcValue);

	/*
	 * Trims the value 
	 * @param pcString the string to trim
	 * @return the trimed string
	 */
	LPSTR trim(LPSTR pcString);

	/*
	 * Checks if the line is a comment
     * @param pcLine the line to check
	 * @return true if it is a comment line, otherwise false
	 */
	bool isCommentLine(LPSTR pcLine);


};

#endif 
