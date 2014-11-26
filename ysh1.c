#include "utility.h"



int main(int argc, char *argv[], char *envp[])
{
    char c;
    int i, fd;
    char *tmp = (char *)malloc(sizeof(char) * 100);
    char *path_str = (char *)malloc(sizeof(char) * 256);
    char *cmd = (char *)malloc(sizeof(char) * 100);
    
    signal(SIGINT, SIG_IGN);
    signal(SIGINT, handle_signal);


    char** env;
    for(env=envp;*env!=0;env++)
    {
      char* thisenv=*env;
      printf("%s\n",thisenv);
    }

    copy_envp(envp);//func in utility

    get_path_string(my_envp, path_str);//fiu   
    insert_path_str_to_search(path_str);//fiu

    if(fork() == 0) 
    {
        execve("/usr/bin/clear", argv, my_envp);
        exit(1);
    } 
    else 
    {
        wait(NULL);
    }
    printf("[MY_SHELL ] ");
    fflush(stdout);

    while(c != EOF) 
    {
        c = getchar();
        switch(c) 
        {
            case '\n': 
            if(tmp[0] == '\0') 
            {
              printf("[MY_SHELL ] ");
            } 
            else 
            {
              fill_argv(tmp);//fiu
              printf("argc:%d\n",my_argc);
              strncpy(cmd, my_argv[0], strlen(my_argv[0]));
              strncat(cmd, "\0", 1);
              if(index(cmd, '/') == NULL) //index()=strchr()
              {
                if(attach_path(cmd) == 0) 
                {
                  handlecommand(cmd);
                  //call_execve(cmd);//fiu
                  
                } 
                else 
                {
                  printf("%s: command not found\n", cmd);
                }
              } 
              else 
              {
                if((fd = open(cmd, O_RDONLY)) > 0) 
                {
                  close(fd);
                  //handlecommand(cmd);
                  call_execve(cmd);//fiu
                } 
                else 
                {
                  printf("%s: command not found\n", cmd);
                }
              }
              free_argv();//fiu
              printf("[!MY_SHELL ] ");
              bzero(cmd, 100);
            }
          bzero(tmp, 100);
          break;
          default: strncat(tmp, &c, 1);
          break;
        }
    }
    free(tmp);
    free(path_str);
    for(i=0;my_envp[i]!=NULL;i++)
        free(my_envp[i]);
    for(i=0;i<10;i++)
        free(search_path[i]);
    printf("\n");
    return 0;
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
      pipeline(2);

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
  else{
    if(chdir(my_argv[1])==-1)
      printf(" %s no such directory\n",my_argv[1]);
  }
}
void pipeline(int f)
{
  
  char    line[4096];
  FILE    *fpin, *fpout;
  char arg1[50],arg2[50];
  int i;
  if(f==2)
  {
    strcpy(arg1,my_argv[0]);
    strcpy(arg2,my_argv[2]);

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
  
}
///declare static int my_argc=0;
///add for(my_argc=0;my_argv[my_argc]!='\0';)my_argc++; at the end of fill_arg
