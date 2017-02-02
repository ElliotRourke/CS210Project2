#include <stdio.h>
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

int shell_ls(char **arguments)
{
  printf("THIS CURRENTLY DOES NOTHING :D \n");
}

int shell_clear()
{
  const char* CLEAR_SCREEN = "\e[1;1H\e[2J";
  write(STDOUT_FILENO,CLEAR_SCREEN,12);
  return 1;
}
