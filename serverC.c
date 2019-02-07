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
#include <math.h>
#define _GNU_SOURCE

#define A_portno 21448
#define B_portno 22448
#define C_portno 23448
#define AWS_portno 24448
#define client_portno 25448
#define monitor_portno 26448

double d_trans,d_prop,d_end;
double cal_result[5];
void error(char *msg)
{
    perror(msg);
    exit(1);
}
double my_log(int base,double x)
{
	return log(x)/log(base);
}
float calculate(char *msg,int size, int power)
{
	double C,B,S,N;
	char pSrc[256];
	strcpy(pSrc,msg);

	char *pToken=NULL;
	char *pSave=NULL;
	char *pDelimiter=",";
	float data[5];
	int i=0;
	pToken=strtok_r(pSrc,pDelimiter,&pSave);
	while(pToken!=NULL)
	{		
		
		//printf("token:%s\n",pToken);
		data[i]=atof(pToken);
		pToken=strtok_r(NULL,pDelimiter,&pSave);
		//printf("data[%d]=:%.2f\n",i,data[i]);
		i++;
		
		
	}
	S=(double)pow(10,((double)power)/10)/1000;
	N=(double)pow(10,((double)data[4])/10)/1000;
	C=data[1]*1000000*(my_log(2,(1+S/N)));
	
	
	d_prop=(data[2]*1000)/(data[3]*pow(10,7))*1000;
	d_trans=(size/C)*1000;
	d_end=d_prop+d_trans;
	
	cal_result[0]=d_prop;
	cal_result[1]=d_trans;
	cal_result[2]=d_end;
	printf("S= %.2f ,N= %.2f ,C= %.2f\n  d_prop= %.2f\n  d_trans= %.2f\n  d_end=%.2f\n\n",S,N,C,d_prop,d_trans,d_end);
	return 0;
}
int main(int argc, char *argv[])
{
while(1){
	int sockfd;
	socklen_t addr_len;

	int buffer[256];
	char string[256];
	struct sockaddr_in servaddr,AWS_addr;
	
	memset(&servaddr,0,sizeof(servaddr));
	memset(&AWS_addr,0,sizeof(servaddr));

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  //create a udp socket 
        error("ERROR opening socket");
//serverA information
	servaddr.sin_family = AF_INET;
  	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port=htons(C_portno);
//AWS information
	AWS_addr.sin_family = AF_INET;
  	AWS_addr.sin_addr.s_addr = INADDR_ANY; 
	AWS_addr.sin_port=htons(AWS_portno);
	

	if (bind(sockfd, (struct sockaddr *) &servaddr, //bind udp_a server
              sizeof(servaddr)) < 0) 
              error("ERROR on binding"); 
     	else 
		{printf("the serverC is up and running using UDP over port %d\n",C_portno);}

//receive data
	int n;char blankstring[256]="no match";
	addr_len=sizeof(AWS_addr);
	if((n=recvfrom(sockfd,buffer,sizeof(buffer),0,
			(struct sockaddr *) &AWS_addr,&addr_len))==-1)
		{error("ERROR on receiving");
		exit(1);}
	
	printf("The server C received LINK_ID=<%d> SIZE=<%d> POWER=<%d>\n",buffer[0],buffer[1],buffer[2]);
	
	if((n=recvfrom(sockfd,string,sizeof(string),0,
			(struct sockaddr *) &AWS_addr,&addr_len))==-1)
		{error("ERROR on receiving");
		exit(1);}
	if(string=="no match")
	{
		if (sendto(sockfd,blankstring,sizeof(blankstring),0,(struct sockaddr *)&AWS_addr, sizeof(AWS_addr))<0)
			error("ERROR writing to socket");
	else
		printf("the server C finished sending the output to AWS\n");
	close(sockfd);
	}
	else
	{
	calculate(string,buffer[1],buffer[2]);
	
	printf("the server C finished the calculation for link %d\n",buffer[0]);
	
	if (sendto(sockfd,cal_result,sizeof(cal_result),0,(struct sockaddr *)&AWS_addr, sizeof(AWS_addr))<0)
			error("ERROR writing to socket");
	else
		printf("the server C finished sending the output to AWS\n");
	
	close(sockfd);
	}
	
}
return 0;
}


