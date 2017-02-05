#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "infunctions.h"

/* Define functions*/
int shell_cd(char **arguments)
{
  if(arguments[1] == NULL)
  {
    fprintf(stderr, "ERROR: Expected argument : cd <Directory>\n");
  }else{
    if (chdir(arguments[1]) != 0)
    {
        perror("Shell");
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
