#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#if !defined (true)
    #define true (1)
#endif
#if !defined (false)
    #define false (0)
#endif

// 성공
#define SUCCESS (0)
// 실패
#define FAILED (-1)

#define MAXLINE 1024

//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int ServerSocketFD = socket(AF_INET, 
                                SOCK_STREAM, 
                                0);
    if(ServerSocketFD == -1)
    {
        perror("Create Socket Error! : \n");
        return FAILED;
    }

    /* 연결 요청할 서버의 주소와 포트 번호, 프로토콜 등을 지정한다.
     * */
    struct sockaddr_in ServerAddress;
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    ServerAddress.sin_port = htons(atoi(argv[1]));
    //ServerAddress.sin_port = htons(3500);

    int ClientLength = sizeof(ServerAddress);

    int ConnectResult = connect(ServerSocketFD, 
                                (struct sockaddr*)&ServerAddress, 
                                ClientLength);
    if(ConnectResult == -1)
    {
        perror("Connect error!\n");
        return FAILED;
    }

    char Buffer[MAXLINE] = { 0 };
    memset(Buffer, 0, sizeof(char) * MAXLINE);
    
    // 키보드 입력을 기다린다
    read(0, Buffer, MAXLINE);

    int Count = 3;
    while(true)
    {
        // 입력받은 데이터를 서버로 전송한다
        int WriteResult = write(ServerSocketFD, 
                                Buffer, 
                                MAXLINE);
        if(WriteResult <= 0)
        {
            perror("Write error!\n");
            return FAILED;
        }

        if((Count % 3) == 0)
        {
            send(ServerSocketFD, 
                 "9", 
                 1, 
                 MSG_OOB);
        }

        memset(Buffer, 0, MAXLINE);

        // 서버로부터 데이터를 읽는다
        int ReadResult = read(ServerSocketFD,
                              Buffer,
                              MAXLINE);
        if(ReadResult <= 0)
        {
            perror("Read error!\n");
            return FAILED;
        }

        ++Count;
    }

    close(ServerSocketFD);

    printf("Read : %s\n", Buffer);

    return SUCCESS;
}
