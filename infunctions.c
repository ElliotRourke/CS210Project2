#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "infunctions.h"

/* Define functions*/
int shell_cd(char **arguments)
{
  if(arguments[1] == NULL)
  {
    chdir(getenv("HOME"));
  }else{
    if (chdir(arguments[1]) != 0)
    {
        fprintf(stderr, "ERROR: Expected argument : cd <Directory>\n");
    }
  }
  return 1;
}

int shell_getpath()
{
  const char * current_path = getenv("PATH");
  printf("%s\n",current_path);
  return 1;
}

int shell_setpath(char **arguments)
{
  const char * alt_path = arguments[1];

  if(arguments[1] == NULL)
  {
    fprintf(stderr, "Missing required arguments : setpath <PATH>\n");
    return 1;
  }else{
    setenv("PATH",alt_path,1);
    printf("NEW PATH SET : \n");
    shell_getpath();
  }
  return 1;
}

int shell_history()
{

  return 1;
}

int shell_help()
{
  printf("|| General Help List || \n \n");
  printf("cd : Changed directory , cd <directory> , can chain changes i.e cd <directory/directory/directory, can return to previous directory cd .. as argument.\n");
  printf("ls : Lists the elements in the current directory. \n");
  printf("pwd : Prints the current working directory.\n");
  printf("getpath : Prints the current value of PATH. \n");
  printf("setpath : Allows the user to change the value of PATH by entering 'setpath <PATH STRING>'.\n");
  printf("\n");
  return 1;
}
