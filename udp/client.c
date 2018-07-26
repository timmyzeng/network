#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Usage: ./client ServiceIP ServicePort\n");
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

    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(argv[1]);
    service.sin_port = htons(atoi(argv[2]));
    
    socklen_t service_len = sizeof(service);

    char buf[1024];

    while(1){
        //输入传输的数据
        printf("Please Enter:");
        fflush(stdout);
        ssize_t num = read(0, buf, sizeof(buf)-1);
        if(num > 0){
            buf[num-1] = 0;
            //发送数据
            sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&service, service_len);
            //接收回显的数据
            ssize_t num = recvfrom(sock, buf, sizeof(buf)-1, 0, NULL, NULL);
            if(num > 0){
                buf[num-1] = 0;
                printf("Echo: %s\n", buf);
                if(0 == strcasecmp(buf, "Have a nice day\n")){
                    printf("\nclient exit\n");
                    close(sock);
                    break;
                }
            }
        }
    }
    return 0;
}
