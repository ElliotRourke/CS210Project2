#include <stdio.h>

/* Define constants */
#define STRINGSIZE 512

/* Define functions */
void command_loop();
char * read_input();
char ** parse_input(char *input);

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

  }while (status);

}

//Reads in user input
char * read_input()
{
  size_t buffer_size = STRINGSIZE;
  char * line = NULL;

  getline(&line,&buffer_size,stdin);
  return line;
}

//Parses user input
// Personal note ( This function is a pointer that points to pointers)
char ** parse_input(char *input)
{
  char * token;
  //strtok used here

}
