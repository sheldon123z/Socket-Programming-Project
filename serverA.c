/*serverA*/

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
char string[256];
void error(char *msg)
{
    perror(msg);
    exit(1);
}
int search(int buffer)
{
	FILE *stream;
    	char *line;
	size_t len = 0;
	int result;


	stream = fopen("database_a.csv", "r");
	if (stream == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
           }
           
	while ((getline(&line, &len, stream)) != -1) {
		
		strcpy(string,line);
		
		const char comma[]=",";
		char *token=strtok(line,comma);
		
		if (buffer==atoi(token))
		{
		printf("found in line: %s\n",line);
		result =1;
		//printf("the information is: %s", string);
		break;}			
		else
		result =0;		
           }
        free(line);
        fclose(stream);
        return result;
}

int main(int argc, char *argv[])
{
while(1){
	int sockfd;
	socklen_t addr_len;

	int buffer;
	struct sockaddr_in servaddr,AWS_addr;
	
	memset(&servaddr,0,sizeof(servaddr));
	memset(&AWS_addr,0,sizeof(servaddr));

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  //create a udp socket 
        error("ERROR opening socket");
//serverA information
	servaddr.sin_family = AF_INET;
  	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port=htons(A_portno);
//AWS information
	AWS_addr.sin_family = AF_INET;
  	AWS_addr.sin_addr.s_addr = INADDR_ANY; 
	AWS_addr.sin_port=htons(AWS_portno);
	

	if (bind(sockfd, (struct sockaddr *) &servaddr, //bind udp_a server
              sizeof(servaddr)) < 0) 
              error("ERROR on binding"); 
     	else 
		{printf("the serverA is up and running using UDP over port %d\n",A_portno);}
//receive data
	int n;
	addr_len=sizeof(AWS_addr);
	if((n=recvfrom(sockfd,&buffer,sizeof(buffer),0,
			(struct sockaddr *) &AWS_addr,&addr_len))==-1)
		{error("ERROR on receiving");
		exit(1);}
	
	printf("The server A received input <%d>\n",buffer);
	
	int result=search(buffer);

	printf("the server A has found <%d> match \n",result);
	
	if (sendto(sockfd,&result,sizeof(result),0,(struct sockaddr *)&AWS_addr, sizeof(AWS_addr))<0)
		error("ERROR writing to socket");
	if (result==1)
	{
		if (sendto(sockfd,string,sizeof(string),0,(struct sockaddr *)&AWS_addr, sizeof(AWS_addr))<0)
		error("ERROR writing to socket");		
	}
	
	printf("the server A finished sending the output to AWS\n");
	close(sockfd);
	}
	return 0;
}


