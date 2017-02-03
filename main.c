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

  //TODO
  // Initialise shell with "setenv" so that it can,
  // use PATH for paths and other parameters external functions need.
  command_loop();

  return 0;
}

//Handles commands input by user
void command_loop()
{
  int status;
  char *input;
  char **arguments;

  do{
    //Prompt
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
    return 1;
  }

  if((strcmp(arguments[0],"exit") == 0) || (**arguments == 120))
  {
    printf("\n");
    return 0;
  }

  if((strcmp(arguments[0],"cd") == 0))
  {
    shell_cd(arguments);
    return 1;
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
    execvp(arguments[0],arguments);
  }
  else
  {
    wait(NULL);
    return 1;
  }
  return 1;
}
