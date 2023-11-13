#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl8.6/tcl.h>

#include "tcl.h"


Tcl_Interp *interp_nef;

// Tcl_CreateInterp creates a new interp_nef structure and returns a token for it //
int init_interpreter()
{
	interp_nef = NULL;
	interp_nef = Tcl_CreateInterp();
		
	// define new tcl comands //
	Tcl_CreateObjCommand(interp_nef, "less", less, NULL, NULL);
	Tcl_CreateObjCommand(interp_nef, "ls", ls, NULL, NULL);

	if (Tcl_Init(interp_nef) == TCL_ERROR)
	{
		//fprintf(stderr, RED"!!!Error while creating interp_nef  \n"NRM);
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
		Tcl_SetResult( interp, "Wrong number of arguments", clientdata);
		return TCL_ERROR;
	}

	if(argc == 2)
	{
		arg1 = Tcl_GetStringFromObj(argv[1], &arg1_length);
		if (arg1 == NULL)
		{
			Tcl_SetResult( interp, "Error at Tcl_GetStringFromObj", clientdata);
			return TCL_ERROR;
		}

		cmd = (char *) malloc((arg1_length + 4) * sizeof(char));  //2 for ls 1 for whitespace and 1 for \0
		if (cmd == NULL)
		{
			printf("Problem in allocation of memory. Program exits...\n");
			exit(1);
		}

		sprintf(cmd, "ls %s", arg1);
	}
	else if(argc == 3)
	{
		arg1 = Tcl_GetStringFromObj(argv[1], &arg1_length);
		arg2 = Tcl_GetStringFromObj(argv[2], &arg2_length);

		if(arg1 == NULL || arg2 == NULL)
		{
			printf("Error at Tcl_GetStringFromObj\n");
			return TCL_ERROR;
		}

		cmd = (char *) malloc((arg1_length + arg2_length + 5) * sizeof(char));  //2 for ls 2 for whitespace and 1 for \0
		if (cmd == NULL)
		{
			printf("Problem in allocation of memory. Program exits...\n");
			exit(1);
		}

		sprintf(cmd, "ls %s %s", arg1, arg2);
	
	}
	else
	{
		cmd = strdup("ls");
		if (cmd == NULL){
			printf("Problem in allocation of memory. Program exits...\n");
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
		printf("Wrong number of arguments!\n");
		return TCL_OK;
	}

	arg = Tcl_GetStringFromObj(argv[1], &length);
	if (arg == NULL)
	{
		printf("Error at Tcl_GetStringFromObj\n");
		return TCL_ERROR;
	}

	cmd = (char *) malloc((length + 6) * sizeof(char)); //4 for less, 1 for whitespace and 1 for \0
	if (cmd == NULL)
	{
		printf("Unable to Allocate Memory! Exiting...\n");
		exit(1);
	}

	sprintf(cmd, "less %s", arg);

	system(cmd);
	free(cmd);

	return TCL_OK;
}
