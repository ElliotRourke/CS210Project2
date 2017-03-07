#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

/* Define functions*/
int shell_cd(char **arguments);
int shell_getpath();
int shell_setpath(char **arguments);
int char_to_int(char * temp);
int shell_help();
int shell_history();
#endif
