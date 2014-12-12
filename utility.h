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
static int pipecount=0;
int i;
static char *args[512];
pid_t pid;

static int processtable[100];

int checkcommand();
void changedir();
void handlecommand(char *cmd);
int pipeline(int f,int n,int last);
void getpipeargs();
void user_cmd();
int echo_env();
int bg() ;
void jobs() ;

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
int returnposition(int a){
    while(strcmp("|",my_argv[a])!=0 && strcmp("\0",my_argv[a])!=0){
        a++;
    }
    return a;
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
    if (strcmp("echo",my_argv[0])==0 && strstr(my_argv[1],"$")!=NULL){
        return echo_env();
    }
    if (strcmp("user",my_argv[0])==0){
        user_cmd();
        return 1;
    }
    if (my_argc>=3){
        for(i=0;my_argv[i]!='\0';i++)
            if(strcmp("|",my_argv[i])==0){
                getpipeargs();

                for (i=0;my_argv[i]!='\0';i++){
                if(strcmp("|",my_argv[i])==0)
                    pipecount++;
                }
                
                for(i=0;i<pipecount+1;i++)
                {
                    wait(NULL);
                }
                pipecount=0;
                return 1;
            }
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
void getpipeargs(){
    
    int input=0,j=0,a=0;
    for (i=0;my_argv[i]!='\0';i++){
            if(strcmp("|",my_argv[i])==0)
                pipecount++;
    }
    if(pipecount>=2)
    {
        a=returnposition(a);
        for(i=0;i<a;i++)
        {
            if(strcmp("|",my_argv[i])!=0)
            {
                args[i]=my_argv[i];
            }    
        }
        input=pipeline(input,1,0);
        a++;
        a=returnposition(a);
        for (i+=1;i<a;i++)
        {
            if(strcmp("|",my_argv[i])!=0)
            {
                args[j]=my_argv[i];
                j++;
            }
        }
        args[j]=NULL;
        input=pipeline(input,0,0);
        j=0;
        for (i+=1;my_argv[i]!='\0';i++)
        {
            args[j]=my_argv[i];
            j++;
        }
        args[j]=NULL;
        input=pipeline(input,0,1);
    }
    else
    {
        if(pipecount==0)
        {
            for(i=0;my_argv[i]!='\0';i++){
                
                    printf("copied: %s\n",my_argv[i] );
                    args[i]=my_argv[i];   
            }
            input=pipeline(input,1,1);
        }
        else
        {
            a=returnposition(a);
            for(i=0;i<a;i++)
            {
                if(strcmp("|",my_argv[i])!=0)
                {
                    args[i]=my_argv[i];
                }
            }
            input=pipeline(input,1,0);  
            for (i+=1;my_argv[i]!='\0';i++)
            {
           
                args[j]=my_argv[i];
                j++;
            }
            args[j]=NULL;
            input=pipeline(input,0,1);
        }
    }
    pipecount=0;
}
int pipeline(int f,int n,int last)
{
    int pipes[2];
    pipe (pipes);
    pid = fork();
    
    if(pid==0){
        if (n==1 && last ==0 && f==0 ){
            dup2(pipes[1],STDOUT_FILENO);
        }
        else if (n==0 && last ==0 && f!=0){
            dup2(f,STDIN_FILENO);
            dup2(pipes[1],STDOUT_FILENO);
        }
        else{
            dup2(f,STDIN_FILENO);
        }
        if (execvp(args[0],args)==-1)
            exit(EXIT_FAILURE);
    }

    if (f !=0)
        close(f);
    close(pipes[1]);
    if (last==1)
        close(pipes[0]);
    return pipes[0]; 
}

int echo_env()
{       
    char** env;
    char *c=my_argv[1];
    char *a= (char *)malloc(sizeof(char)* strlen(c));

    for(i=0;c[i+1]!=0;i++)
        a[i]=c[i+1];
    strcat(a,"=");
    i=0;
    for(env=my_envp;*env!=0;env++)
    {
        char *thisenv=*env;
        if(strstr(thisenv,a)!=NULL){
        i=strcspn(thisenv,"=");
        i++;
        for(;thisenv[i]!=0;i++)
            printf("%c",thisenv[i] );
        }
    }
    if(i==0){
        return 0;
    }
    else{
    printf("\n");
    return 1;
    }   
}

void user_cmd()
{
    char** env;
    for(env=my_envp;*env!=0;env++)
    {
      char* thisenv=*env;
      if(strstr(thisenv,"USER=")!=NULL){
        i=strcspn(thisenv,"=");
        i++;
        for(;thisenv[i]!=0;i++)
            printf("%c",thisenv[i] );
      }
    }
    printf("\n");
}
int bg () { 
   if (my_argc==0){
      printf ("error. no command.\n") ;
      return 1;
   } 
   pid = fork () ;
   if (pid==0){   
      setpgid(0, 0) ;
      bzero (my_argv[my_argc-1], strlen(my_argv[my_argc-1])+1);
      free(my_argv[my_argc-1]);
      free(my_argv[my_argc]);
      my_argc=my_argc-1;
   }
   else {
      return 1;
   }
}

void jobs() {
   return ;
}

void stdRedirect()
{
  int in = 0, out = 0, Exit = 0, i = 0;

  while(Exit < 1)
  {
    if(strcmp("\0", my_argv[i]) == 0)
      Exit++;

    if(strcmp("<", my_argv[i]) == 0)
    {
      in = open(my_argv[i+1]), O_RDONLY);

      if(strcmp(">", my_argv[i+2]) == 0)
      {
        out = open(my_argv[i+3], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

        dup2(in, 0);
        dup2(out, 1);

        close(in);
        close(out);

        execvp(args[0], args);
        Exit++;
      }

      else
      {
        dup2(in, 0);

        close(in);

        execvp(args[0], args);
        Exit++;
      }

    }
    if(strcmp(">", my_argv[i]) == 0)
    {
      out = open(my_argv[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

      dup2(out, 1);

      close(out);

      execvp(args[0], args);
      Exit++;
    }
    args[i] = myargv[i];
    i++;
  }
}
