#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

void generate_tickets(int k[10], int l[10])//added
{
    srand(time(NULL));
    int i;
    for(i=0;i<10;i++)
    {
            k[i]= 10000 + rand()%90000; //this generates a random number from 10000-99999
            l[i]=0;                     
    }


}
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    //allT is the database holding all ticket id numbers.
    //soldT has all zeros 
    int soldT[10],allT[10],id_out,count=0,i; //added

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    generate_tickets(allT,soldT);//added
    /*
    for(i=0;i<10;i++)
    {
        
                printf("\nTicket#%d: %d sold status:%d\n",i,allT[i],soldT[i]);
                
       // printf("\n");
    
    }
    */

    if (argc < 2) 
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
    if (sockfd < 0) 
        error("ERROR opening socket");
     
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
     
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
     
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
    
    clilen = sizeof(cli_addr);
    
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
    if (newsockfd < 0) 
        error("ERROR on accept");
    
    
   
   do { 
        write(newsockfd,buffer,255);
        bzero(buffer,256);
        n=read(newsockfd,buffer,255);//waits on client message

        if(strstr(buffer,"buy")!=NULL)
        {       
            if(count<10)
            {         
                for(i=0; i<10; i++ )
                {
                    if(soldT[i]==0)
                    {
                        id_out=allT[i];
                        soldT[i]=id_out;
                        printf("Sold ticket id: %d\n",allT[i]); 
                        write(newsockfd,&id_out,sizeof(id_out));
                        count++;
                        i=10;
                    }
                }
            }
           else
            {
                id_out=0;
                write(newsockfd,&id_out,sizeof(id_out));
                printf("sold out!\n");
             
            }
        }
        else if (strstr(buffer,"cancel")!=NULL)
        {
            read(newsockfd,&id_out,sizeof(id_out));
            for(i=0; i<10; i++)
            {
                int temp;
                temp=soldT[i];
                if(temp==id_out)
                {
                    count--;
                    soldT[i]=0;
                    id_out=allT[i];
                    printf("Cancelled ticket:%d\n\n",id_out);
                    i=10;
                }

            }
        }
        else if (strstr(buffer,"new")!=NULL)
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    } while(strstr(buffer,"done")==NULL);//will loop until client sends "done" as buffer

    
    close(newsockfd);
    close(sockfd);
    return 0; 
}
