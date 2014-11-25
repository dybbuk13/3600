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
                  call_execve(cmd);//fiu
                } 
                else 
                {
                  printf("%s: command not found\n", cmd);
                }
              }
              free_argv();//fiu
              printf("[MY_SHELL ] ");
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
  if (strcmp("cd",my_argv[0])==0){
    changedir();
    return -1;
  //if ()

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
