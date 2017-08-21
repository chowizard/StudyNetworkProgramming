#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

#define THREAD_COUNT_MAX (2)

//----------------------------------------------------------------------------*/
/* 전역 변수 / 함수 선언
//----------------------------------------------------------------------------*/
pthread_mutex_t Mutex;

void* ThreadFunction(void* Data);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    pthread_t ThreadID[THREAD_COUNT_MAX] = { 0 };
    int ThreadCount = 0;
   
    int MutexInitResult = pthread_mutex_init(&Mutex, NULL);
    if(0 != MutexInitResult)
    {
        printf("Mutex initialize failure!\n");
        return SEVERITY_ERROR;
    }

    for(int Index = 0; Index < THREAD_COUNT_MAX; ++Index)
    {
        int Result = pthread_create(&ThreadID[Index],
                                    NULL,
                                    ThreadFunction,
                                    (void*)&ThreadCount);
        printf("Create thread result : %d\n", Result);

        usleep(5000);
    }

    while(true)
    {
        printf("Main Thread : %d\n", ThreadCount);
        sleep(2);
    }

    for(int Index = 0; Index < THREAD_COUNT_MAX; ++Index)
    {
        pthread_join(ThreadID[Index], NULL);
    }

    return SEVERITY_SUCCESS;
}

//----------------------------------------------------------------------------*/
/* 스레드 별로 실행할 예제 함수
//----------------------------------------------------------------------------*/
void* ThreadFunction(void* Data)
{
    int* Count = (int*)Data;
    pthread_t ThreadID = pthread_self();

    int Temp = 0;
    while(true)
    {
        // 임계영역 설정
        int MutexLockResult = pthread_mutex_lock(&Mutex);
        if(-1 == MutexLockResult)
        {
            printf("Mutex lock is failed! : %d", MutexLockResult);
        }

        // 임계영역 내 수행
        Temp = *Count;
        ++Temp;

        sleep(1);

        (*Count) = Temp;

        printf("Thread ID : %lu %d\n", ThreadID, *Count);

        // 임계영역 해제
        int MutexUnlockResult = pthread_mutex_unlock(&Mutex);
        if(-1 == MutexUnlockResult)
        {
            printf("Mutex lock is failed! : %d", MutexLockResult);
        }
    }
}
