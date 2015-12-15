#ifndef _PROMPT_H
#define _PROMPT_H

typedef struct {
  char *name;			/* User printable name of the function. */
  int (*func)(unsigned int, char**);		/* Function to call to do the job. */
  char *doc;			/* Documentation for this function.  */
} COMMAND;

extern int exit_prog(unsigned int argc, char* argv[]);
extern int help(unsigned int argc, char* argv[]);
extern void commandPrompt();

#endif
