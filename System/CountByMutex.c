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

#define SUCCESS (0)
#define FAILED (-1)

#define ARRAY_LENGTH_MAX (1024)
#define THREAD_COUNT_MAX (4)

//----------------------------------------------------------------------------*/
/* 타입 / 객체 선언과 정의
//----------------------------------------------------------------------------*/
typedef struct _DataInfo
{
    int* dataPoint;
    int index;
}
DataInfo;

//----------------------------------------------------------------------------*/
/* 전역 변수 / 전역 함수 선언
//----------------------------------------------------------------------------*/
// 전역 뮤텍스 객체
pthread_mutex_t g_Mutex;
// 전역 컨디션 객체
pthread_cond_t g_Condition;
// 
int* Datas;
// 
int Sums[ARRAY_LENGTH_MAX];

//
void* ThreadFunction(void* Data);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int ThreadResult = SUCCESS;

    Datas = malloc(sizeof(int) * ARRAY_LENGTH_MAX);
    if(Datas == NULL)
    {
        printf("Datas allocation failed!\n");
        return FAILED;
    }

    ThreadResult = pthread_mutex_init(&g_Mutex, NULL);
    if(ThreadResult != SUCCESS)
    {
        printf("Thread initialize failed!\n");
        return FAILED;
    }

    ThreadResult = pthread_mutex_init(&g_Condition, NULL);
    if(ThreadResult != SUCCESS)
    {
        printf("Thread initialize failed!\n");
        return FAILED;
    }

    pthread_t Threads[THREAD_COUNT_MAX];
    DataInfo CalculateData;
    for(int Index = 0; Index < THREAD_COUNT_MAX; ++Index)
    {
        CalculateData.dataPoint = Datas;
        CalculateData.index = Index; 

        int ThreadResult = pthread_create(&Threads[Index],
                                          NULL,
                                          ThreadFunction,
                                          (void*)&CalculateData);

        usleep(100);
    }

    for(int Index = 0; Index < ARRAY_LENGTH_MAX; ++Index)
    {
        *Datas = Index;
        *Datas++;
    }

    pthread_cond_broadcast(&g_Condition);

    int Sum = 0;
    for(int Index = 0; Index < THREAD_COUNT_MAX; ++Index)
    {
        pthread_join(Threads[Index], NULL);
        Sum += Sums[Index];
    }
    printf("SUM (0-99) : %d\n", Sum);

    return SUCCESS;
}

//----------------------------------------------------------------------------*/
/* 각 스레드마다 계산하는 함수
//----------------------------------------------------------------------------*/
void* ThreadFunction(void* Data)
{
    DataInfo CalculateData;
    CalculateData = *((DataInfo*)Data);

    pthread_mutex_lock(&g_Mutex);
    pthread_cond_wait(&g_Condition, &g_Mutex);
    printf("Start %d Thread\n", CalculateData.index);
    pthread_mutex_unlock(&g_Mutex);

    int Sum = 0;
    for(int Index = 0; Index < 25; ++Index)
    {
        int DataPointIndex = (CalculateData.index * 25) + Index;
        Sum += CalculateData.dataPoint[DataPointIndex];
    }
    printf("(%d) %d\n", CalculateData.index, Sum);

    Sums[CalculateData.index] = Sum;

    return NULL;
}

