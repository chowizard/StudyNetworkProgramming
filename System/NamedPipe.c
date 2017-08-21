#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

int main(int argv, char** argc)
{
    int Result = mkfifo("/tmp/myfifo_r", 
                        S_IRUSR | S_IWUSR);
    if(Result == -1)
    {
        return 1;
    }

    printf("Success create named pipe : /tmp/myfifo_r\n");

    return 0;
}
