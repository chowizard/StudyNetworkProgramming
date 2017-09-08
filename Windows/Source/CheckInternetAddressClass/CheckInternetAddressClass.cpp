#include "stdafx.h"

#include <stdio.h>
#include <malloc.h>
#include <WinSock2.h>

u_long ClassMask();

//------------------------------------------------------------------------------
//int _tmain(int argc, _TCHAR* argv[])
int __CRTDECL wmain(int argc, wchar_t* argv[])
{
	char Buffer[256] = { 0 };
	wchar_t ClassName[6][16] = 
	{
		L"E Class\0", 
		L"D Class\0", 
		L"C Class\0", 
		L"B Class\0", 
		L"A Class\0", 
		L"\0"
	};

	if(argc != 2)
	{
		fwprintf_s(stderr, 
				   L"Usage : %s [Internet Address]\n", 
				   argv[0]);

		return SEVERITY_ERROR;
	}

	//int ConvertedChars = 0;
	//size_t ArgumentLength = wcslen(argv[1]);
	//size_t ConvertSize = (ArgumentLength + 1) * sizeof(wchar_t);
	//char* IPAddress = reinterpret_cast<char*>(alloca(ConvertSize));
	//
	//wchar_t* ArgumentStringPosition = argv[1];
	//while((*ArgumentStringPosition) == '\n')
	//{
	//	wctomb_s(&ConvertedChars, 
	//			 IPAddress, 
	//			 ConvertSize, 
	//			 *ArgumentStringPosition++);
	//}

	size_t InputArgumentLength = wcslen(argv[1]) + 1;
	size_t ConvertSize = InputArgumentLength * sizeof(wchar_t);
	char* IPAdress = reinterpret_cast<char*>(alloca(ConvertSize));
	WideCharToMultiByte(CP_ACP, 
						0, 
						argv[1], 
						InputArgumentLength, 
						IPAdress, 
						ConvertSize, 
						NULL, 
						NULL);

	unsigned int LongAddress = inet_addr(IPAdress);
	if(LongAddress == INADDR_ANY)
	{
		wprintf_s(L"INADDR_ANY\n");
		return SEVERITY_SUCCESS;
	}
	else if(LongAddress == INADDR_NONE)
	{
		wprintf_s(L"INADDR_NONE\n");
		return SEVERITY_SUCCESS;
	}

	unsigned int HostAddress = ntohl(LongAddress);
	int Index = 0;
	for(Index = 0; Index < 5; ++Index)
	{
		unsigned int CurrentAddressClass = ClassMask() << Index;
		if((CurrentAddressClass & HostAddress) == CurrentAddressClass)
		{
			break;
		}
	}
	wprintf_s(L"%s -> %s\n", argv[1], ClassName[Index]);

	return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
u_long ClassMask()
{
	return htonl(15L << 4);
}
