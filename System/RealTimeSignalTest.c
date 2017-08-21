#include <stdio.h>
#include <unistd.h>
#include <signal.h>

//----------------------------------------------------------------------------*/
// 열거형, 상수, 콜백 정의
//----------------------------------------------------------------------------*/
#ifndef false
    #define false (0)
#endif
#ifndef true
    #define true (1)
#endif

//----------------------------------------------------------------------------*/
// 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char* argv)
{
    sigset_t SignalSet;
    siginfo_t SignalInfo;
    int SignalNumber = 0;

    printf("My Process ID %d\n", getpid());

    sigemptyset(&SignalSet);
    sigaddset(&SignalSet, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &SignalSet, NULL);

    while(true)
    {
        sleep(5);

        SignalNumber = sigwaitinfo(&SignalSet, &SignalInfo);
        printf("%d : %d\n", SignalNumber, SignalInfo.si_pid);
    }

    return 0;
}
