#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "infunctions.h"

int shell_cd(char **arguments){
  if(arguments[1] == NULL){
    chdir(getenv("HOME"));
  }else{
    if (chdir(arguments[1]) != 0){
        fprintf(stderr, "ERROR: No directory of name %s\n", arguments[1]);
    }
  }
  return 1;
}

int shell_getpath(char **arguments){
  if(arguments[1] == NULL){
    const char * current_path = getenv("PATH");
    printf("%s\n",current_path);
  }else{
    fprintf(stderr, "ERROR: No arguments expected.\n");
  }
  return 1;
}

int shell_setpath(char **arguments){
  const char * alt_path = arguments[1];

  if(alt_path == NULL || strcmp(alt_path,"/") == 0 || strcmp(alt_path,"/ we") == 0 ){
    fprintf(stderr, "Missing required arguments : setpath <PATH>\n");
    return 1;
  }else{
    setenv("PATH",alt_path,1);
    printf("NEW PATH SET : \n");
    arguments[1] = '\0';
    shell_getpath(arguments);
  }
  return 1;
}

int char_to_int(char * temp){
  int x = 0;
  int y = 0;
  int z = 0;
  int i;
  char c[2];

  if(temp == NULL){
    return z;
  }
  strcpy(c,temp);

  if((isdigit(c[0])) && (c[1] == '\0')){
      x = c[0] - '0';
      return x;
    }else if((isdigit(c[0])) && (isdigit(c[1]))){
      x = c[0] - '0';
      x  = x * 10;
      y = c[1] - '0';
      z = x + y;
      return z;
    }else{
      return z;
    }
}

int shell_help(char **arguments){
  if(arguments[1] == NULL){
    printf("|| General Help List || \n \n");
    printf("cd : Changed directory , cd <directory> , can chain changes i.e cd <directory/directory/directory, can return to previous directory cd .. as argument.\n");
    printf("ls : Lists the elements in the current directory. \n");
    printf("pwd : Prints the current working directory.\n");
    printf("getpath : Prints the current value of PATH. \n");
    printf("setpath : Allows the user to change the value of PATH by entering 'setpath <PATH STRING>'.\n");
    printf("\n");
  }else{
    fprintf(stderr, "Error: No arguments expected\n");
  }

  return 1;
}
