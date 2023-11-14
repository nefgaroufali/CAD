#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl8.6/tcl.h>

#include "tcl.h"

#define NRM  "\x1B[0m"		// Normal Color
#define RED  "\x1B[31m"		// Red Color
#define GRN  "\x1B[32m"		// Green Color
#define YEL  "\x1B[33m"		// Yellow Color
#define BLU  "\x1B[34m"		// Blue Color
#define MAG  "\x1B[35m"		// Magenta Color
#define CYN  "\x1B[36m"		// Cyan Color
#define WHT  "\x1B[37m"	    // White Color


Tcl_Interp *interp_nef;

// Initialisation of TCL //
int init_interpreter()
{
	interp_nef = NULL;
	interp_nef = Tcl_CreateInterp();	// Tcl_CreateInterp creates a new interp_nef structure and returns a token for it //
		
	// define my tcl comands //
	Tcl_CreateObjCommand(interp_nef, "less", less, NULL, NULL);
	Tcl_CreateObjCommand(interp_nef, "ls", ls, NULL, NULL);

	if (Tcl_Init(interp_nef) == TCL_ERROR)
	{
		fprintf(stderr, RED"!!!Error while creating interp_nef\n"NRM);
		return TCL_ERROR;
	}
	return TCL_OK;
}

// delete created interp_nef //
int del_interpreter()
{
	
	//Tcl_FreeResult(interp_nef);
	Tcl_DeleteInterp(interp_nef);
	Tcl_InterpDeleted(interp_nef);
	//Tcl_Free(interp_nef);
	Tcl_Finalize();
	return TCL_ERROR;
}

//ls, implementation of ls command
int ls(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[]){

	char *cmd;
	char *arg1, *arg2;
	int arg1_length, arg2_length;

	// Check the number of arguments //
	if(argc > 3)
	{
		fprintf(stderr, RED"!!! (ls) too much arguments \n\x1B[0m ls <arg> <directory> \n or ls <directory>\n");
		return TCL_ERROR;
	}
	if(argc == 2)
	{
		// Store arg1 as a string to built the command //
		arg1 = Tcl_GetStringFromObj(argv[1], &arg1_length);
		if (arg1 == NULL)
		{
			fprintf(stderr, RED"!!!Error in Tcl_GetStringFromObj  \n"NRM);
			return TCL_ERROR;
		}

		// Allocate memory for the command string //
		cmd = (char *) malloc((arg1_length + 4) * sizeof(char));  //2 for ls 1 for whitespace and 1 for \0
		if (cmd == NULL)
		{
			fprintf(stderr, RED"!!!Error in memory allocation \n"NRM);
			exit(1);
		}

		// Built the command //
		sprintf(cmd, "ls %s", arg1);
	}
	else if(argc == 3)
	{
		// Store arg1 and arg2 as strings to built the command //
		arg1 = Tcl_GetStringFromObj(argv[1], &arg1_length);
		arg2 = Tcl_GetStringFromObj(argv[2], &arg2_length);

		if(arg1 == NULL || arg2 == NULL)
		{
			fprintf(stderr, "\x1B[31m!!!Error in Tcl_GetStringFromObj  \n");
			return TCL_ERROR;
		}

		// Allocate memory for the string of the command //
		cmd = (char *) malloc((arg1_length + arg2_length + 5) * sizeof(char));  //2 for ls 2 for whitespace and 1 for \0
		if (cmd == NULL)
		{
			fprintf(stderr, RED"!!!Error in memory allocation \n"NRM);
			exit(1);
		}

		// Built the command //
		sprintf(cmd, "ls %s %s", arg1, arg2);
	
	}
	else
	{
		// Copy the command //
		cmd = strdup("ls");
		if (cmd == NULL){
			fprintf(stderr, RED"!!!Error in memory allocation \n"NRM);
			exit(1);
		}
	}

	system(cmd);
	free(cmd);

	return TCL_OK;
}

//less, implementation of less command
int less(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[])
{
	char *cmd;
	char *arg;
	int length = 0;

	// Check the number of arguments //
	if (argc != 2)
	{
		fprintf(stderr, RED"!!!Error: Wrong number of arguments\n"NRM);
		return TCL_OK;
	}

	// Store arg as a string to built the command //
	arg = Tcl_GetStringFromObj(argv[1], &length);
	if (arg == NULL)
	{
		fprintf(stderr, "\x1B[31m!!!Error in Tcl_GetStringFromObj  \n");
		return TCL_ERROR;
	}

	// Allocate memory for the string of the command //
	cmd = (char *) malloc((length + 6) * sizeof(char)); //4 for less, 1 for whitespace and 1 for \0
	if (cmd == NULL)
	{
		fprintf(stderr, RED"!!!Error in memory allocation \n"NRM);
		exit(1);
	}

	// Built the command //
	sprintf(cmd, "less %s", arg);

	system(cmd);
	free(cmd);

	return TCL_OK;
}
