#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT    5100

int main(int argc, char **argv)
{
    int serverSock_fd, clientSock_fd;
    socklen_t clen;
    int n;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];

    if((serverSock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()\n");
        return -1;
    }
    int option = 1;
    setsockopt(serverSock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // all local ip
    servaddr.sin_port = htons(TCP_PORT);

    /* bind 함수를 사용하여 서버 소켓의 주소 설정 */
    if(bind(serverSock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()\n");
        return -1;
    }

    if(listen(serverSock_fd, 8) < 0) {
        perror("listen()\n");
        return -1;
    }

    clen = sizeof(cliaddr);
    //do {
        /* 클라이언트가 접속하면 접속을 허용하고 클라이언트 소켓 생성 */
    while((clientSock_fd = accept(serverSock_fd, (struct sockaddr *)&cliaddr, &clen)) > 0)
    {
        if (clientSock_fd < 0) {
            perror("accept() error\n");
            return -1;
        }
        /* 네크워크 주소를 문자열로 변경 */
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
        printf("Client is connected : %s\n", mesg);

        if((n = read(clientSock_fd, mesg, BUFSIZ)) <= 0) {
            perror("read()\n");
            break;
        }
        if (strncmp(mesg, "q", 1) == 0) break;

        printf("received data : %s\n", mesg);

        /* 클라이언트로 buff에 있는 문자열 전송  */
        if(write(clientSock_fd, mesg, n) <= 0)
            perror("write()\n");

        close(clientSock_fd);

    }// while(strncmp(mesg, "q", 1));

    close(serverSock_fd);
    printf("close(ssock)\n");

    return 0;
}