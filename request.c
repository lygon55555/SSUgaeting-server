#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include </usr/include/mysql/mysql.h>
#include "http-parser.h"
#include "request.h"


#define BUF_SIZE 2048
#define SMALL_BUF 128
#define CHOP(x) x[strlen(x)-1] = ' '
// char* Tokenizer(char* original, char* search, char token);


void* request_handler(void *arg)
{
    int clnt_sock=*((int*)arg);
//         char ss[BUF_SIZE]; 
//         read(clnt_sock,ss,sizeof(ss));
//         printf("%s",ss);
//         write(clnt_sock,response_packet,sizeof(response_packet));
//         printf("%s\n",response_packet);
//         puts("ok sent");
//         puts("BSBS");
    http_request_line_t req_line= {0,};
    http_header_fields_t fields = {0,};
    static user_info user={"ad","1","name","ad@gmail.com","sex","age","imageURL","statusmsg","height","address","major","religion","hobby","college","club","abroadexp","milserv"};
    chat chatinfo={0,};
    FILE* clnt_read;
    FILE* clnt_write;
    char buf[BUF_SIZE]={0,};
    char header[5120]={0,};
    char message[102400]={0,};
    char ct[15];
    char response_packet[5120] ={0,};
     
    read(clnt_sock,header,sizeof(header));
    
    puts(header);
//     clnt_read = fdopen(clnt_sock, "r");
  //   clnt_write = fdopen(clnt_sock, "w");
//     while(1)
//     {
//         fgets(buf,BUF_SIZE,clnt_read);
//         strcat(header,buf);
//     
//         if((strstr(buf,"&"))!=NULL)
//             break;
//     }

    char* a = strtok(header,"&");


    strcpy(message,strtok(NULL,"&"));  


     parseHttpHeader(parseHttpRequestLine(header, &req_line), &fields);
     
     parse_message(message, &user, &chatinfo, response_packet);

    //char b[1024]="HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&";
     ssize_t writelen;
     ssize_t packetlen;
     char* respons = response_packet;
    write(clnt_sock,response_packet,sizeof(response_packet));
    packetlen=strlen(respons);
    printf("%zd\n",writelen);
    printf("%zd\n",packetlen);
    puts(response_packet);
//    send_data(clnt_write,"text/plain", req_line.uri);
     close(clnt_sock);

}
void send_data(FILE* fp, char* ct, char* file_name)
{
   char protocol[]="HTTP/1.0 200 OK\r\n";
   char server[]="Server:Linux Web Server \r\n";
   char cnt_len[]= "Content-length:2048\r\n";
   char cnt_type[SMALL_BUF];
    char buf[BUF_SIZE]="HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&";
    FILE* send_file;
   sprintf(cnt_type, "Content-type:%s\r\n\r\n",ct);
    send_file=fopen(file_name,"r");
    if(send_file==NULL)
    {
        send_error(fp);
        return;
    }

    fputs(protocol,fp);
   fputs(server,fp);
   fputs(cnt_len,fp);
   fputs(cnt_type,fp);
    
//     while(fgets(buf,BUF_SIZE,send_file)!=NULL)
//     {
        fputs(buf,fp);
        puts(buf);
        fflush(fp);
//     }
    fflush(fp);
    fclose(fp);
    
}

char* content_type(char* file)
{
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);
    strtok(file_name, ".");
    strcpy(extension,strtok(NULL, "."));
    if(!strcmp(extension, "html")||!strcmp(extension, "htm"))
        return "text/html";
    else if (!strcmp(extension, "gif" ))
        return "image/gif";
    else if (!strcmp(extension, "jpeg"))
        return "image/jpeg";
    else 
        return "text/plain";
    
}
//         case '2' :      //login id/pw message
//             header = strtok(message,"$");
//             user->id = strtok(NULL,"$");
//             user->password = strtok(NULL,"$");
void send_error(FILE* fp)
{
    char protocol[]="HTTP/1.0 400 Bad Request\r\n";
    char server[]="Server:Linux Web Server \r\n";
    char cnt_len[]= "Content-length:2048\r\n";
    char cnt_type[]= "Content-type:text/html\r\n\r\n";
    char content[]="<html><head><title>NETWORK</title></head><body><font size=+5><br>error!! check request-filetype & request-method!</font></body></html>";
    
    fputs(protocol,fp);
    fputs(server,fp);
    fputs(cnt_len,fp);
    fputs(cnt_type,fp);
    fflush(fp);
}

void error_handling(char* message)
{
    fputs(message,stderr);
    fputs("\n",stderr);
    exit(1);
}

void parse_message(char* message, user_info* user, chat* chatinfo, char* response_packet)
{
	MYSQL mysql;
	MYSQL_RES* sql_res;
	MYSQL_ROW sql_row;
	int fields;

	mysql_init(&mysql);
	if(!mysql_real_connect(&mysql, "localhost", "root", "1234", NULL, 3306, (char *) NULL, 0))
	{
		printf("MYSQL CONNECTION FAILED...\n");
		printf("%s\n", mysql_error(&mysql));
		exit(1);
	}
	else
		printf("MYSQL CONNECTED\n");
		
    puts (message);
    char header[10];
    filteringinfo filter={" "," "," "," "," "," "," "," "," "," "," "};
//    user_info admin = {"ad","1",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//    puts(admin.id);
//    puts(admin.password);
    user_info temp = {0,};
    int numchattingroom=0;
    int numwaitingQ=0;
    int dbputok=0;
    
//    puts(message[7]);
//    char * emp;
  //  emp = strstr(message,"header=");
 //   putc(emp[7],stdout);
    switch(message[0])
    {
    	char query[1024] = {'\0'};
    	
        case '0' :      //id repetition check message
        	if(mysql_query(&mysql, "USE ssugaeting"))
			{
				printf("0 ssugaeting DB connection failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
				printf("0 ssugaeting DB connected\n");
			
            strcpy(header, strtok(message,"$"));
            strcpy(user->id, strtok(NULL,"$"));
            int sameidexist=1;
            
            sprintf(query, "SELECT * FROM profile WHERE id = '%s'", user->id);
            
            if(mysql_query(&mysql, query))
			{
				printf("0 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			
			sql_res = mysql_store_result(&mysql);
			sql_row = mysql_fetch_row(sql_res);
			
			if(sql_row == NULL)
			{
				printf("no data found in DB (id repetition check)\n");
				sameidexist = 1;
			}
			else
			{
				printf("same id exists\n");
				sameidexist = 0;
            }
            
            if(sameidexist==0)	//Fail
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            else	//OK
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            puts(response_packet);
            
            break;
        case '1' :      //create account message
        	if(mysql_query(&mysql, "USE ssugaeting"))
			{
				printf("1 ssugaeting DB connection failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
				printf("1 ssugaeting DB connected\n");
						
            strcpy(header , strtok(message,"$"));
            strcpy(user->id ,strtok(NULL,"$"));
            strcpy(user->password , strtok(NULL,"$"));
            strcpy(user->name ,strtok(NULL,"$"));
            strcpy(user->email , strtok(NULL,"$"));
            strcpy(user->sex , strtok(NULL,"$"));
            
            sprintf(query, "INSERT INTO profile (id, password, name, email, sex) VALUES ('%s', '%s', '%s', '%s', '%s')", user->id, user->password, user->name, user->email, user->sex);
            
            if(mysql_query(&mysql, query))
			{
				printf("1 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
		
            sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            puts(response_packet);
            break;
        case '2' :      //login id/pw message
            if(mysql_query(&mysql, "USE ssugaeting"))
			{
				printf("2 ssugaeting DB connection failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
				printf("2 ssugaeting DB connected\n");
				
            strcpy(header , strtok(message,"$"));
            
            strcpy(temp.id , strtok(NULL,"$"));
            puts(temp.id);
            
            strcpy(temp.password , strtok(NULL,"$"));
            puts(temp.password);
            
            sprintf(query, "SELECT * FROM profile WHERE id = '%s', password = '%s'", temp.id, temp.password);
            
            if(mysql_query(&mysql, query))
            {
            	printf("2 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}		
          	else
          	{	
          		sql_res = mysql_store_result(&mysql);
          		sql_row = mysql_fetch_row(sql_res);
          		
          		if(sql_row == NULL)
          		{
          			printf("no data found in DB (no same id, password)\n");
          			sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
          		}
          		else
          		{
	          		char asdf[1024];
					puts("camein");
					sprintf(asdf,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",
					sql_row[0],sql_row[1],sql_row[2],sql_row[3],sql_row[4],sql_row[5],sql_row[6],sql_row[7],sql_row[8],sql_row[9],sql_row[10],sql_row[11],sql_row[12],sql_row[13],sql_row[14],sql_row[15],sql_row[16]);
					int len = strlen(asdf);
					sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:text/plain\r\n\r\n%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",
					len,sql_row[0],sql_row[1],sql_row[2],sql_row[3],sql_row[4],sql_row[5],sql_row[6],sql_row[7],sql_row[8],sql_row[9],sql_row[10],sql_row[11],sql_row[12],sql_row[13],sql_row[14],sql_row[15],sql_row[16]);
					puts(response_packet);
				}
            }
            
            /*
            if (!strcmp(temp.id,user->id))
            {
                if(!strcmp(temp.password,user->password))
                {
                    char asdf[1024];
                    puts("camein");
                    sprintf(asdf,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",user->id,user->password,user->email,user->name,user->sex,user->statusmsg,user->age,user->height,user->address,user->hobby,user->college,user->major,user->imageURL,user->religion,user->club,user->abroadexp,user->milserv);
                    int len = strlen(asdf);
                    sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:text/plain\r\n\r\n%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",len,user->id,user->password,user->email,user->name,user->sex,user->statusmsg,user->age,user->height,user->address,user->hobby,user->college,user->major,user->imageURL,user->religion,user->club,user->abroadexp,user->milserv);
                    puts(response_packet);
                }
                else
                    sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            }
            else
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            */
            
            break;
        case '3' :      //email check message to find id
        	if(mysql_query(&mysql, "USE ssugaeting"))
			{	
				printf("3 ssugaeting DB connection failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
				printf("3 ssugaeting DB connected\n");
				
            strcpy(header, strtok(message,"$"));
            strcpy(temp.email, strtok(NULL,"$"));
            
           	sprintf(query, "SELECT * FROM profile WHERE email = '%s'", temp.email);            
            
            if(mysql_query(&mysql, query))
            {
            	printf("3 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
            }
            else
            {	
				sql_res = mysql_store_result(&mysql);
				sql_row = mysql_fetch_row(sql_res);
            	
				if(sql_row == NULL)
				{
					printf("no data found in DB (no same email)\n");
					sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
				}
				else
				{
					char asdf[1024];
					puts("camein");
					sprintf(asdf,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",
					sql_row[0],sql_row[1],sql_row[2],sql_row[3],sql_row[4],sql_row[5],sql_row[6],sql_row[7],sql_row[8],sql_row[9],sql_row[10],sql_row[11],sql_row[12],sql_row[13],sql_row[14],sql_row[15],sql_row[16]);
					int len = strlen(asdf);
					sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:text/plain\r\n\r\n%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",len,user->id,user->password,user->email,user->name,user->sex,user->statusmsg,user->age,user->height,user->address,user->hobby,user->college,user->major,user->imageURL,user->religion,user->club,user->abroadexp,user->milserv);
					puts(response_packet);
				}
				
            }
            
            /*
            if (!strcmp(temp.email,user->email))
                {
                    char asdf[1024];
                    puts("camein");
                    sprintf(asdf,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",user->id,user->password,user->email,user->name,user->sex,user->statusmsg,user->age,user->height,user->address,user->hobby,user->college,user->major,user->imageURL,user->religion,user->club,user->abroadexp,user->milserv);
                    int len = strlen(asdf);
                    sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:text/plain\r\n\r\n%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",len,user->id,user->password,user->email,user->name,user->sex,user->statusmsg,user->age,user->height,user->address,user->hobby,user->college,user->major,user->imageURL,user->religion,user->club,user->abroadexp,user->milserv);
                    puts(response_packet);
                }
            else
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            */
            break;
        case '4' :      //email/id check message to find pw
        	if(mysql_query(&mysql, "USE ssugaeting"))
			{
				printf("4 ssugaeting DB connection failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
				printf("4 ssugaeting DB connected\n");
				
            strcpy(header, strtok(message,"$"));
            strcpy(temp.id, strtok(NULL,"$"));
            strcpy(temp.email, strtok(NULL,"$"));
            
			sprintf(query, "SELECT * FROM profile WHERE id = '%s', email = '%s'", temp.id, temp.email);
			
			if(mysql_query(&mysql, query))
			{
				printf("4 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
			{
				sql_res = mysql_store_result(&mysql);
				sql_row = mysql_fetch_row(sql_res);
				
				if(sql_row == NULL)
				{
					printf("no data found in DB (no same id, email)\n");
					sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
				}
				else
				{
					char asdf[1024];
					puts("camein");
					sprintf(asdf,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",
					sql_row[0],sql_row[1],sql_row[2],sql_row[3],sql_row[4],sql_row[5],sql_row[6],sql_row[7],sql_row[8],sql_row[9],sql_row[10],sql_row[11],sql_row[12],sql_row[13],sql_row[14],sql_row[15],sql_row[16]);
					int len = strlen(asdf);
					sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:text/plain\r\n\r\n%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",
					len,sql_row[0],sql_row[1],sql_row[2],sql_row[3],sql_row[4],sql_row[5],sql_row[6],sql_row[7],sql_row[8],sql_row[9],sql_row[10],sql_row[11],sql_row[12],sql_row[13],sql_row[14],sql_row[15],sql_row[16]);
					puts(response_packet);
				}
			}

            /*
            if (!strcmp(temp.id,user->id))
            {
                if(!strcmp(temp.email,user->email))
                {
                    char asdf[1024];
                    puts("camein");
                    sprintf(asdf,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",user->id,user->password,user->email,user->name,user->sex,user->statusmsg,user->age,user->height,user->address,user->hobby,user->college,user->major,user->imageURL,user->religion,user->club,user->abroadexp,user->milserv);
                    int len = strlen(asdf);
                    sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:text/plain\r\n\r\n%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s&",len,user->id,user->password,user->email,user->name,user->sex,user->statusmsg,user->age,user->height,user->address,user->hobby,user->college,user->major,user->imageURL,user->religion,user->club,user->abroadexp,user->milserv);
                    puts(response_packet);
                }
                else
                    sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            }
            else
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            */    
                
            break;
            
        case '5' :      //new password setting
        	if(mysql_query(&mysql, "USE ssugaeting"))
			{
				printf("5 ssugaeting DB connection failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
        	else
				printf("5 ssugaeting DB connected\n");
				
				
				
				
        	//		 ask and check
        	
        	
        	
        	
        	        	
            strcpy(header, strtok(message,"$"));
            strcpy(user->id, strtok(NULL,"$"));
            strcpy(user->password, strtok(NULL,"$"));
            
            sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            break;

        case '6' :      //filter information message   
        	if(mysql_query(&mysql, "USE ssugaeting"))
			{
				printf("6 ssugaeting DB connection failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
				printf("6 ssugaeting DB connected\n");
				
            //lookup DB with filter information
            int numfilteredusers=0;
            numfilteredusers = 1; // enter #num of filtered users which are to send back as response_packet
    
            strcpy(header, strtok(message,"$"));
            strcpy(filter.mysex,strtok(NULL,"$"));
            strcpy(filter.fromage,strtok(NULL,"$"));
            strcpy(filter.toage,strtok(NULL,"$"));
            strcpy(filter.heightfrom,strtok(NULL,"$"));
            strcpy(filter.heightto,strtok(NULL,"$"));
            strcpy(filter.religion,strtok(NULL,"$"));
            strcpy(filter.hobby,strtok(NULL,"$"));
            strcpy(filter.college,strtok(NULL,"$"));
            strcpy(filter.club,strtok(NULL,"$"));
            strcpy(filter.abroadexp,strtok(NULL,"$"));
            strcpy(filter.milserv,strtok(NULL,"$"));
            
            sprintf(query, "SELECT * FROM profile WHERE sex = '%s', '%s' < age < '%s', '%s' < height < '%s', religion = '%s', hobby = '%s', college = '%s', circle = '%s', abroadExperience = '%s', militaryStatus = '%s'", 
            filter.mysex, filter.fromage, filter.toage, filter.heightfrom, filter.heightto, filter.religion, filter.hobby, filter.college, filter.club, filter.abroadexp, filter.milserv);
            
            if(mysql_query(&mysql, query))
			{
				printf("6 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
			{
          		sql_res = mysql_store_result(&mysql);
          		sql_row = mysql_fetch_row(sql_res);
          		
          		if(sql_row == NULL)
          		{
          			printf("no data found in DB (no filtered profile)\n");
					numfilteredusers = 0;	//no filtered user
				}
				else
				{
				}
					




                          
            if (numfilteredusers==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            else
            {
            	//ask and check
            	
            	
            	
            	
                //put all info to "user_info usersfromDB"
                user_info usersfromDB[numfilteredusers];
                for (int i = 0; i < numfilteredusers; i++)// "CHANGE IF DB IS IMPLEMENTED"
                {
                    strcpy(usersfromDB[i].id,user->id);
                    strcpy(usersfromDB[i].password,user->password);
                    strcpy(usersfromDB[i].email,user->email);
                    strcpy(usersfromDB[i].name,user->name);
                    strcpy(usersfromDB[i].sex,user->sex);
                    strcpy(usersfromDB[i].statusmsg,user->statusmsg);
                    strcpy(usersfromDB[i].age,user->age);
                    strcpy(usersfromDB[i].height,user->height);
                    strcpy(usersfromDB[i].address,user->address);
                    strcpy(usersfromDB[i].hobby,user->hobby);
                    strcpy(usersfromDB[i].college,user->college);
                    strcpy(usersfromDB[i].major,user->major);
                    strcpy(usersfromDB[i].imageURL,user->imageURL);
                    strcpy(usersfromDB[i].religion,user->religion);
                    strcpy(usersfromDB[i].club,user->club);
                    strcpy(usersfromDB[i].abroadexp,user->abroadexp);
                    strcpy(usersfromDB[i].milserv,user->milserv);
                }
                
                char asdf[1024];
                char endtoken[20]="&";
                
                //int size=strlen(asdf);
                strcpy(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 1024\r\nContent-Type:text/plain\r\n\r\n");
                
                for (int i = 0; i < 1; i++)
                {
                    sprintf(asdf,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s|",
                    usersfromDB[i].id,usersfromDB[i].password,usersfromDB[i].email,usersfromDB[i].name,usersfromDB[i].sex,usersfromDB[i].statusmsg,usersfromDB[i].age,usersfromDB[i].height,usersfromDB[i].address,usersfromDB[i].hobby,usersfromDB[i].college,usersfromDB[i].major,usersfromDB[i].imageURL,usersfromDB[i].religion,usersfromDB[i].club,usersfromDB[i].abroadexp,usersfromDB[i].milserv);

                    if(strcat(response_packet,asdf)==NULL)
                        error_handling("strcat() error in response_packet for filter\n");

                }
                
                puts(response_packet);
                
                if(strncat(response_packet,endtoken,1)==NULL)
                    error_handling("strcat() error in response_packet for chattinglist\n");
            }
            break;
        case '7' :      //current chatting list message
        	if(mysql_query(&mysql, "USE chatlist"))
			{
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}

            strcpy(header, strtok(message,"$"));
            strcpy(user->id,strtok(message,"$"));
            
            
            //ask and check
			sprintf(query, "SELECT * FROM profile WHERE id = '%s', email = '%s'", temp->id, temp->email);
                        
                        //get chatting lists from DB and save profile data to user_info for every chattingroom
            numchattingroom = 1; //count number of chattingrooms in possession "CHANGE IF DB IS IMPLEMENTED"
            
            if (numchattingroom==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            else
            {
                //put all info to "user_info usersfromDB"
                user_info usersfromDB[numchattingroom];
                int unreadmsg[numchattingroom]; //get unread msg from 
                int a=0;
                for (int i=0 ;i<numchattingroom;i++)// "CHANGE IF DB IS IMPLEMENTED"
                {
                    strcpy(usersfromDB[i].id,user->id);
                    strcpy(usersfromDB[i].password,user->password);
                    strcpy(usersfromDB[i].email,user->email);
                    strcpy(usersfromDB[i].name,user->name);
                    strcpy(usersfromDB[i].sex,user->sex);
                    strcpy(usersfromDB[i].statusmsg,user->statusmsg);
                    strcpy(usersfromDB[i].age,user->age);
                    strcpy(usersfromDB[i].height,user->height);
                    strcpy(usersfromDB[i].address,user->address);
                    strcpy(usersfromDB[i].hobby,user->hobby);
                    strcpy(usersfromDB[i].college,user->college);
                    strcpy(usersfromDB[i].major,user->major);
                    strcpy(usersfromDB[i].imageURL,user->imageURL);
                    strcpy(usersfromDB[i].religion,user->religion);
                    strcpy(usersfromDB[i].club,user->club);
                    strcpy(usersfromDB[i].abroadexp,user->abroadexp);
                    strcpy(usersfromDB[i].milserv,user->milserv);
                    unreadmsg[i]=a;
                }
                char messagepart[1024];
                char endtoken[20]="&";
               // int size=strlen(messagepart);
                strcpy(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 1024\r\nContent-Type:text/plain\r\n\r\n");
                for (int i =0;i<numchattingroom;i++)
                {
                    sprintf(messagepart,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%d|",usersfromDB[i].id,usersfromDB[i].password,usersfromDB[i].email,usersfromDB[i].name,usersfromDB[i].sex,usersfromDB[i].statusmsg,usersfromDB[i].age,usersfromDB[i].height,usersfromDB[i].address,usersfromDB[i].hobby,usersfromDB[i].college,usersfromDB[i].major,usersfromDB[i].imageURL,usersfromDB[i].religion,usersfromDB[i].club,usersfromDB[i].abroadexp,usersfromDB[i].milserv,unreadmsg[i]);
                    
                    puts("PASS\n");
                    if(strcat(response_packet,messagepart)==NULL)
                        error_handling("strcat() error in response_packet for chattinglist\n");
                }
                memset(messagepart,0,sizeof(messagepart));
                numwaitingQ = 0;
                // get profile information for all connection request in waiting queue that matches requesting ID(user->ID)
                for (int i=0;i<numwaitingQ;i++)
                {
                    strcpy(usersfromDB[i].id,user->id);
                    strcpy(usersfromDB[i].password,user->password);
                    strcpy(usersfromDB[i].email,user->email);
                    strcpy(usersfromDB[i].name,user->name);
                    strcpy(usersfromDB[i].sex,user->sex);
                    strcpy(usersfromDB[i].statusmsg,user->statusmsg);
                    strcpy(usersfromDB[i].age,user->age);
                    strcpy(usersfromDB[i].height,user->height);
                    strcpy(usersfromDB[i].address,user->address);
                    strcpy(usersfromDB[i].hobby,user->hobby);
                    strcpy(usersfromDB[i].college,user->college);
                    strcpy(usersfromDB[i].major,user->major);
                    strcpy(usersfromDB[i].imageURL,user->imageURL);
                    strcpy(usersfromDB[i].religion,user->religion);
                    strcpy(usersfromDB[i].club,user->club);
                    strcpy(usersfromDB[i].abroadexp,user->abroadexp);
                    strcpy(usersfromDB[i].milserv,user->milserv);
                }
                for (int i =0;i<numchattingroom;i++)
                {
                    sprintf(messagepart,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$1|",usersfromDB[i].id,usersfromDB[i].password,usersfromDB[i].email,usersfromDB[i].name,usersfromDB[i].sex,usersfromDB[i].statusmsg,usersfromDB[i].age,usersfromDB[i].height,usersfromDB[i].address,usersfromDB[i].hobby,usersfromDB[i].college,usersfromDB[i].major,usersfromDB[i].imageURL,usersfromDB[i].religion,usersfromDB[i].club,usersfromDB[i].abroadexp,usersfromDB[i].milserv);
                    
                    puts("PASS\n");
                    if(strcat(response_packet,messagepart)==NULL)
                        error_handling("strcat() error in response_packet for chattinglist\n");
                }
                if(strncat(response_packet,endtoken,1)==NULL)
                    error_handling("strcat() error in response_packet for chattinglist\n");
            }
            break;
        case '8' :      //chatting accept request message
        	
        	//make WAITING QUEUE in DB
        	
        	
        	
            strcpy(header, strtok(message,"$"));
            strcpy(chatinfo->s_id,strtok(NULL,"$"));
            strcpy(chatinfo->d_id,strtok(NULL,"$"));
            dbputok=1;
            //PUT DB this information. with name " WAITING QUEUE"
            if (dbputok==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            else
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            break;
        case '9' :      //chatting accept response message
            strcpy(header, strtok(message,"$"));
            strcpy(chatinfo->s_id,strtok(NULL,"$"));
            strcpy(chatinfo->d_id,strtok(NULL,"$"));
            strcpy(chatinfo->acceptance,strtok(NULL,"$"));
            
            dbputok=1; //CHANGE DB : change table of the connection info above. "WAITING QUEUE" to "CHATTING LIST"
            if (dbputok==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            else if( strcmp("0",chatinfo->acceptance)==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nREJECT&");
            else if( strcmp("1",chatinfo->acceptance)==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nACCEPT");
            else
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nSENT_WRONG&");
            break;
            
        case 'a' :      //chat : send txt
            strcpy(header,strtok(message,"$"));
            strcpy(chatinfo->s_id,strtok(NULL,"$"));
            strcpy(chatinfo->d_id,strtok(NULL,"$"));
            strcpy(chatinfo->textmsg,strtok(NULL,"$"));
            //put information to DB and raise count of unread msg;
            dbputok=1; 
            if (dbputok==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            else
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            break;
        case 'b' :      //chat : send image
            strcpy(header,strtok(message,"$"));
            strcpy(chatinfo->s_id,strtok(NULL,"$"));
            strcpy(chatinfo->d_id,strtok(NULL,"$"));
            strcpy(chatinfo->image,strtok(NULL,"$"));
            //put information to DB and raise count of unread msg;
            dbputok=1; 
            if (dbputok==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            else
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            break;
        case 'c' :      //update profile message
            
            strcpy(header, strtok(message,"$"));   
            user_info newuserinfo={0,};
            strcpy(newuserinfo.id,strtok(NULL,"$"));
            strcpy(newuserinfo.password,strtok(NULL,"$"));
            strcpy(newuserinfo.email, strtok(NULL,"$"));
            strcpy(newuserinfo.name, strtok(NULL,"$"));
            strcpy(newuserinfo.sex, strtok(NULL,"$"));
            strcpy(newuserinfo.statusmsg, strtok(NULL,"$"));
            strcpy(newuserinfo.age, strtok(NULL,"$"));
            strcpy(newuserinfo.height, strtok(NULL,"$"));
            strcpy(newuserinfo.address, strtok(NULL,"$"));
            strcpy(newuserinfo.hobby, strtok(NULL,"$"));
            strcpy(newuserinfo.college, strtok(NULL,"$"));
            strcpy(newuserinfo.major, strtok(NULL,"$"));
            strcpy(newuserinfo.imageURL, strtok(NULL,"$"));
            strcpy(newuserinfo.religion, strtok(NULL,"$"));
            strcpy(newuserinfo.club, strtok(NULL,"$"));
            strcpy(newuserinfo.abroadexp, strtok(NULL,"$"));
            strcpy(newuserinfo.milserv, strtok(NULL,"$"));
            //put DB new information
            
            int dbputok=1; 
            if (dbputok==0)
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
            else
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            break;
    }
    
    mysql_free_result(sql_res);
    mysql_close(&mysql);
}


// char* Tokenizer(char* original, char* search, char token)
// {
//     char* result;
//     char* temp;
//     char* endofstr;
//     temp = strstr(original,search);
//     endofstr = strchr(temp,token);
//     
//     strncpy(result,temp,(endofstr-temp)/4);
//     return result;
// }


