#include "stdafx.h"
#include "FNUtility.h"

//------------------------------------------------------------------------------
int ConvertWStringToPacket(wchar_t* InputString,
                           size_t InputLength, 
                           OUT char* Buffer, 
                           size_t BufferLength)
{
    if(Buffer == nullptr)
    {
        wprintf_s(L"Buffer is NULL!");
        return SEVERITY_ERROR;
    }

    if(BufferLength < 0)
    {
        wprintf_s(L"Buffer length is invalid!");
        return SEVERITY_ERROR;
    }

    if(InputString == nullptr)
    {
        wprintf_s(L"Record target is null!");
        return SEVERITY_ERROR;
    }

    if(InputLength < 0)
    {
        wprintf_s(L"Input string length is invalid!");
        return SEVERITY_ERROR;
    }

    return WideCharToMultiByte(CP_ACP, 
                               0, 
                               InputString, 
                               InputLength, 
                               Buffer, 
                               BufferLength, 
                               NULL, 
                               NULL);
}

//------------------------------------------------------------------------------
int ConvertPacketToWString(char* Buffer, 
                           size_t BufferLength, 
                           OUT wchar_t* RecordTarget, 
                           size_t RecordLength)
{
    if(Buffer == nullptr)
    {
        wprintf_s(L"Buffer is NULL!");
        return SEVERITY_ERROR;
    }

    if(BufferLength < 0)
    {
        wprintf_s(L"Buffer length is invalid!");
        return SEVERITY_ERROR;
    }

    if(RecordTarget == nullptr)
    {
        wprintf_s(L"Record target is null!");
        return SEVERITY_ERROR;
    }

    if(RecordLength < 0)
    {
        wprintf_s(L"Record target length is invalid!");
        return SEVERITY_ERROR;
    }

    int result =  MultiByteToWideChar(CP_ACP, 
                               0, 
                               Buffer, 
                               BufferLength, 
                               RecordTarget, 
                               RecordLength);
     return result;
}