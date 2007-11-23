/*
-------------------------------------------------------------------------------------

Java virtual maschine arguments

--------------------------------------------------------------------------------------

Project		: JVMLauncher
Version		: 1.0
Compiler	: VC++ 6
Java		: JDK1.4
Author		: Reto Weiss
Copyright	: Ivyteam, 2006

--------------------------------------------------------------------------------------
History:
		  28.3.2006 ReW created
--------------------------------------------------------------------------------------*/
#define JVMLauncher

#include "VmOptions.h"
#include <assert.h>

CVmOptions::CVmOptions(void)
: m_dwSize(10), m_pOptions(NULL), m_dwCount(0)
{
	m_dwSize = 10;
	m_pOptions = new JavaVMOption[m_dwSize];
}

CVmOptions::~CVmOptions(void)
{
	for (DWORD pos = 0; pos < m_dwCount; pos++)
	{
		delete m_pOptions[pos].optionString;
	}
	delete m_pOptions;
}

void CVmOptions::addOption(LPCSTR pcOption, void* pvExtraInfo)
{
	assert(pcOption != NULL);
	m_dwCount++;
	if (m_dwCount > m_dwSize)
	{
		m_dwSize = m_dwSize*2;
		JavaVMOption* pNewOptions = new JavaVMOption[m_dwSize];
		for (DWORD pos=0; pos < m_dwCount-1; pos++)
		{
			pNewOptions[pos] = m_pOptions[pos];
		}
		delete m_pOptions;
		m_pOptions = pNewOptions;
	}
	m_pOptions[m_dwCount-1].optionString = new char[strlen(pcOption)+1];
	strcpy_s(m_pOptions[m_dwCount-1].optionString, strlen(pcOption)+1, pcOption);
	m_pOptions[m_dwCount-1].extraInfo = pvExtraInfo;
}

/*
 * Add a new option 
 * @param pcOption the option to add 
 * @param pvExtraInfo the extra information about the option. Maybe NULL.
 */
void CVmOptions::addOption(LPCSTR pcBeginOption, LPCSTR pcEndOption, void* pvExtraInfo)
{
	size_t dwLength;
	assert(pcBeginOption != NULL);
	assert(pcEndOption != NULL);
	m_dwCount++;
	if (m_dwCount > m_dwSize)
	{
		m_dwSize = m_dwSize*2;
		JavaVMOption* pNewOptions = new JavaVMOption[m_dwSize];
		for (DWORD pos=0; pos < m_dwCount-1; pos++)
		{
			pNewOptions[pos] = m_pOptions[pos];
		}
		delete m_pOptions;
		m_pOptions = pNewOptions;
	}
	dwLength = pcEndOption-pcBeginOption+1;
	m_pOptions[m_dwCount-1].optionString = new char[dwLength];
	strncpy_s(m_pOptions[m_dwCount-1].optionString, dwLength, pcBeginOption, dwLength-1);
	m_pOptions[m_dwCount-1].extraInfo = pvExtraInfo;
}

DWORD CVmOptions::getCount()
{
	return m_dwCount;
}

JavaVMOption* CVmOptions::getOptions()
{
	return m_pOptions;
}
