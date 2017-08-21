#include <stdio.h>
#include <string.h>
#include <unistd.h>

//-------------------------------------------------------------------------------------*/
/* 전역 상수 설정
//-------------------------------------------------------------------------------------*/
#define MAX_BUFFER_LENGTH (80)

//-------------------------------------------------------------------------------------*/
/* 엔트리 함수
//-------------------------------------------------------------------------------------*/
int main(int argc, 
         char** argv)
{
    char Buffer[MAX_BUFFER_LENGTH] = { 0 };

    printf("Input Your Message : \n");
    memset(Buffer, 0, MAX_BUFFER_LENGTH);
    read(0, Buffer, MAX_BUFFER_LENGTH);
    
    printf("Your Message is : \n");
    write(1, Buffer, MAX_BUFFER_LENGTH);

    return 0;
}
