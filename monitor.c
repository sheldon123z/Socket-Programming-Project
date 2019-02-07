#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define A_portno 21448
#define B_portno 22448
#define C_portno 23448
#define AWS_portno 24448
#define client_portno 25448
#define monitor_portno 26448

void error(char *msg)
{
    perror(msg);
    exit(0);
}




int main(int argc, char *argv[])
{
	while(1)
	{
	double buffer[256]={};
	printf("the monitor is up and running...\n");
	int sockfd, n;	
	int LINK_ID, SIZE, POWER;	
					//declare variables
	char *name="localhost";
	struct sockaddr_in serv_addr;   //define structs
	struct hostent *server;		//define server struct
  	
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	error("ERROR opening socket");
	server = gethostbyname(name);

	if (server == NULL) {
	fprintf(stderr,"ERROR, no such host\n");
	exit(0);
	}
   
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;//ip type
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length); //ip address
	serv_addr.sin_port = htons(monitor_portno);//port

	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
	error("ERROR connecting");

	if ((n=recv(sockfd,buffer,sizeof(buffer),0)) < 0) 
	error("ERROR reading socket");
	printf("the monitor received LINK= <%.0f> SIZE=<%.2f>, POWER=<%.2f> \n",buffer[3],buffer[4],buffer[5]);
	printf("the result for link<%.0f>:\nTt = <%.2f>ms \nTp=<%.2f>ms \nDelay=<%.2f>ms \n",buffer[3],buffer[1],buffer[0],buffer[2]);
	if(buffer[0]==0 && buffer[1]==0 && buffer[2]==0)
	{
	printf("Found no match for link <%.0f>",buffer[3]);
        }
   
	close(sockfd);
}
    return 0;
}





