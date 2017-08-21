#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//----------------------------------------------------------------------------*/
/* 전처리기 선언
//----------------------------------------------------------------------------*/
#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

#define SEVERITY_SUCCESS (0)
#define SEVERITY_ERROR (1)

#define THREAD_COUNT_MAX (4)
#define ARRAY_SIZE_MAX (128)

//----------------------------------------------------------------------------*/
/* 전역 변수 / 함수 선언
//----------------------------------------------------------------------------*/
void* ThreadFunction(void* Data);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    return 0;
}
