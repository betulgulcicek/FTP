/*
	Ag Protokollerine Giris
	
	Betul GULCICEK

	101044025

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>


#define BUFFER 512
#define BACKLOG 1

const int nthreads = 4;

// function prototypes
int create_server_socket (int port);

struct sockaddr_in sock_serv,sock_clt;


int main(int argc,char** argv){
    int sfd,fd;
    unsigned int length=sizeof(struct sockaddr_in);
    long int n, m,count=0;
    unsigned int nsid;
    ushort clt_port;
    char buffer[BUFFER],filename[256];
    char dst[INET_ADDRSTRLEN];
    
	   
	time_t intps;
	struct tm* tmi;
	    
    if(argc!=2) {
        error("USAGE:\t ./%s <num_port>\n", argv[0]);
        exit(3);
    }
    
    sfd = create_server_socket(atoi(argv[1]));
    
    bzero(buffer,BUFFER);
    listen(sfd,BACKLOG);
    
    nsid=accept(sfd,(struct sockaddr*)&sock_clt,&length);
    if(nsid==-1){
        perror("accept fail");
        return EXIT_FAILURE;
    }
    else {
        if(inet_ntop(AF_INET,&sock_clt.sin_addr,dst,INET_ADDRSTRLEN)==NULL){
            perror("error socket");
            exit (4);
        }
        clt_port=ntohs(sock_clt.sin_port);
        
        bzero(filename,256);
        intps = time(NULL);
        tmi = localtime(&intps);
        bzero(filename,256);

        sprintf(filename,"transmit.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
        printf("Creating the copied output file : %s\n",filename);
        
        if ((fd=open(filename,O_CREAT|O_WRONLY,0600))==-1)
        {
            perror("open fail");
            exit (3);
        }
        bzero(buffer,BUFFER);
        
        n=recv(nsid,buffer,BUFFER,0);


        while(n) {
            if(n==-1){
                perror("recv fail");
                exit(5);
            }
            if((m=write(fd,buffer,n))==-1){
                perror("write fail");
                exit (6);
            }
            count=count+m;
            bzero(buffer,BUFFER);
            n=recv(nsid,buffer,BUFFER,0);
        }
        close(sfd);
        close(fd);
    }
    close(nsid);
    
    printf("Transmission %s.%d\n",dst,clt_port);
    printf("Alinan byte sayisi : %ld \n",count);
    
    return EXIT_SUCCESS;
}

int create_server_socket (int port){
    int l;
	int sfd;
    int yes=1;
    
	sfd = socket(PF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("socket fail");
        return EXIT_SUCCESS;
	}
    

    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,&yes,sizeof(int)) == -1 ) {
        perror("setsockopt error");
        exit(5);
    }
    
  
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
	sock_serv.sin_addr.s_addr=htonl(INADDR_ANY);
    
	
	if(bind(sfd,(struct sockaddr*)&sock_serv,l)==-1){
		perror("bind fail");
		return EXIT_FAILURE;
	}
    
    
    return sfd;
}
