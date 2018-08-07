#include <stdlib.h>  
#include <sys/types.h>  
#include <stdio.h>  
#include <sys/socket.h>  
#include <string.h>  
#include <signal.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <pthread.h>  
#include <time.h>
#include <string>

using namespace std;

#define BUF_SIZE 4096
#define SERVER_PORT 2179
#define QUEUE_SIZE 10
#define MAX_CLIENT 100

void *fun_thrReceiveHandler(void *socketInfo);  
void *fun_thrAcceptHandler(void *socketListen);  
int checkThrIsKill(pthread_t thr);  

char HELLO[10];

typedef struct MySocketInfo
{
    int socketCon;  
    char *ipaddr;  
    uint16_t port;  
    int on;
    pthread_t pid;
}_MySocketInfo;  

// client array  
struct MySocketInfo arrConSocket[MAX_CLIENT];  
int conClientCount = 0;  

char* getTime();
const char* getName();
char* getList();
void init_Hello();

int main()
{
    //judge if quit
    char command;
    //accept thread
    pthread_t thrAccept;
    //server address 
    struct sockaddr_in server_addr;  
    printf("server begin\n");  
    // create TCP socket *
    int socketListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
    if(socketListen < 0)
    {  
        printf("socket failed\n");  
        exit(-1);  
    }
    else
    {  
        printf("socket created\n");  
    }  
    //set zero
    bzero(&server_addr,sizeof(struct sockaddr_in));  
    //TCP/IP
    server_addr.sin_family=AF_INET;  
    //all ip
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY); /* 这里地址使用全0，即所有 */  
    //port
    server_addr.sin_port=htons(SERVER_PORT);  
    //bind
    if(bind(socketListen, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) != 0)
    {
        perror("bind failed\n");  
        exit(-1);  
    }
    else
    {  
        printf("bind created\n");  
    }  
    // listen
    if(listen(socketListen, QUEUE_SIZE) != 0)
    {  
        printf("listen failed\n");  
        exit(-1);  
    }
    else
    {  
        printf("listen created\n");  
    }  
    //create accept thread
    pthread_create(&thrAccept,NULL,fun_thrAcceptHandler,&socketListen);  
    
    while(1)
    {
        // receive command from console
        scanf("%c",&command);
        if(command == 'q')
        {
            pthread_cancel(thrAccept);
            printf("quit!\n");  
            break;
        }
        else if(command == 'l')
        {
            //total socket
            printf("total client connection： %d\n",conClientCount);  
            printf("%s\n",getList());
        }
        else if(command == 't')
        {
            printf("Server time: %s\n",getTime());
        }
        else if(command == 'n')
        {
            printf("Server name: %s\n",getName());
        }
        else if(command == 'h')
        {
            printf("help:\n");
            printf("'q' for quit\n'l' for printing list\n't' for server time\n'n' for server name\n\n");
        }
        sleep(0.5);  
    }
}  


void *fun_thrReceiveHandler(void *socketCon)
{
    //buffer for receive data
    char buffer[BUF_SIZE];  
    //buffer for send data
    char buf[BUF_SIZE];
    //for get the function res
    char *buf_tmp;
    //buffer length
    int buffer_length;  
    //socket connection number
    int _socketCon;
    _socketCon = *((int *)socketCon);  
    //send hello
    write(_socketCon, HELLO, 10);  
    while(1)
    {  
        //set buffer 0  
        bzero(&buffer,sizeof(buffer));  
        bzero(&buf,sizeof(buffer)); 
        //socket number
        sleep(0.1);
        //printf("receving info... the socket number is %d\n",_socketCon);  
        //read info
        buffer_length = read(_socketCon,buffer,BUF_SIZE);  
        if(buffer_length == 0)
        {  
            printf("Client %d quit\n",_socketCon);  
            conClientCount--;  
            break;  
        }
        else if(buffer_length < 0)
        {  
            printf("Receive info failed\n");  
            break;  
        }
        else
        {
            switch (buffer[0])
            {
                //request
                case 1:
                {
                    switch(buffer[1])
                    {
                        //getTime
                        buf[0] = 2;
                        case 1:
                        {
                            buf[1] = 1;
                            buf[2] = 0;
                            buf_tmp = getTime();
                            strcat(buf,buf_tmp);
                            printf("debug: %s\n",buf + 2);
                            write(_socketCon, buf, strlen(buf + 2) + 2); 
                            free(buf_tmp);
                            break;
                        }
                        //getName
                        case 2:
                        {
                            buf[1] = 2;
                            buf[2] = 0;
                            strcat(buf,getName());
                            printf("debug: %s\n",buf + 2);
                            write(_socketCon, buf, strlen(buf + 2) + 2); 
                            break;
                        }
                        //getList
                        case 3:
                        {
                            buf[1] = 3;
                            buf[2] = 0;
                            buf_tmp = getList();
                            strcat(buf, buf_tmp);
                            printf("debug: %s\n",buf + 2);
                            write(_socketCon, buf, strlen(buf + 2) + 2);
                            free(buf_tmp);
                            break;
                        }
                        //send to another
                        case 4:
                        {
                            buf[1] = 4;
                            buf[2] = 0;
                        }
                        //error
                        default:
                        {
                            printf("error message");
                            break;
                        }
                    }
                    break;
                }
                case 2:
                {
                    printf("server do not receive response information");
                    break;
                }
                case 3:
                {
                    printf("server do not receive instruction information");
                    break;
                }
                default:
                {
                    printf("error message!\n");
                }
            }
            
        }
        buffer[buffer_length] = '\0';  
        printf("Info from client %d : %s\n",_socketCon, buffer + 2);
        sleep(0.2);
    }

    printf("Socket %d quit\n",_socketCon);  
    //set on 0
    for(int i = 0; i < MAX_CLIENT; i++)
    {
        if(arrConSocket[i].socketCon == _socketCon)
        {
            arrConSocket[i].on = 0;
	        conClientCount--;
            break;
        }
    }
    return NULL;
}

void *fun_thrAcceptHandler(void *socketListen)
{
    init_Hello();
    int i;
    //socket listen
    int _socketListen = *((int *)socketListen);  
    int sockaddr_in_size = sizeof(struct sockaddr_in);  
    struct sockaddr_in client_addr;  
    int socketCon;
    pthread_t thrReceive = 0;  
    _MySocketInfo socketInfo;  
    while(1)
    {
        //accept
        socketCon = accept(_socketListen, (struct sockaddr *)(&client_addr), (socklen_t *)(&sockaddr_in_size));  
        if(socketCon < 0)
        {
            printf("A connection failed!\n");  
            continue;
        }
        else
        {  
            printf("A connection created!\nip: %s:%d\r\n",inet_ntoa(client_addr.sin_addr),client_addr.sin_port);  
            printf("The socket number is ：%d\n",socketCon);  
            printf("client number is %d\n",conClientCount);  
        }
        //set client socket info
        socketInfo.socketCon = socketCon;  
        socketInfo.ipaddr = inet_ntoa(client_addr.sin_addr);  
        socketInfo.port = client_addr.sin_port;
        socketInfo.on = 1;
        //save the socket info into array
        for(i = 0; i < MAX_CLIENT; i++)
        {
            if(arrConSocket[i].on == 0)
            {
                arrConSocket[i] = socketInfo;  
                conClientCount++;
                break;
            }
        }
        //create a new thread for receive
        pthread_create(&thrReceive,NULL,fun_thrReceiveHandler,&socketCon);  
        //set pid
        arrConSocket[i].pid = thrReceive;

        sleep(0.5);  
    }
}




int checkThrIsKill(pthread_t thr)
{
    int res = 1;  
    int res_kill = pthread_kill(thr,0);  
    if(res_kill == 0)
    {
        res = 0;  
    }
    return res;  
}  


char* getTime()
{
    time_t t;
    struct tm * lt;
    time (&t);
    char* buf = (char*)malloc(sizeof(char) * 32);
    lt = localtime (&t);
    sprintf ( buf, "%d/%d/%d %d:%d:%d\n",lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    return buf;
}

const char* getName()
{
    return "liu\n";
}

char* getList()
{
    char* buf = (char*)malloc(sizeof(char) * conClientCount * 40);
    char* tmp = (char*)malloc(sizeof(char) * 40);
    for(int i = 0; i < MAX_CLIENT; i++)
    {
        if(arrConSocket[i].on == 1)
        {
            sprintf(tmp,"socket %d(%s:%d)",arrConSocket[i].socketCon,arrConSocket[i].ipaddr,arrConSocket[i].port);
            strcat(buf,tmp);
        }
    }
    free(tmp);
    return buf;
}

void init_Hello()
{
    HELLO[0] = 3;
    HELLO[1] = 1;
    HELLO[2] = 'H';
    HELLO[3] = 'e';
    HELLO[4] = 'l';
    HELLO[5] = 'l';
    HELLO[6] = 'o';
    HELLO[7] = '!';
    HELLO[8] = '\n';
    HELLO[9] = 0;
}