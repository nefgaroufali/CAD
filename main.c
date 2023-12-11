#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <tcl8.6/tcl.h>

#include "readline.h"
#include "tcl.h"
#include "my_parse.h"
#include "my_structs.h"

#define NRM  "\x1B[0m"		// Normal Color
#define RED  "\x1B[31m"		// Red Color

int main(int argc, char* argv[])
{
    // ************** Readline Initialisation ************** //
    char *text = NULL; // readline result //
    char *textexpansion; // readline result history expanded //
    int expansionresult;
    int tcl_res;
    HIST_ENTRY **the_history_list; // readline commands history list - NULL terminated //
    char command[LINE_MAX]; // current command //
    unsigned long i;
    
    // use rl_filename_completion_function(), the default filename completer //
    rl_completion_entry_function = NULL; 
    rl_attempted_completion_function = custom_completer;
    rl_completion_append_character = '\0';
    
    // ************** TCL Initialisation ************** //
    // arg[0] -> current executable
	Tcl_FindExecutable(argv[0]);

	// make interp for tcl. tcl_interp is defined in src/tcl/tcl.h library
	init_interpreter();
    
    printf("\n\n\n");
   
    printf("     ______       _       ______    \n");
    printf("   .' ___  |     / \\     |_   _ `.  \n");
    printf("  / .'   \\_|    / _ \\      | | `. \\ \n");
    printf("  | |          / ___ \\     | |  | | \n");
    printf("  \\ `.___.'\\ _/ /   \\ \\_  _| |_.' / \n");
    printf("   `.____ .'|____| |____||______.'  \n");

    printf("\n\n\n");
    printf("************************************");
    printf("\n");
    printf("Name: Garoufali Maria Nefeli\n");
    printf("AEM: 3129\n");
    printf("Subject: CAD\n");
    printf("************************************");
    
    printf("\n");
    printf("\n");
    printf("\n");
    
    
    while (1)
    {
        // Read the command
        text = readline("PR> ");
        if (text != NULL)
        {
            expansionresult = history_expand(text, &textexpansion);
            if ((expansionresult == 0) || // no expansion //
            (expansionresult == 2)) // do not execute //
            {
                add_history(text);
                strcpy(command, text); // store command //
            }
            else
            {
                add_history(textexpansion);
                strcpy(command, textexpansion); // store command //
            }
            free(textexpansion);
            free(text);
        }
        // Handle two basic commands: history and quit //
        if (strcmp(command, "quit") == 0)
        {
            del_interpreter();
            free_comp_hash_table(&comp_hash_table);
            free_gatepins_hash_table(&gatepins_hash_table);
            free_lib_hash_table(&lib_hash_table);
            return EXIT_SUCCESS;
        }
        else if (strcmp(command, "history") == 0)
        {
            the_history_list = history_list(); // get history list //
            if (the_history_list != NULL)
            {
                i = 0;
                while (*(the_history_list + i) != NULL) // history list - NULL terminated //
                {
                    printf("%ld: %s\n", (i + history_base), (*(the_history_list + i))->line);
                    i++;
                }
            }
        }
        else // TCL commands //
		{	
			// evaluate tcl commands //
			tcl_res = Tcl_Eval(interp_nef, command);
			if (tcl_res == TCL_ERROR)
			{
				fprintf(stderr, RED"!!! Error !!!\n"NRM);
                // returns the result for interp_nef as a string //
				if (*Tcl_GetStringResult(interp_nef) != '\0')
				{
		  		 	printf("%s\n", Tcl_GetStringResult(interp_nef));
					// free result //
					Tcl_ResetResult(interp_nef);
				}
			} 
			else if (tcl_res == TCL_OK)
			{
				// returns the result for interp_nef as a string //
				if (*Tcl_GetStringResult(interp_nef) != '\0')
				{
		  			printf("%s\n", Tcl_GetStringResult(interp_nef));
					// free result //
					Tcl_ResetResult(interp_nef);
				}
			}
		}	
	}
	return EXIT_FAILURE;
}
