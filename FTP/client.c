/*
	Ag Protokollerine Giris
	
	Betul GULCICEK

	101044025

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

#define BUFFER 512

// function prototypes
int create_client_socket (int port, char* ipaddr);

struct sockaddr_in sock_serv;

int main (int argc, char**argv)
{
	struct timeval start, stop, delta;
    int sfd,fd;
    char buf[BUFFER];
    off_t count=0, m,sz;
	long int n;
    int l=sizeof(struct sockaddr_in);
	struct stat buffer;
    
	if (argc != 4){
		printf("Error usage : %s <ip_serv> <port_serv> <filename>\n",argv[0]);
		return EXIT_FAILURE;
	}
    
    sfd=create_client_socket(atoi(argv[2]), argv[1]);
    
	if ((fd = open(argv[3],O_RDONLY))==-1){
		perror("open fail");
		return EXIT_FAILURE;
	}
    
    	


	if (stat(argv[3],&buffer)==-1){
		perror("stat fail");
		return EXIT_FAILURE;
	}
	else
		sz=buffer.st_size;
    
	//preparation de l'envoie
	bzero(&buf,BUFFER);
    
    if(connect(sfd,(struct sockaddr*)&sock_serv,l)==-1){
        perror("connection error\n");
        exit (3);
    }
	gettimeofday(&start,NULL);
    n=read(fd,buf,BUFFER);
	while(n){
		if(n==-1){
			perror("read fails");
			return EXIT_FAILURE;
		}
		m=sendto(sfd,buf,n,0,(struct sockaddr*)&sock_serv,l);
		if(m==-1){
			perror("send error");
			return EXIT_FAILURE;
		}
		count+=m;

		bzero(buf,BUFFER);
        n=read(fd,buf,BUFFER);
	}
	
	m=sendto(sfd,buf,0,0,(struct sockaddr*)&sock_serv,l);
	gettimeofday(&stop,NULL);
    
	printf("Aktarilan bayt sayisi : %lld\n",(long long int)count);
	printf("Toplam boyut : %lld \n",(long long int)sz);
    
    close(sfd);
	return EXIT_SUCCESS;
}



int create_client_socket (int port, char* ipaddr){
    int l;
	int sfd;
    
	sfd = socket(PF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("socket fail");
        return EXIT_FAILURE;
	}
    
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
    if (inet_pton(AF_INET,ipaddr,&sock_serv.sin_addr)==0){
		printf("Invalid IP adress\n");
		return EXIT_FAILURE;
	}
    
    return sfd;
}
