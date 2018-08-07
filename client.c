#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_PORT	2179 //侦听端口
#define BUF_SIZE 4096

extern int errno;
int mutex = 0;
char buf[BUF_SIZE] = "\0";
char *result = NULL;

void thread(void *arg)
{
    int sockfd = *((int*)arg);
    int count = 1; 
    while(1)  //循环等待服务器有没有信息发过来，有就打印出来，没就阻塞等待  
    {   
        while(mutex > 0);
        memset(buf, 0, 4096);
        read(sockfd, buf, BUF_SIZE);
        if(strlen(buf) < 4095)
        {
            if(count == 1)
            {
                result = (char *)malloc(BUF_SIZE * sizeof(char));
                strcpy(result, buf);
            }
            else
            {
                strcat(result, buf);
            }
            count = 1;
            mutex = 1;
        }
        else
        {
            if(count == 1)
            {
                result = (char *)malloc(BUF_SIZE * sizeof(char));
                strcpy(result, buf);
                count++;
            }
            else
            {
                strcat(result, buf);
                count++;
            }
        }  
    }
}

void thread_print()
{
    while(1)
    {
        while(mutex <= 0);
        if(result != NULL)
        {
            printf("Get Result:\n");
            printf("%s\n", result + 2);
            free(result);
            mutex = 0;
        }
        if(strlen(result) == 0)
        {
            break;
        }
    }
}

int main(void)
{	
    char str[BUF_SIZE];
    int choice;
    int sockfd;
    int ret;
	struct sockaddr_in serverAddr;
    pthread_t id;
    pthread_t id_print;
    int i;
    char s[20];
    // 创建一个socket：
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket() failed! code:%d\n", errno);
		return -1;
	}

	// 设置服务器的地址信息：
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bzero(&(serverAddr.sin_zero), 8);
    
    while(1)
    {
        printf("Input a number to choose the action:\n");
        printf("1.Connect\n");
        printf("2.Close\n");
        printf("3.Get Time\n");
        printf("4.Get Name\n");
        printf("5.Action List\n");
        printf("6.Send Message\n");
        printf("7.Quit\n");
        printf("8.Send 100 times\n");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                printf("connecting!\n");
	            if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	            {
		            printf("connect() failed! code:%d\n", errno);
		            close(sockfd);
		            return -1;
	            }
	            printf("Connected!\n");
                ret = pthread_create(&id, NULL, (void*)thread, (void*)&sockfd);
                if (ret != 0)
                {
                    printf("Create thread error!n");
                    return -1;
                }
                ret = pthread_create(&id_print, NULL, (void*)thread_print, NULL);
                if (ret != 0)
                {
                    printf("Create thread error!n");
                    return -1;
                }
                break;
            case 2:
                printf("closing!\n");
                close(sockfd);
                printf("Closed!\n");
                break;
            case 3:
                memset(str,0,4096);
                str[0] = 1;
                str[1] = 1;
                strcat(str, "Get Time");
                if(send(sockfd, (char *)&str, strlen(str) + 1, 0) == -1)
	            {
		            printf("send() failed!\n");
		            close(sockfd);
		            return -1;
	            }
	            printf("time request has been sent!\n");
                while(mutex <= 0);
                break;
            case 4:
                memset(str,0,4096);
                str[0] = 1;
                str[1] = 2;
                strcat(str, "Get Name");
                if(send(sockfd, (char *)&str, strlen(str) + 1, 0) == -1)
	            {
		            printf("send() failed!\n");
		            close(sockfd);
		            return -1;
	            }
	            printf("name request has been sent!\n");
                while(mutex <= 0);
                break;
            case 5:
                memset(str,0,4096);
                str[0] = 1;
                str[1] = 3;
                strcat(str, "Get Client List");
                if(send(sockfd, (char *)&str, strlen(str) + 1, 0) == -1)
	            {
		            printf("send() failed!\n");
		            close(sockfd);
		            return -1;
	            }
	            printf("client list request has been sent!\n");
                while(mutex <= 0);
                break;
            case 6:
                memset(str,0,4096);
                str[0] = 1;
                str[1] = 4;
                printf("Please input destination:\n");
                scanf("%s",s);
                strcat(str, s);
                strcat(str," ");
                //strcat(str, "127.0.0.1");
                //strcat(str, ":8000 ");
                strcat(str, "My name is Jack!");
                if(send(sockfd, (char *)&str, sizeof(str), 0) == -1)
	            {
		            printf("send() failed!\n");
		            close(sockfd);
		            return -1;
	            }
	            printf("student info has been sent!\n");
                break;
            case 7:
                break;
            case 8:
                for(i = 0; i<100; i++)
                {
                    memset(str,0,4096);
                    str[0] = 1;
                    str[1] = 1;
                    strcat(str, "Get Time");
                    if(send(sockfd, (char *)&str, strlen(str) + 1, 0) == -1)
	                {
		                printf("send() failed!\n");
		                close(sockfd);
		                return -1;
	                }
	                printf("time request has been sent!\n");
                    while(mutex <= 0);
                    printf("%d\n",mutex);
                }
                break;
        }
        if(choice == 7)
        {
            close(sockfd);
            printf("QUIZ");
            return 0;
        }
    }
	return 0;
}
