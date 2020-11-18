#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#define PORT 8900
#define BUF_SIZE 2048
int execute(char* command,char* buf);
int main()
{
	int sockfd;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	char cmd[2048];
	int sendnum;
	int recvnum;
	int length;
	int port;
	int connected;	//change
	memset(send_buf,0,2048);
	memset(recv_buf,0,2048);
	memset(cmd,0,2048);  //change
	port = PORT;
	if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))  //change
	{
		perror("generating socket error\n");
		exit(1);
	}
	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	if (-1==bind(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))
	{
		perror("binding error\n");
		close(sockfd);
		exit(1);
	}
	if(-1==listen(sockfd,10))
	{
		perror("listen socket error\n");
		close(sockfd);
		return -1;
	}
	length = sizeof(struct sockaddr_in);
	while(1)
	{
		memset(recv_buf,0,2048);
		memset(send_buf,0,2048);
		if(-1==(connected=accept(sockfd,(struct sockaddr*)&client, &length)))
		{
			perror("three shakehands error\n");
			close(sockfd);
			return -1;
		}
		while(1)      //change
		{
			memset(recv_buf,0,2048);
            memset(send_buf,0,2048);
			recvnum = recv(connected, recv_buf, sizeof(recv_buf), 0);
			if(0>=recvnum)  
			{
				perror("error in recving data...\n");
				break;
			}
			recv_buf[recvnum]='\0';
			if (0==strcmp(recv_buf,"quit"))   
			{
				perror("quitting remote controling\n");
				close(connected);
				return 0;
			}
			strcpy(cmd,"/bin/");
			strcat(cmd,recv_buf);
			execute(cmd,send_buf);
			if ('\0'==*send_buf)
			{
				memset(cmd,0,sizeof(cmd));
				strcpy(cmd,"/sbin/");
				strcat(cmd,recv_buf);
				execute(cmd,send_buf);		
				if ('\0'==*send_buf)
				{	
					memset(cmd,0,sizeof(cmd));
					strcpy(cmd,"/usr/bin/");
					strcat(cmd,recv_buf);
					execute(cmd,send_buf);
				}
				if ('\0'==*send_buf)
				{	
					memset(cmd,0,sizeof(cmd));
					strcpy(cmd,"/usr/sbin/");
					strcat(cmd,recv_buf);
					execute(cmd,send_buf);
				}
			}
			if ('\0'==*send_buf)
				sprintf(send_buf,"command is not vaild,check it please\n");
			printf("%s\n",send_buf);
			sendnum = send(connected,send_buf,sizeof(send_buf),0);
			if (0>=sendnum)
			{
				perror("sending data error\n");
				continue;
			}
		}
		close(connected);
	}
	close(sockfd);
	exit(1);
}
int execute(char* command,char* buf)
{
	FILE * 	fp;
	int count;
	if (NULL==(fp = popen(command,"r")))
	{
		perror("creating pipe error\n");
		exit(1);
	}
	count = 0 ;
	while(((buf[count] = fgetc(fp))!=EOF)&&count<2047)
		count++;
	buf[count]='\0';
	pclose(fp);
	if(0==count)  
		printf(buf, "%s cannot execute\n",command);
	return count;
}



