#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//-----------------------------------------------------------------------------------------------*/
/* 엔트리 함수
//-----------------------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    struct hostent* MyEntry = NULL;

    if(argc != 2)
    {
        fprintf(stderr, 
                "Usage : %s [domain name]\n", argv[0]);
        return 1;
    }

    MyEntry = gethostbyname(argv[1]);

    if(MyEntry == NULL)
    {
        perror("Error!\n");
        return 1;
    }

    printf("Host Name : %s\n", MyEntry->h_name);

    char* CurrentAddress = *(MyEntry->h_addr_list);
    if(CurrentAddress == NULL)
    {
        return 0;
    }

    while(CurrentAddress != NULL)
    {
        struct in_addr* AddressNumber = (struct in_addr*)CurrentAddress;
        printf("%s\n", inet_ntoa(*AddressNumber));

        CurrentAddress = *(++(MyEntry->h_addr_list));
    }

    return 0;
}
