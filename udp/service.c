#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//INADDR_ANY是一个宏，代表0，接收本网段中所有IP
#define SERVICE_IP INADDR_ANY

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Usage: ./service Port\n");
        return 1;
    }
    //AF_INET指的是使用IPv4，
    //SOCK_DGRAM指的是使用无连接的不可靠的报文传输协议如UDP
    //最后一个零指的是协议，0代表使用默认协议如SOCK_DGRAM默认UDP
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        printf("create socket error, errno is: %d, errstring is: %s\n", errno, strerror(errno));
        return 2;
    }

    //初始化变量local
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[1]));
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t local_len = sizeof(local);

    //绑定IP和端口号
    if((bind(sock, (struct sockaddr*)&local, local_len)) < 0){
        printf("bind socket error, errno is: %d, errstring is: %s\n", errno, strerror(errno));
        return 3;
    }

    char buf[1024];
    struct sockaddr_in client;

    while(1){
        socklen_t client_len = sizeof(client);
        //接收数据
        ssize_t num = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr *)&client, &client_len);
        if(num > 0){
            buf[num-1] = 0;
            //打印数据
            printf("[%s:%d] : %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buf);
            //退出
            if(0 == strcasecmp(buf, "bye")){
                char bye[] = "Have a nice day\n";
                sendto(sock, bye, sizeof(bye), 0, (struct sockaddr *)&client, client_len);
                printf("client exit\n");
                close(sock);
                break;
            }
            //发送数据
            sendto(sock, buf, sizeof(buf)-1, 0, (struct sockaddr *)&client, client_len);
        }
    }
    return 0;
}
