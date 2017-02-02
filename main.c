#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

/* Define constants */
#define STRINGSIZE 512

/* Define functions */
void command_loop();
char *read_input();
char **parse_input(char *input);
int execute_input(char **arguments);

//This is a simple OS shell designed by our team!
int main(int argc, char const *argv[])
{
  //Firstly initialize the shell - load any configs etc

  //Secondly enter "command loop"
  command_loop();
  //Perform clean-up for exit/shut-down


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
  size_t buffer_size = STRINGSIZE;
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

  token = strtok(input,delimiters);

  while( token != NULL){
      tokens[index] = token;
      index++;
      printf("'%s'\n",token);
      token = strtok(NULL, delimiters);

  }

  tokens[index] = NULL;

  return tokens;
}

int execute_input(char **arguments){
  // function that will call lots of other functions

  if((strcmp(arguments[0],"exit") == 0))
  {
    return 0;
  }

  if((strcmp(arguments[0],"cd") == 0))
  {
      int b = 1;
      shell_cd(b);
  }

  return 1;
}
