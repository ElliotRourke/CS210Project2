//TODO
// IMPROVE ERROR MESSAGES
// CD (Should try to check which file the user wanted)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include "infunctions.h"

/* Define constants */
#define STRINGSIZE 50
#define MAX_HISTORY_SIZE 20
#define MAX_ALIAS_SIZE 10

/* Define functions */
void command_loop();
char *read_input();
char **parse_input(char *input);
int execute_input(char **arguments);
void add_to_history(char *input);
int shell_history();
char **shell_past_command(char **arguments);
void save_history(FILE * source);
void load_history(FILE * source);
int add_alias(char **arguments);
int create_process(char **arguments);

int history_index = 0;
int history_counter = 1;
int alias_index = 0;
int alias_counter = 1;

struct HISTORY{
  int command_id;
  char *cmd;
}history_array[MAX_HISTORY_SIZE];

struct ALIAS{
  int command_id;
  char *cmd;
  char *alias;
}alias_array[MAX_ALIAS_SIZE];


//This is a simple OS shell designed by our team!
int main(int argc, char const *argv[]){
  FILE * source;
  const char *old_path = getenv("PATH");
  chdir(getenv("HOME"));
  load_history(source);
  command_loop();
  chdir(getenv("HOME"));
  save_history(source);
  printf("Restoring Path...\n");
  setenv("PATH",old_path,1);
  return 0;
}

//Handles commands input by user
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
  strcpy(cmd,buffer);

  if((strcspn(cmd,"!") == 0) || (cmd[0] == '\n')){
    return line;
  }else{
    add_to_history(cmd);
  }

  return line;
}

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

  if((strcmp(arguments[0],"alias") == 0)){
    return add_alias(arguments);
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

  if((strcmp(arguments[0],"help") == 0)){
    return shell_help(arguments);
  }

  return create_process(arguments);
}

void add_to_history(char *input){
  if(history_array[history_index].cmd != NULL){
    free(history_array[history_index].cmd);
  }

  history_array[history_index].cmd = strdup(input);
  history_array[history_index].command_id = history_counter;
  history_index = (history_index + 1 ) % MAX_HISTORY_SIZE;
  history_counter++;
}

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

char **shell_past_command(char **arguments){
  int a = 0;
  char *str;
  int b = 0;
  int c = 0;
  int i,j;
  char *temp_cmd = malloc(STRINGSIZE * sizeof(char*));
  char *temp_arg = malloc(STRINGSIZE * sizeof(char*));

  if(strcmp(arguments[0],"!!") == 0){
    if(history_array[history_index-1].cmd == NULL){
      arguments[0] = NULL;
      fprintf(stderr, "ERROR: No elements in history.\n");
    }else{
      if(arguments[1] == NULL){
        strcpy(temp_cmd,history_array[history_index-1].cmd);
        arguments = parse_input(temp_cmd);
      }else{
          i = 1;
          strcpy(temp_cmd,history_array[history_index-1].cmd);
          do{
            if(arguments[i] != NULL){
              strcpy(temp_arg,arguments[i]);
            }
            strcat(temp_cmd," ");
            strcat(temp_cmd,temp_arg);
            i++;
          }while(arguments[i] != NULL);
          arguments = parse_input(temp_cmd);
        }
      }
    return arguments;
  }

  if((arguments[0][0]=='!') && (arguments[0][1]!='-')){
    a = strtol(&arguments[0][1],&str,10);
    if((a <= 0) || (a > history_counter)){
      arguments[0] = NULL;
      fprintf(stderr, "ERROR: Not valid history function. Command IDs start a numeral 1.\n");
      return arguments;
    }
    for(i = 0; i < MAX_HISTORY_SIZE; i ++){
      if(history_array[i].command_id == a){
        strcpy(temp_cmd,history_array[i].cmd);
        if(arguments[1] != NULL){
          j = 1;
          do{
            if(arguments[j] != NULL){
              strcpy(temp_arg,arguments[j]);
            }
            strcat(temp_cmd," ");
            strcat(temp_cmd,temp_arg);
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
    strcpy(temp_cmd,history_array[c-1].cmd);
    if(arguments[1] != NULL){
      i = 1;
      do{
        if(arguments[i] != NULL){
          strcpy(temp_arg,arguments[i]);
        }
        strcat(temp_cmd," ");
        strcat(temp_cmd,temp_arg);
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

void save_history(FILE * source){
  source = fopen("hist_list.txt", "w+");
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

void load_history(FILE * source){
  source = fopen("hist_list.txt", "r");
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
    strcpy(cmd,cmds[1]);

    for(i = 2; i < STRINGSIZE; i++){
      if(cmds[i] != NULL){
        strcat(cmd," ");
        strcat(cmd,cmds[i]);
      }
    }
    strcat(cmd,"\n");
    temp_hist[j] = strdup(cmd);
    j++;
  }

  for(i = 0; i < MAX_HISTORY_SIZE; i++){
    if(temp_hist[i]){
      add_to_history(temp_hist[i]);
    }
  }

  fclose(source);
}

int add_alias(char **arguments){
  int i = 2;
  char temp[STRINGSIZE+1];

  if(arguments[1] != NULL && arguments[2] != NULL){
    if(alias_array[alias_index].cmd != NULL){
      free(alias_array[alias_index].alias);
      free(alias_array[alias_index].cmd);
    }

    alias_array[alias_index].alias = strdup(arguments[1]);
    //Need to check here - What if command has spaces?
    strcpy(temp,arguments[2]);
    for(i = 2; i < STRINGSIZE; i++){
      if(arguments[i] != NULL){
        strcat(temp," ");
        strcat(temp,arguments[i]);
      }
    }
    alias_array[alias_index].cmd = strdup(temp);

    alias_array[alias_index].command_id = alias_counter;
    alias_index = (alias_index + 1 ) % MAX_ALIAS_SIZE;
    alias_index++;

    return 1;
  }else{
    fprintf(stderr, "Invalid arguments. Please enter: alias <alias name> <command>\n");
    return 1;
  }
}

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
