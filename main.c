#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "request.h"


#define TRUE 1
#define FALSE 0
#define BUF_SIZE 4096
#define SMALL_BUF 128

int main(int argc, char *argv[])
{
    
    
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    pthread_t t_id;
    int option;
    char ss[BUF_SIZE]; //k
 
    socklen_t optlen,clnt_adr_size;
    if(argc!=2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    optlen=sizeof(option);
    option = TRUE;
    setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR,(void*)&option,optlen);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");
    if(listen(serv_sock,20)==-1)
        error_handling("listen() error");
    
    while(1)
    {
        clnt_adr_size=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
       // printf("A");
        printf("Connection Request : %s:%d\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));

       int a =pthread_create(&t_id,NULL,request_handler,&clnt_sock);
       //      char* msg ="OK/r/n";
       
       //      read(clnt_sock,ss,sizeof(ss));
       //      printf("%s",ss);
 

      pthread_detach(t_id);
    }
   
    close(serv_sock);
    return 0;
}


    
