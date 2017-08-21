#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    printf("-- START %d --\n", getpid());
    execl("./ExecutionTest", "ExecutionTest", NULL);
    printf("-- END --\n");

    return 0;
}
