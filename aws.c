
/*AWS server*/
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
//remember to change to 448 

void *get_in_addr(struct sockaddr *);
void sigchld_handler(int);
void error(char *);


int main(int argc, char *argv[])
{	
     int listen_sockfd, mlisten_sockfd,client_sockfd,monitor_sockfd;//tcp sockets
     int AWS_udp_sockfd;//udp sockets
     int clilen,monlen, pid;
     int ReceiveBuffer[256]={};
     struct sockaddr_in serv2cli_addr,serv2mon_addr,cli_addr,mon_addr;//tcp addresses
     struct sockaddr_in AWS_udp_addr,server_A,server_B,server_C;//udp addresses

     struct sigaction sa;
     
     char addr_string[INET6_ADDRSTRLEN];



   
     if ((listen_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)    //create a tcp socket
        error("ERROR opening socket");
     if ((mlisten_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)//create a tcp monitor socket
        error("ERROR opening socket");
     if ((AWS_udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  //create a udp socket 
        error("ERROR opening socket AWS_udp_sockfd");
     
     
     bzero((char *) &serv2cli_addr, sizeof(serv2cli_addr));//clear server address
     bzero((char *) &serv2mon_addr, sizeof(serv2mon_addr));//clear monitor address
     bzero((char *) &AWS_udp_addr, sizeof(AWS_udp_addr));//clean udp addresses
    
     bzero((char *) &server_A, sizeof(server_A));//clean udp addresses
     bzero((char *) &server_B, sizeof(server_B));//clean udp addresses
     bzero((char *) &server_C, sizeof(server_C));//clean udp addresses
     
    
     serv2cli_addr.sin_family = AF_INET;
     serv2cli_addr.sin_addr.s_addr = INADDR_ANY;
     serv2cli_addr.sin_port = htons(client_portno);//assign port number to serv2cli_addr
	
	
     serv2mon_addr.sin_family = AF_INET;
     serv2mon_addr.sin_addr.s_addr = INADDR_ANY;   
     serv2mon_addr.sin_port = htons(monitor_portno);//assign port number to serv2mon_addr
 

     AWS_udp_addr.sin_family = AF_INET;
     AWS_udp_addr.sin_addr.s_addr = INADDR_ANY; 
     AWS_udp_addr.sin_port=htons(AWS_portno);//AWS port 
//setup serverA
     server_A.sin_family = AF_INET;
     server_A.sin_addr.s_addr = INADDR_ANY; 
     server_A.sin_port=htons(A_portno);
//setup serverB
     server_B.sin_family = AF_INET;
     server_B.sin_addr.s_addr = INADDR_ANY; 
     server_B.sin_port=htons(B_portno);//AWS port 
//setup serverC  
     server_C.sin_family = AF_INET;
     server_C.sin_addr.s_addr = INADDR_ANY; 
     server_C.sin_port=htons(C_portno);


     if (bind(listen_sockfd, (struct sockaddr *) &serv2cli_addr, //bind server listen socket
              sizeof(serv2cli_addr)) < 0) 
              error("ERROR on binding");

     
     if (bind(mlisten_sockfd, (struct sockaddr *) &serv2mon_addr, 
              sizeof(serv2mon_addr)) < 0) 
              error("ERROR on binding");
 
     if (bind(AWS_udp_sockfd, (struct sockaddr *) &AWS_udp_addr, //bind udp_a server
              sizeof(AWS_udp_addr)) < 0) 
              error("ERROR on binding"); 

     listen(mlisten_sockfd,10);
     listen(listen_sockfd,10);//listen_socket listen to connections
    
     clilen = sizeof(cli_addr);
     monlen = sizeof(mon_addr);
     printf("The AWS is up and running\n");
     
while(1){
	monitor_sockfd = accept(mlisten_sockfd, 
		(struct sockaddr *) &mon_addr, &monlen); //accept client connection
	if (monitor_sockfd < 0) 
		error("ERROR on accept");
	
	inet_ntop(mon_addr.sin_family,get_in_addr((struct sockaddr *)&mon_addr),
		addr_string,INET6_ADDRSTRLEN);
	//printf("server has got monitor connection from IP: %s\n",addr_string);		
	
	client_sockfd = accept(listen_sockfd, 
		(struct sockaddr *) &cli_addr, &clilen); //accept client connection
	if (client_sockfd < 0) 
		error("ERROR on accept");
	inet_ntop(cli_addr.sin_family,get_in_addr((struct sockaddr *)&cli_addr),
		addr_string,INET6_ADDRSTRLEN); //internet to presentation
       // printf("server has got client connection from IP: %s\n\n",addr_string);
        pid = fork();

     if (pid < 0)
         error("ERROR on fork");
     if (pid == 0)  //created a new process to receive client's data
		{   	 
		close(listen_sockfd);
		close(mlisten_sockfd);
	int result_A,result_B;
	double result_C[5];
	char string_A[256],string_B[256],blankstring[256],blankstring2[256];
	socklen_t addr_len_A=sizeof(server_A), addr_len_B=sizeof(server_B),addr_len_C=sizeof(server_C);

//receive from client
		int n;
		if((n=recv(client_sockfd,ReceiveBuffer,sizeof(ReceiveBuffer),0))==-1)
		{error("ERROR on receiving");
		exit(1);}

				
		
		
//receive from server A and B

	
//send to backened serverA
		if (sendto(AWS_udp_sockfd,&ReceiveBuffer[0],sizeof(ReceiveBuffer[0]),
		0,(struct sockaddr *)&server_A, sizeof(server_A))<0)
		error("ERROR writing to socket");
		
//receive from A
		if((n=recvfrom(AWS_udp_sockfd,&result_A,sizeof(result_A),0,
			(struct sockaddr *) &server_A,&addr_len_A))==-1)
		{error("ERROR on receiving"); 
		exit(1);}
	
		if(result_A==1)
		{
			if((n=recvfrom(AWS_udp_sockfd,string_A,sizeof(string_A),0,
				(struct sockaddr *) &server_A,&addr_len_A))==-1)
			{error("ERROR on receiving"); 
			exit(1);}
		}
	
//send to backened serverB
		if (sendto(AWS_udp_sockfd,&ReceiveBuffer[0],sizeof(ReceiveBuffer[0]),
			0,(struct sockaddr *)&server_B, sizeof(server_B))<0)
			error("ERROR writing to socket");
		
			
//receive from B
		if((n=recvfrom(AWS_udp_sockfd,&result_B,sizeof(result_B),0,
				(struct sockaddr *) &server_B,&addr_len_B))==-1)
			{error("ERROR on receiving"); 
			exit(1);}

		if(result_B==1)
		{
			if((n=recvfrom(AWS_udp_sockfd,string_B,sizeof(string_B),0,
				(struct sockaddr *) &server_A,&addr_len_A))==-1)
			{error("ERROR on receiving"); 
			exit(1);}		
		}

	
//printf the information 
printf("The AWS received LINK_ID = <%d> SIZE= <%d> and power= <%d>from client using TCP over port <%d>\n ",ReceiveBuffer[0],ReceiveBuffer[1],ReceiveBuffer[2],client_portno);

printf("The AWS sent LINK_ID = <%d> SIZE= <%d> and power= <%d> from client using TCP over port <%d>\n ",ReceiveBuffer[0],ReceiveBuffer[1],ReceiveBuffer[2],monitor_portno);

printf("The AWS send LINK_ID = <%d>,SIZE= <%d> and power= <%d> to server A using UDP over port <%d>\n",ReceiveBuffer[0],ReceiveBuffer[1],ReceiveBuffer[2],AWS_portno);
	
printf("The AWS send LINK_ID = <%d> SIZE= <%d> and power= <%d> to server B using UDP over port <%d>\n",ReceiveBuffer[0],ReceiveBuffer[1],ReceiveBuffer[2],AWS_portno);

printf("The AWS received <%d> match from backened server A using UDP over port<%d>\n",result_A,A_portno);		

printf("The AWS received <%d> match from backened server B using UDP over port<%d>\n",result_B,B_portno);	
	if (sendto(AWS_udp_sockfd,ReceiveBuffer,sizeof(ReceiveBuffer),
			0,(struct sockaddr *)&server_C, sizeof(server_C))<0)
			error("ERROR writing to socket");

//send <input> to server C
	if ((result_A==1) || (result_B==1))
	{


	//resultA==1
	if (result_A==1)
		{
		if (sendto(AWS_udp_sockfd,string_A,sizeof(string_A),
			0,(struct sockaddr *)&server_C, sizeof(server_C))<0)
			error("ERROR writing to socket");
		
	
		if((n=recvfrom(AWS_udp_sockfd,result_C,sizeof(result_C),0,
				(struct sockaddr *) &server_C,&addr_len_C))==-1)
			{error("ERROR on receiving"); 
			exit(1);}
		}
	//resultB==1	
	if (result_B==1)
		{
		if (sendto(AWS_udp_sockfd,string_B,sizeof(string_B),
			0,(struct sockaddr *)&server_C, sizeof(server_C))<0)
			error("ERROR writing to socket");
		
	
		if((n=recvfrom(AWS_udp_sockfd,result_C,sizeof(result_C),0,
				(struct sockaddr *) &server_C,&addr_len_C))==-1)
			{error("ERROR on receiving"); 
			exit(1);}
		}
	printf("The AWS send LINK_ID = <%d>, SIZE=<%d>, POWER=<%d> and link information to Backened-Server C using UDP over port<%d> \n",ReceiveBuffer[0],ReceiveBuffer[1],ReceiveBuffer[2],C_portno);	
	
	//send the result to client and monitor
	
	if ((send(client_sockfd,result_C,sizeof(result_C),0)) < 0) 
			error("ERROR send to client socket");	
	printf("The AWS sent delay=<%.2f>ms to the client using TCP over port number <%d> \n",result_C[2],client_portno);


	//send to monitor
	for (int i=3;i<6;i++)		
	result_C[i]=(double)ReceiveBuffer[i-3];
	if ((send(monitor_sockfd,result_C,sizeof(result_C),0)) < 0) 
			error("ERROR send to monitor socket");
			
	printf("The AWS sent detailed results to the monitor using TCP over port number <%d> \n",monitor_portno);	
	}
//no matches case	
	if ((result_A!=1) && (result_B!=1))
	{
	if (sendto(AWS_udp_sockfd,blankstring,sizeof(blankstring),
			0,(struct sockaddr *)&server_C, sizeof(server_C))<0)
			error("ERROR writing to socket");
	if((n=recvfrom(AWS_udp_sockfd,blankstring2,sizeof(blankstring2),0,
				(struct sockaddr *) &server_C,&addr_len_C))==-1)
			{error("ERROR on receiving"); 
			exit(1);}
	printf("found no match for LINK_ID: <%d>\n",ReceiveBuffer[0]);
	}
	
	
	
//close child process			
	exit(0);
	     }
	close(client_sockfd);
	close(monitor_sockfd);
}	

     return 0; /* we never get here */
}

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET)
	{
	return &(((struct sockaddr_in*)sa)->sin_addr);
			}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void sigchld_handler(int s)
{
	int saved_errno = errno;
	while(waitpid(-1,NULL,WNOHANG)>0);
	errno = saved_errno;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

