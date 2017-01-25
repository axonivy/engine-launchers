#define JVMLauncher

#include "JavaMainArguments.h"
#include "LaunchException.h"

JavaMainArguments::JavaMainArguments(int argc, LPSTR argv[])
: m_argc(argc), m_argv(NULL), m_length(1000)
{	
	LPSTR* argvNew = new LPSTR[m_length];

	for (int i = 0; i < m_argc; i++)
	{
		argvNew[i] = new char[strlen(argv[i])+1];
		strcpy_s(argvNew[i], strlen(argv[i])+1, argv[i]);
	}

	m_argv = argvNew;
}

JavaMainArguments::~JavaMainArguments(void)
{
	for (int i = 0; i < m_argc; i++)
	{
		delete m_argv[i];
	}
	delete m_argv;
}

void JavaMainArguments::addArgument(LPSTR pcArg)
{
	if (m_argc >= m_length)
	{
		throw CLaunchException(JVMLauncherErrorCodes.ILLEGAL_AMOUNT_OF_ARGUMENTS,
			"To much arguments passed to limited JavaMainArguments container"); 
	}
	
	m_argv[m_argc] = new char[strlen(pcArg)+1];
	strcpy_s(m_argv[m_argc], strlen(pcArg)+1, pcArg);
	
	m_argc = m_argc + 1;
}

int JavaMainArguments::getCount()
{
	return m_argc;
}

LPSTR JavaMainArguments::getArgument(int pos)
{
	return m_argv[pos];
}

LPSTR* JavaMainArguments::getArguments()
{
	return m_argv;
}
