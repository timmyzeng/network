#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// ./client service_ip service_port
int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Usage: %s service_ip service_port\n", argv[0]);
        return 1;
    }
	//创建套接字
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock < 0){
        printf("create client_sock failed, errno : %d, error_string: %s\n", errno, strerror(errno));
        return 2;
    }
	//初始化套接字
    struct sockaddr_in client_socket;

    bzero(&client_socket, sizeof(client_socket));
    client_socket.sin_family = AF_INET;
    client_socket.sin_port = htons(atoi(argv[2]));
    int num = inet_pton(AF_INET, argv[1], &client_socket.sin_addr);
    if(num <= 0){
        if(num == 0){
            fprintf(stderr, "inet_pton not in presentation format");
        }
        else{
            printf("inet_pton failed, errno : %d, error_string: %s\n", errno, strerror(errno));
        }
        return 3;
    }
    socklen_t client_socket_len= sizeof(client_socket);
	//发起请求连接
    if((connect(client_sock, (struct sockaddr*)(&client_socket), client_socket_len)) < 0){
        printf("connect failed, errno : %d, error_string: %s\n", errno, strerror(errno));
        close(client_sock);
        return 3;
    }
    printf("connect success ...\n");
    printf("please enter message\n");
	//进入连接，开始循环发送数据
    char buf[1024];
    while(1){
        bzero(buf, sizeof(buf));
        printf("client# ");
        fgets(buf, sizeof(buf), stdin);
	
        buf[strlen(buf)-1] = '\0';
        //发送数据
        int num1 = send(client_sock, buf, strlen(buf)+1, 0);
        if(num1 > 0){
            bzero(buf, sizeof(buf));
            int num2 = recv(client_sock, buf, sizeof(buf), 0);
            if(num2 < 0){
	            //阻塞式等待数据接收
                printf("recv failed, errno : %d, error_string: %s\n", errno, strerror(errno));
                break;
            }
            printf("service# %s\n", buf);
        }
        else{
            printf("send failed, errno : %d, error_string: %s\n", errno, strerror(errno));
            break;
        }
    }
    close(client_sock);
    printf("client quit!!\n");
    return 0;
}
