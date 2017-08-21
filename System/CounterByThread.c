#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
//#include <linux/types.h>
//#include <linux/ipc.h>
//#include <linux/sem.h>

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
union semun
{
	int val;			/* value for SETVAL */
	struct semid_ds *buf;	/* buffer for IPC_STAT & IPC_SET */
	unsigned short *array;	/* array for GETALL & SETALL */
	struct seminfo *__buf;	/* buffer for IPC_INFO */
	void *__pad;
};

//----------------------------------------------------------------------------*/
/* 전역 변수 / 함수 선언
//----------------------------------------------------------------------------*/
int SemaphoreID = 0;

void* ThreadFunction(void* Data);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    SemaphoreID = semget((key_t)3477,
                         1, 
                         IPC_CREAT | 0666);
    if(-1 == SemaphoreID)
    {
        return SEVERITY_ERROR;
    }

    union semun SemaphoreUnion;
    SemaphoreUnion.val = 1;

    int ResultSemaphoreControl = semctl(SemaphoreID,
                                        0,
                                        SETVAL,
                                        SemaphoreUnion);
    if(-1 == ResultSemaphoreControl)
    {
        return SEVERITY_ERROR;
    }

    pthread_t ThreadID[THREAD_COUNT_MAX] = { 0 };
    int ThreadCount = 0;
    
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
    struct sembuf SemaphoreOpen = 
    {
        0,
        -1,
        SEM_UNDO
    };
    struct sembuf SemaphoreClose = 
    {
        0,
        1,
        SEM_UNDO
    };

    int* Count = (int*)Data;
    pthread_t ThreadID = pthread_self();

    int Temp = 0;
    while(true)
    {
        // 임계영역 설정
        int Result = semop(SemaphoreID, 
                           &SemaphoreOpen,
                           1);
        if(-1 == Result)
        {
            return NULL;
        }

        // 임계영역 내 수행
        Temp = *Count;
        ++Temp;

        sleep(1);

        (*Count) = Temp;

        printf("Thread ID : %lu %d\n", ThreadID, *Count);

        // 임계영역 해제
        semop(SemaphoreID,
              &SemaphoreClose,
              1);
    }
}
