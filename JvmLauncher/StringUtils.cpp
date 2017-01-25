#define JVMLauncher

#include "StringUtils.h"

bool StringUtils::isEmpty(LPCSTR pcString)
{
	if (pcString == NULL)
    {
		return true;
    }
    return !strcmp(pcString, "");
}