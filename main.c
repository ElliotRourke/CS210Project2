#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "infunctions.h"

/* Define constants */
#define STRINGSIZE 50

/* Define functions */
void command_loop();
char *read_input();
char **parse_input(char *input);
int execute_input(char **arguments);
int create_process(char **arguments);

//This is a simple OS shell designed by our team!
int main(int argc, char const *argv[])
{
  const char *old_path = getenv("PATH");
  chdir(getenv("HOME"));
  command_loop();
  printf("Restoring Path...\n");
  setenv("PATH",old_path,1);
  shell_getpath();
  return 0;
}

//Handles commands input by user
void command_loop()
{
  int status;
  char *input;
  char **arguments;
  char buf[1024];

  do{
    //Prompt
    printf("%s",  getcwd(buf,sizeof(buf)));
    printf("$ ");
    //Read
    input = read_input();
    //Parse
    arguments = parse_input(input);
    //Execute
    status = execute_input(arguments);

    free(input);
    free(arguments);

  }while (status);

}

//Reads in user input
char *read_input()
{
  ssize_t buffer_size = STRINGSIZE;
  char * line = NULL;

  getline(&line,&buffer_size,stdin);
  return line;
}

//Parses user input
// Personal note ( This function is a pointer that points to pointers)
char **parse_input(char *input)
{
  int buffer_size = STRINGSIZE + 1;
  int index = 0;
  char delimiters[] = " \t | > < & ; \n";
  char *token;
  char **tokens = malloc(buffer_size * sizeof(char*));

  if(!tokens)
  {
      fprintf(stderr, "Shell: Error allocating memory.\n");
      exit(EXIT_FAILURE);
  }
  token = strtok(input,delimiters);

  while( token != NULL){
      tokens[index] = token;
      index++;

      if(buffer_size >= index)
      {
        tokens = realloc(tokens,buffer_size * sizeof(char*));
        if(!tokens)
        {
            fprintf(stderr, "Shell: Error allocating memory.\n");
            exit(EXIT_FAILURE);
        }
      }
      token = strtok(NULL, delimiters);
  }
  tokens[index] = NULL;

  return tokens;
}

int execute_input(char **arguments){
  // function that will call lots of other functions
  // Is this an efficient or elegant way to call the functions?
  // what about a char list that holds "functions" like the one we have for delimiters?
  //Might let us use a switch statement or something?
  //Maybe multiple IF's is not "clean" but is "elegant"
  if(arguments[0] == NULL)
  {
    return 0;
  }

  if((strcmp(arguments[0],"exit") == 0) || (**arguments == 120))
  {
    printf("\n");
    return 0;
  }

  if((strcmp(arguments[0],"cd") == 0))
  {
    return shell_cd(arguments);
  }

  if((strcmp(arguments[0],"getpath") == 0))
  {
    return shell_getpath();
  }

  if((strcmp(arguments[0],"setpath") == 0))
  {
    return shell_setpath(arguments);
  }

  return create_process(arguments);
}

int create_process(char **arguments)
{
  pid_t pid;
  pid = fork();

  if(pid < 0)
  {
    fprintf(stderr, "Fork Failed\n");
    perror("Shell");
    exit(-1);
  }
  else if(pid == 0)
  {
    if(execvp(arguments[0],arguments) == -1)
    {
      perror("Shell");
    }
    exit(EXIT_FAILURE);
  }
  else
  {
    wait(NULL);
    return 1;
  }
  return 1;
}
