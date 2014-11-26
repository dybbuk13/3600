#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

extern int errno;

typedef void (*sighandler_t)(int);
static char *my_envp[100];
static char *search_path[10];

static char *my_argv[100];
static char *search_path[10];
static int my_argc=0;
static int n=0;


int checkcommand();
void changedir();
void handlecommand(char *cmd);
int pipeline(int f,int n,int last);

int attach_path(char *cmd)
{
    char ret[100];
    int index;
    int fd;
    bzero(ret, 100);
    for(index=0;search_path[index]!=NULL;index++) 
    {
        strcpy(ret, search_path[index]);
        strncat(ret, cmd, strlen(cmd));
        if((fd = open(ret, O_RDONLY)) > 0) 
        {
            strncpy(cmd, ret, strlen(ret));
            close(fd);
            return 0;
        }
    }
    return 0;
}

void call_execve(char *cmd)
{
    int i;
    printf("cmd is %s\n", cmd);
    if(fork() == 0) 
    {
        i = execve(cmd, my_argv, my_envp);
        printf("errno is %d\n", errno);
        if(i < 0)
        {
            printf("!%s: %s\n", cmd, "command not found");
            exit(1);        
        }
    }
    else 
    {
        
        wait(NULL);
    }
}

void copy_envp(char **envp)
{
    int index = 0;
    for(;envp[index] != NULL; index++) 
    {
        my_envp[index] = (char *)
		malloc(sizeof(char) * (strlen(envp[index]) + 1));
        memcpy(my_envp[index], envp[index], strlen(envp[index]));
    }
}

void fill_argv(char *tmp_argv)
{
    char *foo = tmp_argv;
    int i;
    int index = 0;
    char ret[100];
    bzero(ret, 100);
    while(*foo != '\0') 
    {
            if(index == 10)
            break;

        if(*foo == ' ') 
        {
            my_argc+=2;
            if(my_argv[index] == NULL)
                my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 1);
            else 
            {
                bzero(my_argv[index], strlen(my_argv[index]));
            }
            strncpy(my_argv[index], ret, strlen(ret));
            strncat(my_argv[index], "\0", 1);
            bzero(ret, 100);
            index++;
        } 
        else 
        {
            strncat(ret, foo, 1);
        }
        foo++;
        /*printf("foo is %c\n", *foo);*/
    }
    my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 1);
    strncpy(my_argv[index], ret, strlen(ret));
    strncat(my_argv[index], "\0", 1);
    
    for(my_argc=0;my_argv[my_argc]!='\0';)
        my_argc++;

    
}

void free_argv()
{
    int index;
    for(index=0;my_argv[index]!=NULL;index++)
    {
        bzero(my_argv[index], strlen(my_argv[index])+1);
        my_argv[index] = NULL;
        free(my_argv[index]);
    }
}

void get_path_string(char **tmp_envp, char *bin_path)
{
    int count = 0;
    char *tmp;
    while(1) 
    {
        tmp = strstr(tmp_envp[count], "PATH");
        if(tmp == NULL) 
        {
            count++;
        } 
        else 
        {
            break;
        }
    }
        strncpy(bin_path, tmp, strlen(tmp));
}

void handle_signal(int signo)
{
    printf("\n[MY_SHELL ] ");
    fflush(stdout);
}

void insert_path_str_to_search(char *path_str) 
{
    int index=0;
    char *tmp = path_str;
    char ret[100];

    while(*tmp != '=')
        tmp++;
    tmp++;

    while(*tmp != '\0') 
    {
        if(*tmp == ':') 
        {
            strncat(ret, "/", 1);
            search_path[index] = (char *) malloc(sizeof(char) * (strlen(ret) + 1));
            strncat(search_path[index], ret, strlen(ret));
            strncat(search_path[index], "\0", 1);
            index++;
            bzero(ret, 100);
        } 
        else 
        {
            strncat(ret, tmp, 1);
        }
        tmp++;
    }
}
void handlecommand(char *cmd)
{
    if (checkcommand()==0){
        call_execve(cmd);
  }
}
int checkcommand()
{
    if (strcmp("exit",my_argv[0])==0){
        exit(EXIT_SUCCESS);
    }
    if (strcmp("cd",my_argv[0])==0){
        changedir();
        return 1;
    }
    if (my_argc==3)
    {
        if (strcmp("|",my_argv[1])==0)
        {
            pipeline(2,0,0);

            return 1;
        }
    }   
    if (my_argc>3){

        pipeline(my_argc-1,0,0);
        return 1;
    }
  

  return 0;
}
void changedir()
{
    if(my_argv[1]==NULL)
    {
        chdir(getenv("HOME"));
    }
    else
    {
        if(chdir(my_argv[1])==-1)
         printf(" %s no such directory\n",my_argv[1]);
    }
}
int pipeline(int f,int n,int last)
{
char    line[4096];
  FILE    *fpin, *fpout;
  char arg1[50],arg2[50],arg3[50];
  memset(arg1,'\0',sizeof arg3);
  memset(arg2,'\0',sizeof arg3);
  memset(arg3,'\0',sizeof arg3);
  int i;
  if(f==2)
  {
    strcpy(arg1,my_argv[0]);
    strcpy(arg2,my_argv[2]);

  }
  else{
    for(i=0;i<f-1;i++){
        strcat(arg1,my_argv[i]);
        strcat(arg1," ");
    }
    strcpy(arg2,my_argv[my_argc-1]);
  }
  puts(my_argv[0]);
  puts(arg1);
  puts(arg2);

  if ((fpin=popen(arg1,"r"))==NULL)
    printf("cant open %s",arg1);
    
  if ((fpout=popen(arg2,"w"))==NULL)
    printf("popen error");

  while(fgets(line,4096,fpin)!=NULL){
    if(fputs(line,fpout)==EOF)
      printf("fputs error to pipe\n");
  }
  if(ferror(fpin))
    printf("fgets error");
  if(pclose(fpout)== -1)
    printf("pclose error\n");
  wait(NULL);
}
