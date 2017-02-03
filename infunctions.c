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
