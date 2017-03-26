/*
The main.c file handles the user input, saving, loading and initialisation of the shells functions.
*/


/*Import libraries used.*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include "infunctions.h"


/* Define constants. */
#define STRINGSIZE 50
#define MAX_HISTORY_SIZE 20
#define MAX_ALIAS_SIZE 10


/* Define functions. */
void command_loop();
char *read_input();
char **parse_input(char *input);
int execute_input(char **arguments);
void add_to_history(char *input);
int shell_history();
void save_history();
void load_history();
char **shell_past_command(char **arguments);
int add_alias(char **arguments);
int print_aliases();
int remove_alias(char **arguments);
char **get_alias(char **arguments);
void save_aliases();
void load_aliases();
int create_process(char **arguments);


/* Global variables used to control the position of elements in history array. */
int history_index = 0;
int history_counter = 1;


/* Global variables used to control the position of elements in aliases array. */
int alias_index = 0;
int alias_counter = 1;


/*Array of structs that hold the required data for commands stored in history.*/
struct HISTORY{
  int command_id;
  char *cmd;
}history_array[MAX_HISTORY_SIZE];


/*Array of structs that hold the required data for commands stored as an alias. */
struct ALIAS{
  int command_id;
  char *cmd;
  char *alias;
}alias_array[MAX_ALIAS_SIZE];


/*Definition of this function after the ALIAS struct is required. */
void change_alias(struct ALIAS *x, char *cmd);


/* Main function that initialises the environment, loads files and starts the command loop.*/
int main(int argc, char const *argv[]){
  const char *old_path = getenv("PATH");
  chdir(getenv("HOME"));
  load_history();
  load_aliases();
  command_loop();
  chdir(getenv("HOME"));
  save_history();
  save_aliases();
  printf("Restoring Path...\n");
  setenv("PATH",old_path,1);
  return 0;
}


/*Allows the user to repeatedly enter input into the shell for execution. Until exit condtions are met.*/
void command_loop(){
  int status;
  char *input;
  char *item;
  char **arguments;
  char buf[1024];

  do{
    //Prompt
    printf("%s$ ", getcwd(buf,sizeof(buf)));
    //Read
    if((input = read_input()) == NULL){
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


/*Returns the entered user input. */ //***BUFFER PROBLEM?
char *read_input(){
  ssize_t buffer_size = STRINGSIZE;
  char *line;
  char *cmd = malloc(buffer_size * sizeof(char*));
  char *buffer = malloc(buffer_size * sizeof(char*));
  int check;

  if(!buffer){
    fprintf(stderr, "Shell : Error allocating memory.\n");
    exit(EXIT_FAILURE);
  }
  line = fgets(buffer, buffer_size,stdin);
  strncpy(cmd,buffer,STRINGSIZE);

  if((strcspn(cmd,"!") == 0) || (cmd[0] == '\n') || (line == NULL)){
    return line;
  }else{
    add_to_history(cmd);
  }

  return line;
}


/*Tokenises the input from the user and returns an array of tokens. */
char **parse_input(char *input){
  int buffer_size = STRINGSIZE + 1;
  int index = 0;
  char delimiters[] = " \t | > < & ; \n";
  char *token;
  char **tokens = malloc(buffer_size * sizeof(char*));

  if(!tokens){
      fprintf(stderr, "Shell: Error allocating memory.\n");
      exit(EXIT_FAILURE);
  }
  token = strtok(input,delimiters);

  while( token != NULL){
      tokens[index] = token;
      index++;

      if(buffer_size >= index){
        tokens = realloc(tokens,buffer_size * sizeof(char*));
        if(!tokens){
            fprintf(stderr, "Shell: Error allocating memory.\n");
            exit(EXIT_FAILURE);
        }
      }
      token = strtok(NULL, delimiters);
  }
  tokens[index] = NULL;

  return tokens;
}


/*Takes parsed input and if possible matches it to a funtion within the shell. Else returns an error. */
int execute_input(char **arguments){

  if(arguments[0] == NULL){
    return 1;
  }

  if(strcspn(arguments[0],"!") == 0){
    arguments = shell_past_command(arguments);
    if(arguments[0] == NULL){
      return 1;
    }
  }

  arguments = get_alias(arguments);

  if((strcmp(arguments[0],"history") == 0)){
    if(arguments[1] == NULL){
      shell_history();
      return 1;
    }else{
      fprintf(stderr, "Invalid arguments.\n");
      return 1;
    }
  }

  if((strcmp(arguments[0],"exit") == 0)){
    printf("\n");
    return 0;
  }

  if((strcmp(arguments[0],"alias") == 0) && (arguments[1] == NULL) && (arguments[2] == NULL)){
    return print_aliases();
  }

  if((strcmp(arguments[0],"alias") == 0)){
    return add_alias(arguments);
  }

  if((strcmp(arguments[0],"unalias") == 0)){
    return remove_alias(arguments);
  }

  if((strcmp(arguments[0],"cd") == 0)){
    return shell_cd(arguments);
  }

  if((strcmp(arguments[0],"getpath") == 0)){
    return shell_getpath(arguments);
  }

  if((strcmp(arguments[0],"setpath") == 0)){
    return shell_setpath(arguments);
  }

  return create_process(arguments);
}


/*Adds the user input to the history array.*/
void add_to_history(char *input){
  if(history_array[history_index].cmd != NULL){
    free(history_array[history_index].cmd);
  }

  history_array[history_index].cmd = strndup(input,STRINGSIZE+1);
  history_array[history_index].command_id = history_counter;
  history_index = (history_index + 1 ) % MAX_HISTORY_SIZE;
  history_counter++;
}


/*Prints the history array. */
int shell_history(){
  int i,j;

  for(i = history_index; i < MAX_HISTORY_SIZE; i++){
    if(history_array[i].cmd){
      printf("#%d %s\n",history_array[i].command_id,history_array[i].cmd);
    }
  }

  for(j = 0; j < history_index; j++){
    if(history_array[j].cmd){
      printf("#%d %s\n",history_array[j].command_id,history_array[j].cmd);
    }
  }
  return 1;
}


/*Retrieves past commands from the history depending on the history function used.
 * !! retrieves the most recent item in history.
 * !<no> retrieves the history element listed under that number from history.
 * !-<no> retrieves the history element listed under the number from the current element minus the number.
 * Then executes the retrieved commands.
 */
char **shell_past_command(char **arguments){
  int a = 0;
  char *str;
  int b = 0;
  int c = 0;
  int i,j;
  char *temp_cmd = malloc(STRINGSIZE * sizeof(char*));
  char *temp_arg = malloc(STRINGSIZE * sizeof(char*));

  /*Checks if !! history call was entered. */
  if(strcmp(arguments[0],"!!") == 0){
    if(history_array[history_index-1].cmd == NULL){
      arguments[0] = NULL;
      fprintf(stderr, "ERROR: No elements in history.\n");
    }else{
      if(arguments[1] == NULL){
        strncpy(temp_cmd,history_array[history_index-1].cmd,STRINGSIZE+1);
        arguments = parse_input(temp_cmd);
      }else{
          i = 1;
          strncpy(temp_cmd,history_array[history_index-1].cmd,STRINGSIZE+1);
          do{
            if(arguments[i] != NULL){
              strncpy(temp_arg,arguments[i],STRINGSIZE+1);
            }
            strncat(temp_cmd," ",STRINGSIZE+1);
            strncat(temp_cmd,temp_arg,STRINGSIZE+1);
            i++;
          }while(arguments[i] != NULL);
          arguments = parse_input(temp_cmd);
        }
      }
    return arguments;
  }

  /*Checks if !<no> history call was entered. */
  if((arguments[0][0]=='!') && (arguments[0][1]!='-')){
    a = strtol(&arguments[0][1],&str,10);
    if((a <= 0) || (a > history_counter)){
      arguments[0] = NULL;
      fprintf(stderr, "ERROR: Not valid history function. Command IDs start a numeral 1.\n");
      return arguments;
    }
    for(i = 0; i < MAX_HISTORY_SIZE; i ++){
      if(history_array[i].command_id == a){
        strncpy(temp_cmd,history_array[i].cmd,STRINGSIZE+1);
        if(arguments[1] != NULL){
          j = 1;
          do{
            if(arguments[j] != NULL){
              strncpy(temp_arg,arguments[j],STRINGSIZE+1);
            }
            strncat(temp_cmd," ",STRINGSIZE+1);
            strncat(temp_cmd,temp_arg,STRINGSIZE+1);
            j++;
          }while(arguments[j] != NULL);
          arguments = parse_input(temp_cmd);
          return arguments;
        }else{
          arguments = parse_input(temp_cmd);
          return arguments;
        }
      }
    }
  }

  /*Checks if !-<no> history call is entered. */
  if((arguments[0][1]=='-') && (arguments[0][0]=='!')){
    a = strtol(&arguments[0][2],&str,10);
    b = history_counter;
    if(a < 1){
      arguments[0] = NULL;
      fprintf(stderr, "ERROR: Not valid history function. Command index start a numeral 1.\n");
      return arguments;
    }
    c = b - a - 1;
    c = c % MAX_HISTORY_SIZE;
    if(c <= 0){
      arguments[0] = NULL;
      fprintf(stderr, "ERROR: Not valid history function. Command index specified is less than 1.\n");
      return arguments;
    }
    strncpy(temp_cmd,history_array[c-1].cmd,STRINGSIZE+1);
    if(arguments[1] != NULL){
      i = 1;
      do{
        if(arguments[i] != NULL){
          strncpy(temp_arg,arguments[i],STRINGSIZE+1);
        }
        strncat(temp_cmd," ",STRINGSIZE+1);
        strncat(temp_cmd,temp_arg,STRINGSIZE+1);
        i++;
      }while(arguments[i] != NULL);
      arguments = parse_input(temp_cmd);
      return arguments;
    }else{
      arguments = parse_input(temp_cmd);
      return arguments;
    }
  }

  arguments[0] = NULL;
  fprintf(stderr, "ERROR: Not valid history function.\n");
  return arguments;
}


/*Saves all elements currently in the history array to file .hist_list. */
void save_history(){
  FILE * source = fopen(".hist_list", "w+");
  int i,j;

  for(i = history_index; i < MAX_HISTORY_SIZE; i++){
    if(history_array[i].cmd){
      fprintf(source, "%d %s",history_array[i].command_id,history_array[i].cmd);
    }
  }

  for(j = 0; j < history_index; j++){
    if(history_array[j].cmd){
      fprintf(source, "%d %s",history_array[j].command_id,history_array[j].cmd);
    }
  }

  fclose (source);
}


/*Loads all elements in file .hist_list into the history array. */
void load_history(){
  FILE * source = fopen(".hist_list", "r");
  char **temp_hist = malloc(MAX_HISTORY_SIZE * sizeof(char*));
  char buffer[STRINGSIZE+1];
  char cmd[STRINGSIZE+1];
  char *str;
  char **cmds;
  int cmd_id,i,j;

  if(!source){
    fprintf(stderr, "Error unable to open History file.\n");
    fprintf(stderr, "New file will be created upon exit.\n");
    return;
  }

  j = 0;
  while(fgets(buffer,sizeof(buffer),source)){
    cmd_id = strtol(&buffer[0],&str,10);

    if(cmd_id < history_counter){
      history_counter = cmd_id;
    }

    cmds = parse_input(buffer);
    strncpy(cmd,cmds[1],STRINGSIZE+1);

    for(i = 2; i < STRINGSIZE; i++){
      if(cmds[i] != NULL){
        strncat(cmd," ",STRINGSIZE+1);
        strncat(cmd,cmds[i],STRINGSIZE+1);
      }
    }
    strncat(cmd,"\n",STRINGSIZE+1);
    temp_hist[j] = strndup(cmd,STRINGSIZE+1);
    j++;
  }

  for(i = 0; i < MAX_HISTORY_SIZE; i++){
    if(temp_hist[i]){
      add_to_history(temp_hist[i]);
    }
  }

  fclose(source);
}


/* Adds an alias to the aliases array.
 * Overwrites the element if it already exists in the array.
 */
int add_alias(char **arguments){
  int i,index;
  int j = 3;
  int existing_alias = 1;
  char *temp_cmd = malloc(STRINGSIZE * sizeof(char*));
  char *temp_arg = malloc(STRINGSIZE * sizeof(char*));

  if(arguments[1] != NULL && arguments[2] != NULL){
    if(alias_array[alias_index].cmd != NULL){
      free(alias_array[alias_index].alias);
      free(alias_array[alias_index].cmd);
    }

    /*Check if the alias exists in the alias array. */
    for(i = 0; i < MAX_ALIAS_SIZE; i++){
      if(alias_array[i].alias){
        if(strcmp(alias_array[i].alias,arguments[1]) == 0){
          existing_alias = 0;
          index = i;
        }
      }
    }

    /*Check if the alias array is full.*/
    if(alias_index >= MAX_ALIAS_SIZE && existing_alias != 0){
        fprintf(stderr, "Alias list full. Cannot add alias.\n");
        return 1;
      }

    /*If the alias does exist, overwrite the alias. */
    if(existing_alias ==  0){
      strncpy(temp_cmd,arguments[2],STRINGSIZE+1);
      do{
        if(arguments[j] != NULL){
          strncpy(temp_arg,arguments[j],STRINGSIZE+1);
        }
        strncat(temp_cmd," ",STRINGSIZE+1);
        strncat(temp_cmd,temp_arg,STRINGSIZE+1);
        j++;
      }while(arguments[j] != NULL);
      change_alias(&alias_array[index],temp_cmd);
      fprintf(stderr, "Alias %s has been overwritten.\n",alias_array[index].alias);
      return 1;
    }else{
      /*If the alias does not exist, add the new alias. */
      alias_array[alias_index].alias = strdup(arguments[1]);
      if(arguments[3] == NULL){
        strncpy(temp_cmd,arguments[2],STRINGSIZE+1);
      }else{
        strncpy(temp_cmd,arguments[2],STRINGSIZE+1);
        do{
          if(arguments[j] != NULL){
            strncpy(temp_arg,arguments[j],STRINGSIZE+1);
          }
          strncat(temp_cmd," ",STRINGSIZE+1);
          strncat(temp_cmd,temp_arg,STRINGSIZE+1);
          j++;
        }while(arguments[j] != NULL);
      }
    }
      alias_array[alias_index].cmd = strndup(temp_cmd,STRINGSIZE+1);
      alias_array[alias_index].command_id = alias_counter;
      alias_index = alias_index + 1;
      alias_counter++;
      return 1;
  }else{
    fprintf(stderr, "Invalid arguments.\n");
    return 1;
  }
}


/*Prints all aliases currently in the array.*/
int print_aliases(){
  int i;
  int k = 0;

  for(i = 0; i < MAX_ALIAS_SIZE; i++){
    if(alias_array[i].alias != NULL){
      printf("ID: %d Alias: %s  Command: %s\n",alias_array[i].command_id,alias_array[i].alias,alias_array[i].cmd);
    }else{
      k++;
    }
  }
  if(k == MAX_ALIAS_SIZE){
    fprintf(stderr, "No aliases found.\n");
  }

  return 1;
}


/*Removes an alias from the aliases array. */
int remove_alias(char **arguments){
  int i,index;
  int count = 0;
  int myBool = 1;

  for(i = 0; i < MAX_ALIAS_SIZE; i++){
    if(alias_array[i].alias != NULL){
      if(strcmp(alias_array[i].alias,arguments[1]) == 0){
        alias_array[i].command_id = 0;
        alias_array[i].alias = NULL;
        alias_array[i].cmd = NULL;
        myBool = 0;
        index = i;
      }
    }else{
      count++;
    }
  }

  if(count == MAX_ALIAS_SIZE){
    fprintf(stderr, "Alias list empty. Cannot remove alias.\n");
    return 1;
  }

  if(myBool == 1){
    fprintf(stderr, "Alias not found.\n");
    return 1;
  }

  alias_index = alias_index - 1;
  for(i = index; i <= MAX_ALIAS_SIZE; i++){
    alias_array[i] = alias_array[i+1];
  }
  return 1;
}


/*Changes the value of the command variable in an ALIAS struct.*/
void change_alias(struct ALIAS *a, char *new){
  a->cmd = new;
}


/*Checks if the argument entered is an alias. Returns the command if it is, returns the argument if its not.*/
char **get_alias(char **arguments){
  int i,j;
  char *temp_cmd = malloc(STRINGSIZE * sizeof(char*));
  char *temp_arg = malloc(STRINGSIZE * sizeof(char*));

  for(i = 0; i < MAX_ALIAS_SIZE; i++){
    if(alias_array[i].alias){
      if(strcmp(alias_array[i].alias,arguments[0]) == 0){
        if(arguments[1] == NULL){
          strncpy(temp_cmd,alias_array[i].cmd,STRINGSIZE+1);
          arguments = parse_input(temp_cmd);
        }else{
            j = 1;
            strncpy(temp_cmd,alias_array[i].cmd,STRINGSIZE+1);
            do{
              if(arguments[j] != NULL){
                strncpy(temp_arg,arguments[j],STRINGSIZE+1);
              }
              strncat(temp_cmd," ",STRINGSIZE+1);
              strncat(temp_cmd,temp_arg,STRINGSIZE+1);
              j++;
            }while(arguments[j] != NULL);
            arguments = parse_input(temp_cmd);
          }
      }
    }
  }
  return arguments;
}


/*Saves all elements currently in the alias array to the .aliases file. */
void save_aliases(){
  FILE * source = fopen(".aliases", "w+");
  int i,j;

  for(i = 0; i < MAX_HISTORY_SIZE; i++){
    if(alias_array[i].alias != NULL){
      fprintf(source, "%d %s %s\n",alias_array[i].command_id,alias_array[i].alias,alias_array[i].cmd);
    }
  }

  fclose (source);
}


/*Loads all elements in the .aliases file into the aliases array.*/
void load_aliases(){
  FILE * source = fopen(".aliases", "r");
  char buffer[STRINGSIZE+1];
  char **temp_alias = malloc(MAX_ALIAS_SIZE * sizeof(char*));
  char **cmds;

  if(!source){
    fprintf(stderr, "Error unable to open Alias file.\n");
    fprintf(stderr, "New file will be created upon exit.\n");
    return;
  }

  int i = 0;
  while(fgets(buffer,sizeof(buffer),source)){
    cmds = parse_input(buffer);
    if(cmds[0]){
      add_alias(cmds);
    }
    i++;
    if(i == 10){
      break;
    }
  }
  fclose(source);
}


/*Creates a child process.*/
int create_process(char **arguments){
  pid_t pid;
  pid = fork();

  if(pid < 0){
    fprintf(stderr, "Fork Failed\n");
    perror(arguments[0]);
    exit(-1);
  }
  else if(pid == 0){
    if(execvp(arguments[0],arguments) == -1){
      perror(arguments[0]);
    }
    exit(EXIT_FAILURE);
  }else{
    wait(NULL);
    return 1;
  }
  return 1;
}
