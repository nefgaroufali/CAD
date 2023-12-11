#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl8.6/tcl.h>

#include "tcl.h"
#include "my_parse.h"
#include "my_structs.h"

#define NRM  "\x1B[0m"		// Normal Color
#define RED  "\x1B[31m"		// Red Color

Tcl_Interp *interp_nef;

// Initialisation of TCL //
int init_interpreter()
{
	interp_nef = NULL;
	interp_nef = Tcl_CreateInterp();	// Tcl_CreateInterp creates a new interp_nef structure and returns a token for it //
		
	// define my tcl comands //
	Tcl_CreateObjCommand(interp_nef, "less", less, NULL, NULL);
	Tcl_CreateObjCommand(interp_nef, "ls", ls, NULL, NULL);

	// define commands for the parsing of the file
	Tcl_CreateObjCommand(interp_nef, "read_design", read_design, NULL, NULL);
	Tcl_CreateObjCommand(interp_nef, "list_components", list_components, NULL, NULL);
	//Tcl_CreateObjCommand(interp_nef, "list_IOs", list_IOs, NULL, NULL);
	Tcl_CreateObjCommand(interp_nef, "report_component_function", report_component_function, NULL, NULL);
	Tcl_CreateObjCommand(interp_nef, "report_component_type", report_component_type, NULL, NULL);
	Tcl_CreateObjCommand(interp_nef, "list_component_CCS", list_component_CCS, NULL, NULL);
	//Tcl_CreateObjCommand(interp_nef, "list_IO_CCS", list_IO_CCS, NULL, NULL);

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

// read_design, implementation of read_design command
int read_design(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[])
{
	const char syntax[] = "<filepath>";

	int len = 0;
	FILE *fp = NULL;
	char *file = NULL;

	if (argc != 2)
	{
		Tcl_WrongNumArgs(interp, 1, argv, syntax);
		return TCL_ERROR;
	}

	file = Tcl_GetStringFromObj(argv[1], &len);

	fp = fopen(file, "r");

	if (fp == NULL)
	{
		printf("File does not exist!\n");
		return TCL_ERROR;
	}

	parse_design_file(fp);
	// fclose(fp);

	print_comp_hash_table(&comp_hash_table);
	print_lib_hash_table(&lib_hash_table);
	print_gatepins_hash_table(&gatepins_hash_table);

	return TCL_OK;
}

// list_components, implementation of list_components command
int list_components(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[])
{
	// there are no args, just print all the components names
	for(int i = 0; i < comp_hash_table.size; i++)
	{
		if(comp_hash_table.table[i] != NULL)
		{
			printf("%s\n", comp_hash_table.table[i]->comp_name);
		}
	}

	return TCL_OK;

}

// report_component_type, implementation of report_component_type command
int report_component_type(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[])
{
	char *comp_name;
	struct component *component = NULL;

	// check if the hash tables are null
	if(comp_hash_table.table == NULL)
	{
		fprintf(stderr, RED"!!!Error: No components in the design. !!! Must first call read_design\n"NRM);
		return TCL_OK;
	}
	
	// there is one arg, the component name
	if(argc != 2)
	{
		fprintf(stderr, RED"!!!Error: Wrong number of arguments\n"NRM);
		fprintf(stderr, NRM"Expected: report_component_type <component_name>\n"NRM);
		return TCL_OK;
	}

	comp_name = Tcl_GetString(argv[1]);
	component = find_component(comp_name);

	// print the component name and the component type
	printf("%s %s\n", component->comp_name, component->lib_cell->lib_cell_name);
	printf("\n");

	return TCL_OK;
}

int report_component_function(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[])
{
	char *comp_name;
	struct component *component = NULL;

	// check if the hash tables are null
	if(comp_hash_table.table == NULL)
	{
		fprintf(stderr, RED"!!!Error: No components in the design. !!! Must first call read_design\n"NRM);
		return TCL_OK;
	}
	
	// there is one arg, the component name
	if(argc != 2)
	{
		fprintf(stderr, RED"!!!Error: Wrong number of arguments\n"NRM);
		fprintf(stderr, NRM"Expected: report_component_function <component_name>\n"NRM);
		return TCL_OK;
	}

	comp_name = Tcl_GetString(argv[1]);
	component = find_component(comp_name);

	printf("Component: %s, Function(s): \n", component->comp_name);

	// print the component name and the component function
	int i=0;
	while(component->gatepins_array[i] != NULL)
	{
		if(component->gatepins_array[i]->io == 1)
		{
			printf("%s\n", component->gatepins_array[i]->function);
		}
		i++;
	}

	printf("\n");

	return TCL_OK;
}

int list_component_CCS(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[])
{
	char *comp_name;
	struct component *component = NULL;

	// check if the hash tables are null
	if(comp_hash_table.table == NULL)
	{
		fprintf(stderr, RED"!!!Error: No components in the design. !!! Must first call read_design\n"NRM);
		return TCL_OK;
	}
	
	// there is one arg, the component name
	if(argc != 2)
	{
		fprintf(stderr, RED"!!!Error: Wrong number of arguments\n"NRM);
		fprintf(stderr, NRM"Expected: list_component_CCS <component_name>\n"NRM);
		return TCL_OK;
	}

	comp_name = Tcl_GetString(argv[1]);
	component = find_component(comp_name);

	printf("Component: %s, CCS: \n", component->comp_name);

	// print the component name and the component CCS
	int i=0;
	int j=0;
	while(component->gatepins_array[i] != NULL)
	{
		if(component->gatepins_array[i]->io == 1)
		{
			while(component->gatepins_array[i]->connections[j] != NULL)
			{
				printf("%s\n", component->gatepins_array[i]->connections[j]->component->comp_name);
				j++;
			}
		}
		i++;
	}

	printf("\n");

	return TCL_OK;
}