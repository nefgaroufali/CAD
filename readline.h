#ifndef READLINE_H
#define READLINE_H


extern char *commands_list[];
// commands list //

// function prototypes //
// complete commands with target for completion text bounded //
// by start and end  //
char **custom_completer(const char *, int, int);

// generate command for tab completetion, use state to find out if //
// we need to start from scratch or just the top of the list //
char *tab_generator(const char *, int);

#endif
