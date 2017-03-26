/*
The infunctions.c file holds all built-in functions that are required by the shell.
*/

/*Import libraries used.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "infunctions.h"

/*Moves the user backwards and forwards through the file system depending on input. */
int shell_cd(char **arguments){
  if(arguments[2] != NULL){
    fprintf(stderr, "ERROR: Invalid arguments. cd only permits 1 argument.\n");
    return 1;
  }

  if(arguments[1] == NULL){
    chdir(getenv("HOME"));
  }else{
    if (chdir(arguments[1]) != 0){
        fprintf(stderr, "ERROR: No directory of name %s\n", arguments[1]);
    }
  }
  return 1;
}

/*Retrieves the current PATH variable of the system.*/
int shell_getpath(char **arguments){
  if(arguments[1] == NULL){
    const char * current_path = getenv("PATH");
    printf("%s\n",current_path);
  }else{
    fprintf(stderr, "ERROR: No arguments expected.\n");
  }
  return 1;
}

/*Sets the PATH variable of the system to the user input.*/
int shell_setpath(char **arguments){
  const char * alt_path = arguments[1];

  if(arguments[2] != NULL){
    fprintf(stderr, "ERROR: Invalid arguments. Setpath only permits 1 argument.\n");
    return 1;
  }

  if((alt_path == NULL) || (strcmp(alt_path,"/") == 0) || (strcmp(alt_path,"/ we") == 0) ){
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
