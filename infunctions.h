#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

/* Define functions*/
int shell_cd(char **arguments);
int shell_getpath();
int shell_setpath(char **arguments);
int shell_help();
int shell_history(char **arguments);

#endif
