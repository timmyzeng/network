#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//定义服务器IP地址
#define SERVICE_IP INADDR_ANY
//定义listen等待队列
#define WAIT_QUEUE 5

// ./service port
int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Usage: %s port\n", argv[0]);
        return 1;
    }
	//创建服务器套接字
    int service_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(service_sock < 0){
        printf("create service_sock failed, errno : %d, error_string: %s\n", errno, strerror(errno));
        return 2;
    }
	//初始化
    struct sockaddr_in service_socket;

    bzero(&service_socket, sizeof(service_socket));
    service_socket.sin_family = AF_INET;
    service_socket.sin_port = htons(atoi(argv[1]));
    service_socket.sin_addr.s_addr = htonl(SERVICE_IP);
    socklen_t service_socket_len = sizeof(service_socket);
	//绑定端口号和IP地址
    if((bind(service_sock, (struct sockaddr*)(&service_socket), service_socket_len)) < 0){
        printf("bind failed, errno : %d, error_string: %s\n", errno, strerror(errno));
        close(service_sock);
        return 3;
    }
	//设置为监听状态
    if((listen(service_sock, WAIT_QUEUE)) < 0){
        printf("listen failed, errno : %d, error_string: %s\n", errno, strerror(errno));
        close(service_sock);
        return 4;
    }

    printf("service_socket bind listen success, wait accept...\n");
	//进入循环等待链接
    struct sockaddr_in client_socket;
    while(1){
        socklen_t client_socket_len = sizeof(client_socket);
		//随时等待接入的客户端
        int client_sock = accept(service_sock, (struct sockaddr*)(&client_socket), &client_socket_len);
        if(client_sock < 0){
            printf("accept failed, errno : %d, error_string: %s\n", errno, strerror(errno));
            printf("wait next accept\n");
            continue;
        }
        //获取接入客户端的IP地址和端口号并输出
        /*INET_ADDRSTRLEN 16*/
        char client_ip_buf[INET_ADDRSTRLEN];
        char* ptr = inet_ntop(AF_INET, &client_socket.sin_addr, client_ip_buf, sizeof(client_ip_buf));
        if(ptr == NULL){
            printf("inet_ntop failed, errno : %d, error_string: %s\n", errno, strerror(errno));
            continue;
        }
        printf("a new accept, client ip: %s, client port: %d\n", client_ip_buf, ntohs(client_socket.sin_port));
        printf("waiting message from client\n");
		//跟当前客户端进行阻塞式数据通信
        while(1){
            char buf[1024];
            bzero(buf, sizeof(buf));
            //等待客户端的数据
            int num = recv(client_sock, buf, sizeof(buf), 0);
            if(num == 0){
                printf("client quit\n");
                close(client_sock);
                break;
            }
            if(num > 0){
                printf("client# %s\n", buf);
                printf("service# ");
                bzero(buf, sizeof(buf));

                fgets(buf, sizeof(buf), stdin);
                buf[strlen(buf)-1] = '\0';
                //发送数据给客户端
                int num1 = send(client_sock, buf, strlen(buf)+1, 0);
                if(num1 < 0){
                    printf("send failed, errno : %d, error_string: %s\n", errno, strerror(errno));
                    break;
                }
            }
            else{
                printf("recv failed, errno : %d, error_string: %s\n", errno, strerror(errno));
                break;
            }
        }
        close(client_sock);
    }
    return 0;
}
