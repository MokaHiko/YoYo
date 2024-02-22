#include "Assert.h"

#include <stdio.h>
#include <stdarg.h>

#include "Platform/Platform.h"
#include "Core/Log.h"

namespace yoyo
{
	void Assert(bool value, const char* message)
	{
		if(!value)
		{
			YFATAL("Assertion Failed: %s", message);
			exit(-1);
		}
	}
}
