#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//----------------------------------------------------------------------------*/
/* 
//----------------------------------------------------------------------------*/
#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

//----------------------------------------------------------------------------*/
/* 
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    pid_t ProcessID = 0;
    int AllocatedID = 100;

    printf("Program Start!\n");

    ProcessID = fork();
    printf("fork!\n");

    if(ProcessID == 0)
    {
        printf("I'm Child Process %d", getpid());
        while(true)
        {
            printf("P : %d\n", AllocatedID);
            ++AllocatedID;

            sleep(1);
        }
    }
    else if(ProcessID > 0)
    {
        printf("I'm Parent Process %d", getpid());
        while(true)
        {
            printf("C : %d\n", AllocatedID);
            AllocatedID += 2;

            sleep(1);
        }
    }
    else
    {
        printf("fork Failure!\n");
        return 1;
    }

    return 0;
}
