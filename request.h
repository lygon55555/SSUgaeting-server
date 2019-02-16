#ifndef __REQUEST_H__
#define __REQUEST_H__
typedef struct user_info{
    char id[20];
    char password[20];
    char name[20];
    char email[30];
    char sex[10];
    char age[10];
    char imageURL[100];
    char statusmsg[100];
    char height[10];
    char address[100];
    char major[20];
    char religion[10];
    char hobby[20];
    char college[20];
    char club[20];
    char abroadexp[10];
    char milserv[10];
}user_info;
typedef struct chat{
    char s_id[15];
    char d_id[15];
    char textmsg[1024];
    char acceptance[10];
    char image[100];
    char time[20];
}chat;
typedef struct filteringinfo{
    char mysex[10];
    char fromage[10];
    char toage[10];
    char heightfrom[10]; 
    char heightto[10];
    char religion[10];
    char hobby[30];
    char college[30];
    char club[30];
    char abroadexp[15];
    char milserv[15];
}filteringinfo;

extern void* request_handler(void* arg);
extern void send_error(FILE* fp);
extern void error_handling(char* message);
extern void parse_message(int clnt_sock,char* message, user_info* user, chat* d_user,char* response_packet);


#endif
