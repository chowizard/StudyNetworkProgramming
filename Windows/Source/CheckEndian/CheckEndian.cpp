#include "stdafx.h"

#include <stdio.h>
#include <errno.h>

static const int BIG_ENDIAN = 0;
static const int LITTLE_ENDIAN = 1;

int CheckEndian();

//------------------------------------------------------------------------------
//int _tmain(int argc, _TCHAR* argv[])
int __CRTDECL wmain(int /*argc*/, wchar_t* /*argv*/[])
{
	if(CheckEndian() == LITTLE_ENDIAN)
	{
		wprintf_s(L"Little Endian!\n");
	}
	else
	{
		wprintf_s(L"Big Endian!\n");
	}

	_getwch();

	return 0;
}

//------------------------------------------------------------------------------s
int CheckEndian()
{
	unsigned int CheckTarget = 0x00000001;
	return ((reinterpret_cast<char*>(&CheckTarget)[0] == 0) ? BIG_ENDIAN: LITTLE_ENDIAN);
}

