#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int cc,i,id;///added

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) 
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
       
    srand(time(NULL));

    for(i=0; i<20 ; i++)// this indicates the number of requests
    {
        bzero(buffer,256);
        cc=rand()%10;   
        if(cc>0)// 90% 
        {
            strcpy(buffer,"buy");
            n = write(sockfd,buffer,strlen(buffer));//sends message
            n = read(sockfd,&id,sizeof(id));//waits on sever message
            if(id!=0)
                printf("Client recieved ticket id:%d\n\n",id);
        }
        else//10%
        {
            strcpy(buffer,"cancel");
            n = write(sockfd,buffer,strlen(buffer));
            read(sockfd,&id,sizeof(id));
            printf("Cancel succesful\n\n");
        }
    }
    bzero(buffer,256);
    strcpy(buffer,"done");
    n=write(sockfd,buffer,strlen(buffer));
    

    close(sockfd);
    return 0;
}
