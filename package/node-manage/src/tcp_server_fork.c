#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "typedef.h"
#include <errno.h>

#define PORT  8890
#define QUEUE_SIZE   10
#define BUFFER_SIZE 1024
const char bReuseaddr=1;
//传进来的sockfd，就是互相建立好连接之后的socket文件描述符
//通过这个sockfd，可以完成 [服务端]<--->[客户端] 互相收发数据
volatile bool lora_rx_done = 0;
extern uint8_t radio2tcpbuffer[];
int fd;

void str_echo(int sockfd)
{
    char buffer[BUFFER_SIZE];
    pid_t pid = getpid();
    while(1)
    {
        memset(buffer,0,sizeof(buffer));
        int len = recv(sockfd, buffer, sizeof(buffer),MSG_DONTWAIT);
        if(len < 0)
        {
			if((errno == EINTR)
				|| (errno ==EWOULDBLOCK)
				|| (errno ==EAGAIN))
			{
			}
			else
			{
            	printf("child process: %d ,%d recv error and exited.\n",pid,__LINE__);
            	break;
			}
        }
		else if(len > 0)
		{
			printf("pid:%d receive:\n",pid);
        	fputs(buffer, stdout);
			write(fd,buffer,len);
        	len = send(sockfd, buffer, len, 0);
			if(len < 0)
			{
				printf("child process: %d ,%d recv error and exited.\n",pid,__LINE__);
				break;
			}
		}
		else
		{
		
		}
        if(strcmp(buffer,"exit\n")==0)
        {
            printf("child process: %d exited.\n",pid);
            break;
        }
		printf("lora_rx_done : %d\r\n",lora_rx_done);
		if(lora_rx_done)
		{
			len = send(sockfd, radio2tcpbuffer, 256, 0);
			if(len < 0)
			{
				printf("child process: %d ,%d recv error and exited.\n",pid,__LINE__);
				break;
			}
			memset(radio2tcpbuffer,0,256);
			lora_rx_done = 0;
		}
		usleep(10000);
    }
    close(sockfd);
}

void *tcp_server_routin(void *data)
{
    //定义IPV4的TCP连接的套接字描述符
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
	fd = *(int*)data;
    //定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr.sin_port = htons(PORT);

    setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&bReuseaddr,sizeof(bReuseaddr));
    //bind成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        printf("#######bind error\r\n");
        exit(1);//1为异常退出
    }
    printf("bind success.\n");

    //listen成功返回0，出错返回-1，允许同时帧听的连接数为QUEUE_SIZE
    if(listen(server_sockfd,QUEUE_SIZE) == -1)
    {
        printf("#######listen error\r\n");
        exit(1);
    }
    printf("listen success.\n");

    for(;;)
    {
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        //进程阻塞在accept上，成功返回非负描述字，出错返回-1
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
        if(conn<0)
        {
            printf("#########connect error\r\n");
            exit(1);
        }
        printf("new client accepted.\n");

        pid_t childid;
        if(childid=fork()==0)//子进程
        {
            printf("child process: %d created.\n", getpid());
            close(server_sockfd);//在子进程中关闭监听
            str_echo(conn);//处理监听的连接
            exit(0);
        }
    }

    printf("closed.\n");
    close(server_sockfd);
    return 0;
}


