#ifndef READLINE_H
#define READLINE_H


extern char *commands_list[];
// commands list //

// function prototypes //
// complete commands with target for completion text bounded //
// by start and end  //
char **custom_completer(const char *, int, int);

// generate command for tab completetion //
// return the matched string //
char *tab_generator(const char *, int);

#endif
