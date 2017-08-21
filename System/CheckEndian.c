#include <stdio.h>

//-----------------------------------------------------------------------------------------------*/
/* 열거형 및 상수 정의
//-----------------------------------------------------------------------------------------------*/
#define BIG_EENDIAN (0)
#define LITTLE_ENDIAN (1)

int CheckEndian(void);

//-----------------------------------------------------------------------------------------------*/
/* 엔트리 함수
//-----------------------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int Endian = CheckEndian();
    if(Endian == LITTLE_ENDIAN)
    {
        printf("Little Endian\n");
    }
    else
    {
        printf("Little Endian\n");
    }

    return 0;
}

//-----------------------------------------------------------------------------------------------*/
/* 현재 시스템의 엔디언을 점검한다.
   - little eddian인 경우, 가장 높은 주소부터 읽도록 할 것이므로, 저장할 때 바이트의 순서를 뒤집을
    것이다.
   - big endian인 경우, 가장 낮은 주소부터 읽도록 할 것이므로, 저장할 때 바이트 순서를 그대로 가져
    갈 것이다.
//-----------------------------------------------------------------------------------------------*/
int CheckEndian(void)
{
    unsigned int Test = 0x00000001;
    char CheckPart = ((char*)&Test)[0];

    return ((CheckPart != 0) ? LITTLE_ENDIAN : BIG_EENDIAN);
}
