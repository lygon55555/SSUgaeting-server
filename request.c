#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include </usr/include/mysql/mysql.h>
#include "http-parser.h"
#include "request.h"
#include <sys/socket.h>



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
    char message[5120]={0,};
    char ct[15];
    char response_packet[5120] ={0,};
     
    read(clnt_sock,header,sizeof(header));
    
    puts(header);


    char* a = strtok(header,"&");


    strcpy(message,strtok(NULL,"&"));  


     parseHttpHeader(parseHttpRequestLine(header, &req_line), &fields);
     
     parse_message(clnt_sock,message, &user, &chatinfo, response_packet);

    write(clnt_sock,response_packet,sizeof(response_packet));
    puts(response_packet);

     close(clnt_sock);

}



void error_handling(char* message)
{
    fputs(message,stderr);
    fputs("\n",stderr);
    exit(1);
}

void parse_message(int clnt_sock,char* message, user_info* user, chat* chatinfo, char* response_packet)
{
	MYSQL mysql;
	MYSQL_RES* sql_res;
	MYSQL_ROW sql_row;
	int fields = 0;

	mysql_init(&mysql);
	if(!mysql_real_connect(&mysql, "localhost", "root", NULL, "ssugaeting", 3306, (char *) NULL, 0))
	{
		printf("MYSQL CONNECTION FAILED...\n");
		printf("%s\n", mysql_error(&mysql));
		exit(1);
	}
	else
		printf("MYSQL CONNECTED\n");
		
    char header[10];
 

    user_info temp = {0,};
    int numchattingroom=0;
    int numwaitingQ=0;
    int dbputok=0;
    char query[2048] = {'\0'};
    char filtermsg[11][30]={0,};
    char tempbuf[100] = {0,};
    char endtoken[3]="&";

    switch(message[0])
    {

    	
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
            strcpy(user->email , strtok(NULL,"$"));            
            strcpy(user->name ,strtok(NULL,"$"));
            strcpy(user->sex , strtok(NULL,"$"));
            
            sprintf(query, "INSERT INTO profile (id, password, email, name, sex, stateMsg, age, height, address, hobby, college, major, imageURI, religion, circle, abroadExperience, militaryStatus) VALUES ('%s', '%s', '%s', '%s', '%s', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ')", user->id, user->password, user->email, user->name, user->sex);
            
            if(mysql_query(&mysql, query))
			{
				printf("1 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
		
            sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
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
            printf("ID : %s\n",temp.id);
            
            strcpy(temp.password , strtok(NULL,"$"));
            printf("PASSWORD : %s\n",temp.password);
            
            sprintf(query, "SELECT * FROM profile WHERE id = '%s' and password = '%s'", temp.id, temp.password);
            
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
					sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:text/plain\r\n\r\n%s",
					len,asdf);
					puts(response_packet);
				}
            }
            

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
					sprintf(asdf,"%s&", sql_row[0]);
					int len = strlen(asdf);
					sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:text/plain\r\n\r\n%s&",len, sql_row[0]);
				}
            }

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
            
			sprintf(query, "SELECT * FROM profile WHERE id = '%s' and email = '%s'", temp.id, temp.email);
			
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
					sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
				}
			}

                
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
				       	
            strcpy(header, strtok(message,"$"));
            strcpy(user->id, strtok(NULL,"$"));
            strcpy(user->password, strtok(NULL,"$"));
            
            sprintf(query, "UPDATE profile SET password = '%s' WHERE id = '%s'", user->password, user->id);
            
            if(mysql_query(&mysql, query))
			{
				printf("5 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
			{
				printf("password changed\n");
				sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            }
                       
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
            
            
            
            strcpy(header, strtok(message,"$"));
            strcpy(filtermsg[0],strtok(NULL,"$"));  //mysex
            strcpy(filtermsg[1],strtok(NULL,"$"));  //agefrom
            strcpy(filtermsg[2],strtok(NULL,"$"));  //ageto
            strcpy(filtermsg[3],strtok(NULL,"$"));  //heightfrom
            strcpy(filtermsg[4],strtok(NULL,"$"));  //heightto
            strcpy(filtermsg[5],strtok(NULL,"$"));  //religion
            strcpy(filtermsg[6],strtok(NULL,"$"));  //hobby
            strcpy(filtermsg[7],strtok(NULL,"$"));  //college
            strcpy(filtermsg[8],strtok(NULL,"$"));  //club
            strcpy(filtermsg[9],strtok(NULL,"$"));  //abroadexp
            strcpy(filtermsg[10],strtok(NULL,"$")); //milserv
            
            sprintf(query, "SELECT * FROM profile WHERE ");
            
            for (int i =0;i<11;i++){
                
                if(!strcmp(filtermsg[i],"none"))
                {
                    continue;
                }
                else
                {
                    if(i==0){
                        sprintf(tempbuf,"sex = '%s' and ",filtermsg[0]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else if(i==1){
                        sprintf(tempbuf,"'%s' < age < '%s' and ",filtermsg[1],filtermsg[2]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else if(i==3){
                        sprintf(tempbuf,"'%s' < height < '%s' and ",filtermsg[3],filtermsg[4]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else if(i==5){
                        sprintf(tempbuf,"religion = '%s' and ",filtermsg[5]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else if(i==6){
                        sprintf(tempbuf,"hobby = '%s' and ",filtermsg[6]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else if(i==7){
                        sprintf(tempbuf,"college = '%s' and ",filtermsg[7]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else if(i==8){
                        sprintf(tempbuf,"circle = '%s' and ",filtermsg[8]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else if(i==9){
                        sprintf(tempbuf,"abroadExperience = '%s' and ",filtermsg[9]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else if(i==10){
                        sprintf(tempbuf,"militaryStatus = '%s' and ",filtermsg[5]);
                        strncat(query,tempbuf,strlen(tempbuf));
                    }
                    else
                        continue;
                }
            }
            
            query[strlen(query)-4]='\0';
            printf("FILTER QUERY : %s\n", query);
            if(mysql_query(&mysql, query))
			{
				printf("6 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
			{
          		sql_res = mysql_store_result(&mysql);
                fields = mysql_num_rows(sql_res);
                
          		if(fields == 0)
          		{
          			printf("no data found in DB (no filtered profile)\n");
                    sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
				}
				else
                {
                    user_info usersfromDB[fields];
                    int i = 0;
                    while( (sql_row = mysql_fetch_row(sql_res)) != NULL )
                    {
                        //put all info to "user_info usersfromDB"
                        
                        strcpy(usersfromDB[i].id,sql_row[0]);
                        strcpy(usersfromDB[i].password,sql_row[1]);
                        strcpy(usersfromDB[i].email,sql_row[2]);
                        strcpy(usersfromDB[i].name,sql_row[3]);
                        strcpy(usersfromDB[i].sex,sql_row[4]);
                        strcpy(usersfromDB[i].statusmsg,sql_row[5]);
                        strcpy(usersfromDB[i].age,sql_row[6]);
                        strcpy(usersfromDB[i].height,sql_row[7]);
                        strcpy(usersfromDB[i].address,sql_row[8]);
                        strcpy(usersfromDB[i].hobby,sql_row[9]);
                        strcpy(usersfromDB[i].college,sql_row[10]);
                        strcpy(usersfromDB[i].major,sql_row[11]);
                        strcpy(usersfromDB[i].imageURL,sql_row[12]);
                        strcpy(usersfromDB[i].religion,sql_row[13]);
                        strcpy(usersfromDB[i].club,sql_row[14]);
                        strcpy(usersfromDB[i].abroadexp,sql_row[15]);
                        strcpy(usersfromDB[i].milserv,sql_row[16]);
                        
                        i++;
                    }
                    
                        char asdf[1024];
                        char endtoken[20]="&";
                        
                        //int size=strlen(asdf);
                        strcpy(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\n");
                        
                        for (int i = 0; i < fields; i++)
                        {
                            sprintf(asdf,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s|",
                            usersfromDB[i].id,usersfromDB[i].password,usersfromDB[i].email,usersfromDB[i].name,usersfromDB[i].sex,usersfromDB[i].statusmsg,usersfromDB[i].age,usersfromDB[i].height,usersfromDB[i].address,usersfromDB[i].hobby,usersfromDB[i].college,usersfromDB[i].major,usersfromDB[i].imageURL,usersfromDB[i].religion,usersfromDB[i].club,usersfromDB[i].abroadexp,usersfromDB[i].milserv);

                            if(strcat(response_packet,asdf)==NULL)
                                error_handling("strcat() error in response_packet for filter\n");

                        }
                        
                        
                        if(strncat(response_packet,endtoken,1)==NULL)
                            error_handling("strcat() error in response_packet for chattinglist\n");
                        
				}
			}
            break;
        case '7' :      //current chatting list message
            strcpy(header, strtok(message,"$"));
            strcpy(user->id,strtok(NULL,"$"));
            int nochat=1;
            sprintf(query, "SELECT * FROM chatroom WHERE id1 = '%s' or id2 = '%s'", user->id, user->id);
            strcpy(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\n");
            if(mysql_query(&mysql, query))
			{
				printf("7 query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
			{
          		sql_res = mysql_store_result(&mysql);
                fields = mysql_num_rows(sql_res);
                
          		if(fields == 0)
          		{
          			printf("no data found in DB (no chatroom)\n");
                    nochat =0;
				}
				else
                {
                    printf("how many chatroom : %d\n", fields);
                            //put all info to "user_info usersfromDB"
                    user_info usersfromDB[fields];
                    
                    sprintf(query, "SELECT * FROM profile WHERE ");
                    
                    while( (sql_row = mysql_fetch_row(sql_res)) != NULL)
                    {
                        if(strcmp(sql_row[0],user->id))
                        {
                            sprintf(tempbuf,"id = '%s' or ", sql_row[0]);
                            strncat(query,tempbuf,strlen(tempbuf));
                        }
                        else
                        {
                            sprintf(tempbuf,"id = '%s' or ", sql_row[1]);
                            strncat(query,tempbuf,strlen(tempbuf));
                        }
                    }
                    query[strlen(query)-3]='\0';
                    mysql_free_result(sql_res);
                    
                    
                    if(mysql_query(&mysql, query))
                    {
                        printf("7 query failed...\n");
                        printf("%s\n", mysql_error(&mysql));
                        exit(1);
                    }
                    else
                    {
                        sql_res = mysql_store_result(&mysql);
                        
                        int i = 0;
                        while( (sql_row = mysql_fetch_row(sql_res)) != NULL )
                        {   
                            strcpy(usersfromDB[i].id,sql_row[0]);
                            strcpy(usersfromDB[i].password,sql_row[1]);
                            strcpy(usersfromDB[i].email,sql_row[2]);
                            strcpy(usersfromDB[i].name,sql_row[3]);
                            strcpy(usersfromDB[i].sex,sql_row[4]);
                            strcpy(usersfromDB[i].statusmsg,sql_row[5]);
                            strcpy(usersfromDB[i].age,sql_row[6]);
                            strcpy(usersfromDB[i].height,sql_row[7]);
                            strcpy(usersfromDB[i].address,sql_row[8]);
                            strcpy(usersfromDB[i].hobby,sql_row[9]);
                            strcpy(usersfromDB[i].college,sql_row[10]);
                            strcpy(usersfromDB[i].major,sql_row[11]);
                            strcpy(usersfromDB[i].imageURL,sql_row[12]);
                            strcpy(usersfromDB[i].religion,sql_row[13]);
                            strcpy(usersfromDB[i].club,sql_row[14]);
                            strcpy(usersfromDB[i].abroadexp,sql_row[15]);
                            strcpy(usersfromDB[i].milserv,sql_row[16]);
                            
                            i++;
                        }
                        char messagepart[1024];
                        
                    // int size=strlen(messagepart);
                        strcpy(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\n");
                        for (int i =0;i<fields;i++)
                        {
                            sprintf(messagepart,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$|",usersfromDB[i].id,usersfromDB[i].password,usersfromDB[i].email,usersfromDB[i].name,usersfromDB[i].sex,usersfromDB[i].statusmsg,usersfromDB[i].age,usersfromDB[i].height,usersfromDB[i].address,usersfromDB[i].hobby,usersfromDB[i].college,usersfromDB[i].major,usersfromDB[i].imageURL,usersfromDB[i].religion,usersfromDB[i].club,usersfromDB[i].abroadexp,usersfromDB[i].milserv);
                            
                            puts("PASS\n");
                            if(strcat(response_packet,messagepart)==NULL)
                                error_handling("strcat() error in response_packet for chattinglist\n");
                        }
                            if(strncat(response_packet,endtoken,1)==NULL)
                            error_handling("strcat() error in response_packet for chattinglist\n");
                    }
                }    
            }
            
            
            // GET INFORMATION FROM WAITING QUEUE
            
            
            
            
            
            
            sprintf(query, "SELECT * FROM waitq WHERE sendid = '%s' or recvid = '%s'", user->id, user->id);
            
            if(mysql_query(&mysql, query))
			{
				printf("7 WAITING QUEUE query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
			{
          		sql_res = mysql_store_result(&mysql);
                fields = mysql_num_rows(sql_res);
                
          		if(fields == 0)
          		{
          			printf("no data found in DB (no WAITING QUEUE)\n");
                    if(nochat ==0)
                        sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
				}
				else
                {
                    printf("HOW MANY WAITING : %d\n", fields);
                            //put all info to "user_info usersfromDB"
                    user_info usersfromDB[fields];
                    
                    sprintf(query, "SELECT * FROM profile WHERE ");
                    
                    while( (sql_row = mysql_fetch_row(sql_res)) != NULL)
                    {
                        if(strcmp(sql_row[0],user->id))
                        {
                            sprintf(tempbuf,"id = '%s' or ", sql_row[0]);
                            strncat(query,tempbuf,strlen(tempbuf));
                        }
                        else
                        {
                            sprintf(tempbuf,"id = '%s' or ", sql_row[1]);
                            strncat(query,tempbuf,strlen(tempbuf));
                        }
                    }
                    query[strlen(query)-3]='\0';
                    mysql_free_result(sql_res);
                    
                    if(mysql_query(&mysql, query))
                    {
                        printf("7 query failed...\n");
                        printf("%s\n", mysql_error(&mysql));
                        exit(1);
                    }
                    else
                    {
                        sql_res = mysql_store_result(&mysql);
                        
                        int i = 0;
                        while( (sql_row = mysql_fetch_row(sql_res)) != NULL )
                        {   
                            strcpy(usersfromDB[i].id,sql_row[0]);
                            strcpy(usersfromDB[i].password,sql_row[1]);
                            strcpy(usersfromDB[i].email,sql_row[2]);
                            strcpy(usersfromDB[i].name,sql_row[3]);
                            strcpy(usersfromDB[i].sex,sql_row[4]);
                            strcpy(usersfromDB[i].statusmsg,sql_row[5]);
                            strcpy(usersfromDB[i].age,sql_row[6]);
                            strcpy(usersfromDB[i].height,sql_row[7]);
                            strcpy(usersfromDB[i].address,sql_row[8]);
                            strcpy(usersfromDB[i].hobby,sql_row[9]);
                            strcpy(usersfromDB[i].college,sql_row[10]);
                            strcpy(usersfromDB[i].major,sql_row[11]);
                            strcpy(usersfromDB[i].imageURL,sql_row[12]);
                            strcpy(usersfromDB[i].religion,sql_row[13]);
                            strcpy(usersfromDB[i].club,sql_row[14]);
                            strcpy(usersfromDB[i].abroadexp,sql_row[15]);
                            strcpy(usersfromDB[i].milserv,sql_row[16]);
                            
                            i++;
                        }
                        char messagepart[1024];
                        
                    // int size=strlen(messagepart);
                        strcpy(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\n");
                        for (int i =0;i<fields;i++)
                        {
                            sprintf(messagepart,"%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$%s$|",usersfromDB[i].id,usersfromDB[i].password,usersfromDB[i].email,usersfromDB[i].name,usersfromDB[i].sex,usersfromDB[i].statusmsg,usersfromDB[i].age,usersfromDB[i].height,usersfromDB[i].address,usersfromDB[i].hobby,usersfromDB[i].college,usersfromDB[i].major,usersfromDB[i].imageURL,usersfromDB[i].religion,usersfromDB[i].club,usersfromDB[i].abroadexp,usersfromDB[i].milserv);
                            
                            if(strcat(response_packet,messagepart)==NULL)
                                error_handling("strcat() error in response_packet for chattinglist\n");
                        }
                            if(strncat(response_packet,endtoken,1)==NULL)
                            error_handling("strcat() error in response_packet for chattinglist\n");
                    }
                }    
            }
            break;
        case '8' :      //chatting accept request message
        	strcpy(header, strtok(message,"$"));
            strcpy(chatinfo->s_id,strtok(NULL,"$"));
            strcpy(chatinfo->d_id,strtok(NULL,"$"));
            
            sprintf(query, "INSERT INTO waitq (sendid, recvid) VALUES ('%s', '%s')", chatinfo->s_id, chatinfo->d_id);
            
            if(mysql_query(&mysql, query))
            {
                printf("8 query failed...\n");
                printf("%s\n", mysql_error(&mysql));
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
                exit(1);
            }
            else
            {
                printf("WAITING QUEUE COMPLETE\n");
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            }
            break;
        case '9' :      //chatting accept response message
            strcpy(header, strtok(message,"$"));
            strcpy(chatinfo->s_id,strtok(NULL,"$"));
            strcpy(chatinfo->d_id,strtok(NULL,"$"));
            strcpy(chatinfo->acceptance,strtok(NULL,"$"));
            
            sprintf(query, "SELECT * FROM waitq WHERE sendid = '%s' or recvid = '%s'", chatinfo->d_id, chatinfo->s_id);
            
            if(mysql_query(&mysql, query))
            {
                printf("9 query failed...\n");
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
            else
            {
            	sql_res = mysql_store_result(&mysql);
            	fields = mysql_num_rows(sql_res);
                
                printf(" case 9 , fields : %d\n", fields);
                printf("%s\n", chatinfo->acceptance);
            	
            	if(fields == 0)
            	{
            		printf("NO WAITING QUEUE\n");
            		sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
                }
            	else if(!strcmp("0",chatinfo->acceptance))
            	{
            		printf("CHATTING REJECT\n");
            		sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\nREJECT&");
            		
            		sprintf(query, "DELETE FROM waitq WHERE sendid = '%s' and recvid = '%s'", chatinfo->d_id, chatinfo->s_id);
            		
                        if(mysql_query(&mysql, query))
                        {
                            printf("WAITING QUEUE DELETE FAILED...\n");
                            printf("%s\n", mysql_error(&mysql));
                            exit(1);
                        }
                        else
                            printf("WAITING QUEUE DELETE COMPLETE\n");
            	}
            	else if(!strcmp("1",chatinfo->acceptance))
            	{
            		printf("CHATTING ACCEPT\n");
            		sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\nACCEPT&");
            		
            		sprintf(query, "DELETE FROM waitq WHERE sendid = '%s' and recvid = '%s'", chatinfo->d_id, chatinfo->s_id);
            		
                        if(mysql_query(&mysql, query))
                        {
                            printf("WAITING QUEUE DELETE FAILED...\n");
                            printf("%s\n", mysql_error(&mysql));
                            exit(1);
                        }
                        else
                            printf("WAITING QUEUE DELETE COMPLETE\n");
                        
                        sprintf(query, "INSERT INTO chatroom (id1, id2) VALUES ('%s', '%s')", chatinfo->s_id, chatinfo->d_id);
                        
                        if(mysql_query(&mysql, query))
                        {
                            printf("CHATROOOM QUERY FAILED...\n");
                            printf("%s\n", mysql_error(&mysql));
                            exit(1);
                        }
                        else
                        {
                            printf("CHATROOM MADE\n");
                            
                            sprintf(query, "INSERT INTO chatmsg (sendid, recvid, msg, time, image) VALUES ('%s', '%s', '%s', ' ', ' ')", chatinfo->s_id, chatinfo->d_id, "CONNECTED");
                            
                            if(mysql_query(&mysql, query))
                            {
                                printf("CHATMSG QUERY FAILED...\n");
                                printf("%s\n", mysql_error(&mysql));
                                exit(1);
                            }
                            else
                                printf("CONNECTED CHATTING MESSAGE SENT\n");
                            
                            sprintf(query, "INSERT INTO chatmsg (sendid, recvid, msg, time, image) VALUES ('%s', '%s', '%s', ' ', ' ')", chatinfo->d_id, chatinfo->s_id, "CONNECTED");
                            
                            if(mysql_query(&mysql, query))
                            {
                                printf("CHATMSG QUERY FAILED...\n");
                                printf("%s\n", mysql_error(&mysql));
                                exit(1);
                            }
                            else
                                printf("CONNECTED CHATTING MESSAGE SENT\n");
                        }
            	}
            	else
            		sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\nSENT_WRONG&");
            }
            break;
            
        case 'a' :      //chat : send txt
            strcpy(header,strtok(message,"$"));
            strcpy(chatinfo->s_id,strtok(NULL,"$"));
            strcpy(chatinfo->d_id,strtok(NULL,"$"));
            strcpy(chatinfo->textmsg,strtok(NULL,"$"));
            strcpy(chatinfo->image,strtok(NULL,"$"));
            strcpy(chatinfo->time,strtok(NULL,"$"));
            //put information to DB and raise count of unread msg;
             sprintf(query, "INSERT INTO chatmsg (sendid, recvid, msg, image, time) VALUES ('%s', '%s', '%s', '%s', '%s')", chatinfo->s_id, chatinfo->d_id, chatinfo->textmsg, chatinfo->image, chatinfo->time);
					
					if(mysql_query(&mysql, query))
					{
						printf("a query failed...\n");
						printf("%s\n", mysql_error(&mysql));
						exit(1);
					}
					else
					{
						printf("unread message insertion complete\n");
                        sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
					}
  
            break;
        case 'b' :      //chat : send image
            strcpy(header,strtok(message,"$"));
            strcpy(chatinfo->s_id,strtok(NULL,"$"));
            strcpy(chatinfo->d_id,strtok(NULL,"$"));
            strcpy(chatinfo->textmsg,strtok(NULL,"$"));
            strcpy(chatinfo->image,strtok(NULL,"$"));
            strcpy(chatinfo->time,strtok(NULL,"$"));
            //put information to DB and raise count of unread msg;
             sprintf(query, "INSERT INTO chatmsg (sendid, recvid, msg, image, time) VALUES ('%s', '%s', '%s', '%s', '%s')", chatinfo->s_id, chatinfo->d_id, chatinfo->textmsg, chatinfo->image, chatinfo->time);
					
					if(mysql_query(&mysql, query))
					{
						printf("b query failed...\n");
						printf("%s\n", mysql_error(&mysql));
						exit(1);
					}
					else
					{
						printf("unread message insertion complete\n");
                        sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
					}
            //put information to DB and raise count of unread msg;

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
            
            sprintf(query, "UPDATE profile SET name = '%s', stateMsg = '%s', age = '%s', height = '%s', address = '%s', hobby = '%s', college = '%s', major = '%s', imageURI = '%s', religion = '%s', circle = '%s', abroadExperience = '%s', militaryStatus = '%s' WHERE id = '%s'", newuserinfo.name, newuserinfo.statusmsg, newuserinfo.age, newuserinfo.height, newuserinfo.address, newuserinfo.hobby, newuserinfo.college, newuserinfo.major, newuserinfo.imageURL, newuserinfo.religion, newuserinfo.club, newuserinfo.abroadexp, newuserinfo.milserv, newuserinfo.id);
            
            if(mysql_query(&mysql, query))
            {
                printf("c query failed...\n");
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
            else
            {
                printf("PROFILE UPDATE COMPLETE\n");
                sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nOK&");
            }
            break;
            
            
        case 'd' :      //unread msg
            strcpy(header, strtok(message,"$"));  
            strcpy(chatinfo->s_id, strtok(NULL,"$"));  
            
            
            //get unreadmsg 
            sprintf(query, "SELECT * FROM chatmsg WHERE recvid = '%s'", chatinfo->s_id);
            strcpy(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\n");
            
            if(mysql_query(&mysql, query))
            {
                printf("d query failed...\n");
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
            else
            {
                sql_res = mysql_store_result(&mysql);
                fields = mysql_num_rows(sql_res);      //number of messages
                
                if(fields == 0)
                {
                    printf("no data found in DB (no chat message)\n");
                    sprintf(query, "SELECT * FROM chatroom WHERE id1 = '%s' or id2 = '%s'", chatinfo->s_id, chatinfo->s_id);

                        if(mysql_query(&mysql, query))
                        {
                            printf("7 query failed...\n");
                            printf("%s\n", mysql_error(&mysql));
                            exit(1);
                        }
                        else
                        {
                            sql_res = mysql_store_result(&mysql);
                            fields = mysql_num_rows(sql_res);

                
                            if(fields == 0)
                            {
                                printf("no data found in DB (no chatroom)\n");
                                nochat =0;
                            }
                            else{
                                while( (sql_row = mysql_fetch_row(sql_res)) !=NULL )
                                {
                                    if(strcmp(sql_row[0],chatinfo->s_id))
                                    {
                                        sprintf(tempbuf,"%s$ $ $ |", sql_row[0]);
                                        strncat(response_packet,tempbuf,strlen(tempbuf));
                                    }
                                    else
                                    {
                                        sprintf(tempbuf,"%s$ $ $ |", sql_row[1]);
                                        strncat(response_packet,tempbuf,strlen(tempbuf));
                                    }
                                }
                            }
                        }
                }
                else            
                {
                    char tempid[20] = {0,};
                    sprintf(query, "SELECT * FROM chatmsg WHERE recvid = '%s' ORDER BY sendid asc", chatinfo->s_id);
                    
                    if(mysql_query(&mysql, query))
                    {
                        printf("d query failed...\n");
                        printf("%s\n", mysql_error(&mysql));
                        exit(1);                     
                    }
                    else
                    {
                        mysql_free_result(sql_res);
                        sql_res = mysql_store_result(&mysql);
                        fields = mysql_num_rows(sql_res);
                        
                        sql_row = mysql_fetch_row(sql_res);
                        strcpy(tempid, sql_row[0]);
                        sprintf(tempbuf,"%s$%s$%s$%s",tempid,sql_row[2],sql_row[4],sql_row[3]);
                        strcat(response_packet,tempbuf);
                        while( (sql_row = mysql_fetch_row(sql_res)) != NULL )
                        {
                            if(strcmp(tempid, sql_row[0]))
                            {
                                strcpy(tempid, sql_row[0]);
                                sprintf(tempbuf,"|%s$%s$%s$%s",tempid,sql_row[2],sql_row[4],sql_row[3]);
                                strcat(response_packet,tempbuf);
                                //id add to message
                            }
                            sprintf(tempbuf,"$%s$%s$%s",sql_row[2],sql_row[4],sql_row[3]);
                            strcat(response_packet,tempbuf);
                            
                            // 2, 3, 4 add
                            //message combine
                        }
                        
                        sprintf(query, "SELECT * FROM chatroom WHERE id1 = '%s' or id2 = '%s'", chatinfo->s_id, chatinfo->s_id);
                        if(mysql_query(&mysql, query))
                        {
                            printf("7 query failed...\n");
                            printf("%s\n", mysql_error(&mysql));
                            exit(1);
                        }
                        else
                        {
                            sql_res = mysql_store_result(&mysql);
                            fields = mysql_num_rows(sql_res);
                            if(fields == 0)
                            {
                                printf("no data found in DB (no chatroom)\n");
                                nochat =0;
                            }
                            else{
                                while( (sql_row = mysql_fetch_row(sql_res)) !=NULL )
                                {
                                    if(strcmp(sql_row[0],chatinfo->s_id))
                                    {
                                        sprintf(tempbuf,"|%s$ $ $ ", sql_row[0]);
                                        strncat(response_packet,tempbuf,strlen(tempbuf));
                                    }
                                    else
                                    {
                                        sprintf(tempbuf,"|%s$ $ $ ", sql_row[1]);
                                        strncat(response_packet,tempbuf,strlen(tempbuf));
                                    }
                                }
                            }
                        }
                    }
                }
            }
                
                
                //GET INFORMATION FROM WAITING QUEUE
                
            sprintf(query, "SELECT * FROM waitq WHERE sendid = '%s' or recvid = '%s'", chatinfo->s_id, chatinfo->s_id);
            
            if(mysql_query(&mysql, query))
			{
				printf("d WAITING QUEUE query failed...\n");
				printf("%s\n", mysql_error(&mysql));
				exit(1);
			}
			else
			{
          		sql_res = mysql_store_result(&mysql);
                fields = mysql_num_rows(sql_res);
                
          		if(fields == 0)
          		{
          			printf("no data found in DB (NO WAITING QUEUE)\n");
				}
				else
                {
                    printf("HOW MANY WAITING : %d\n", fields);
                            //put all info to "user_info usersfromDB"
                    user_info usersfromDB[fields];
 
                    
                    while( (sql_row = mysql_fetch_row(sql_res)) !=NULL )
                    {
                        if(strcmp(sql_row[0],chatinfo->s_id))
                        {
                            sprintf(tempbuf,"|%s$ $ $ ", sql_row[0]);
                            strncat(response_packet,tempbuf,strlen(tempbuf));
                        }
                        else
                        {
                            sprintf(tempbuf,"|%s$ $ $ ", sql_row[1]);
                            strncat(response_packet,tempbuf,strlen(tempbuf));
                        }
                    }
                }
            }
            
            if(strncat(response_packet,endtoken,1)==NULL)
                error_handling("strcat() error in response_packet for chattinglist\n");
           
           break;
        case 'e' :      //get msg inside the chatting room
            strcpy(header, strtok(message,"$"));  
            strcpy(chatinfo->s_id, strtok(NULL,"$")); 
            strcpy(chatinfo->d_id, strtok(NULL,"$"));
            
            strcpy(tempbuf,"\0");
            

	          	sprintf(query, "SELECT * FROM chatmsg WHERE sendid = '%s' and recvid = '%s'", chatinfo->d_id, chatinfo->s_id);
	          	puts(query);
	          	if(mysql_query(&mysql, query))
	          	{
	          		printf("e query failed...\n");
	          		printf("%s\n", mysql_error(&mysql));
	          		exit(1);
	          	}
	          	else
	          	{
	          		sql_res = mysql_store_result(&mysql);
	                fields = mysql_num_rows(sql_res);
	                char tempid[20] = {0,};
	          		if(fields == 0)  //no unread message
	          		{
	          			printf("no data found in DB (no chat message)\n");
                        sprintf(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type:text/plain\r\n\r\nFAIL&");
	          	//		continue;	                    
					}
					else	//unread message exists
					{
                        strcpy(response_packet,"HTTP/1.1 200 OK\r\nContent-Length: 2048\r\nContent-Type:text/plain\r\n\r\n");
						while( (sql_row = mysql_fetch_row(sql_res))!=NULL )
						{
							// make response_packet
                                sprintf(tempbuf,"%s$%s$%s$",sql_row[2],sql_row[4],sql_row[3]);
                                strcat(response_packet,tempbuf);
                        
						}
						if(strncat(response_packet,endtoken,1)==NULL)
                            error_handling("strcat() error in response_packet for e\n");
                        
                        
                        sprintf(query, "DELETE FROM chatmsg WHERE sendid = '%s' and recvid = '%s'", chatinfo->d_id, chatinfo->s_id);
                        if(mysql_query(&mysql, query))
                        {
                            printf("DELETE FAILED...\n");
                            printf("%s\n", mysql_error(&mysql));
                            exit(1);
                        }
                        else
                            printf("DELETE COMPLETE\n"); 
					}	          		
	          	 }
           // }
                break;
        }
            
    mysql_free_result(sql_res);
    mysql_close(&mysql);
}



