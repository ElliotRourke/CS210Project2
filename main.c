//TODO
// IMPROVE ERROR MESSAGES
// CD (Should try to check which file the user wanted)
// Setpath still not finished what if we put setpath //////



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "infunctions.h"

/* Define constants */
#define STRINGSIZE 50
#define MAX_HISTORY_SIZE 20

/* Define functions */
void command_loop();
char *read_input();
char **parse_input(char *input);
int execute_input(char **arguments);
void add_to_history(char *input);
int shell_history();
int create_process(char **arguments);

char *history_array[MAX_HISTORY_SIZE];
int history_index = 0;

//This is a simple OS shell designed by our team!
int main(int argc, char const *argv[])
{
  history_array[MAX_HISTORY_SIZE] = malloc(MAX_HISTORY_SIZE * sizeof(char*));
  const char *old_path = getenv("PATH");
  chdir(getenv("HOME"));
  command_loop();
  printf("Restoring Path...\n");
  setenv("PATH",old_path,1);
  return 0;
}

//Handles commands input by user
void command_loop()
{
  int status;
  char *input;
  char *item;
  char **arguments;
  char buf[1024];

  do{
    //Prompt
    printf("%s$ ", getcwd(buf,sizeof(buf)));
    //Read
    if((input = read_input()) == NULL)
    {
      break;
    }
    //Parse
    arguments = parse_input(input);
    //Execute
    status = execute_input(arguments);

    free(input);
    free(arguments);
  }while (status);

}

char *read_input()
{
  ssize_t buffer_size = STRINGSIZE;
  char *line;
  char *cmd = malloc(buffer_size * sizeof(char*));
  char *buffer = malloc(buffer_size * sizeof(char*));
  int check;

  if(!buffer)
  {
    fprintf(stderr, "Shell : Error allocating memory.\n");
    exit(EXIT_FAILURE);
  }
  line = fgets(buffer, buffer_size,stdin);
  strcpy(cmd,buffer);
  add_to_history(cmd);
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

  while( token != NULL)
  {
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

  if(arguments[0] == NULL)
  {
    return 1;
  }

  if((strcmp(arguments[0],"history") == 0))
  {
    shell_history();
    return 1;
  }

  if((strcmp(arguments[0],"exit") == 0))
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
    return shell_getpath(arguments);
  }

  if((strcmp(arguments[0],"setpath") == 0))
  {
    return shell_setpath(arguments);
  }

  if((strcmp(arguments[0],"help") == 0))
  {
    return shell_help(arguments);
  }

  return create_process(arguments);
}

void add_to_history(char *input)
{
  history_array[history_index%MAX_HISTORY_SIZE] = input;
  history_index++;
}

//TODO
//THIS METHOD **WORKS**
//The print is just weird because it overrides the pervious items in the array
//Whilst leaving the other old items...
int shell_history()
{
  int i = 0;
  int hist_num = 1;

  do
  {
    if(history_array[i])
    {
      printf("%d %s\n",hist_num,history_array[i]);
      hist_num++;
    }
    i = (i + 1) % MAX_HISTORY_SIZE;

  }while(i != 0);

  return 1;
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
