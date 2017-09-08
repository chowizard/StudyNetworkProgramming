#pragma once
#ifndef _FN_UTILITY_H_
#define _FN_UTILITY_H_

extern int ConvertWStringToPacket(wchar_t* InputString,
                                  size_t InputLength, 
                                  OUT char* Buffer, 
                                  size_t BufferLength);
extern int ConvertPacketToWString(char* Buffer, 
                                  size_t BufferLength, 
                                  OUT wchar_t* RecordTarget, 
                                  size_t RecordLength);

#endif