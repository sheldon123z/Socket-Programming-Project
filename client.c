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
#define _GNU_SOURCE
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

int buffer[256];
double result[256];

int main(int argc, char *argv[])
{
	int sockfd, n;	
	int LINK_ID, SIZE, POWER;	
					//declare variables
	char *name="localhost";
	struct sockaddr_in serv_addr;   //define structs
	struct hostent *server;		//define server struct
  	
	if (argc < 4 || (atoi(argv[1]))<0 ||atoi((argv[2]))<0) 
	{
       		fprintf(stderr,"usage %s <LINK> <SIZE> <POWER> \n All agrument should be reasonable! \n", argv[0]);
       		
        	exit(0);
   	}
	
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
	bcopy((char *)server->h_addr, 
	 (char *)&serv_addr.sin_addr.s_addr,
	 server->h_length); //ip address
	serv_addr.sin_port = htons(client_portno);//port

	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
	error("ERROR connecting");
	else
	printf("The client is up and running\n");

	for (int i=0;i<3;i++)
		{
			buffer[i]=atoi(argv[i+1]);
		}	
//send the data packet
        if ((n=send(sockfd,buffer,sizeof buffer,0)) < 0) 
	error("ERROR writing to socket");
	
	printf("the client sent ID= <%d> SIZE= <%d> POWER= <%d> to the AWS\n",buffer[0],buffer[1],buffer[2]);
	
//receive from AWS	
	if ((n=recv(sockfd,result,sizeof(result),0)) < 0) 
		error("ERROR reading socket");
    	if (result[2]==0)
    	printf("Found no match for link:%d\n",buffer[0]);
    	else
   	printf("the result for link<%d> is Delay=<%.2f>ms \n",buffer[0],result[2]);
 	

	close(sockfd);

    return 0;
}





