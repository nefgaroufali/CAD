#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#include "my_parse.h"
#include "my_structs.h"

int component_count = 0;
int gatepins_count = 0;
int lib_count = 0;
int times_parsing = 0;
char *library_cells[100] = {NULL};
struct io io_array[IO_ARRAY_SIZE] = {NULL};
//char *io_names[100] = {NULL}; 


// This function counts the lines of the file that start with word "Component" //
// This is used to find the size of the hash table //
void count_components(FILE *fp)
{
    char *line = NULL;
    char *copy_line = NULL;
    char *token = NULL;
    size_t length = 0;
    int first_token = 0;

    while((getline(&line, &length, fp)) != -1)
    {
        copy_line = strdup(line);
        token = strtok(copy_line, DELIMITERS);
        first_token = check_first_token(token);
        free(copy_line);
        switch (first_token)
        {
            case IS_COMMENT:
                continue;
            case IS_ROW:
                continue;
            case IS_IO:
                continue;
            case IS_COMPONENT:
                component_count++;
                continue;
        }
    }
    
	printf("component_count = %d\n",component_count);
    free(line);

    return;
}

// this function counts the number of gatepins in the file //
void count_gatepins(FILE *fp)
{
	char *line = NULL;
	char *copy_line = NULL;
	char *token = NULL;
	char* tokens[LINE_MAX] = {NULL};
	size_t length = 0;
	int column_count = 0;

	while((getline(&line, &length, fp)) != -1)
	{
		copy_line = strdup(line);
		token = strtok(copy_line, DELIMITERS);
		
		while (token != NULL && column_count <  LINE_MAX -1)
		{
			tokens[column_count++] = token;
			token = strtok(NULL, DELIMITERS);
		}
		tokens[column_count] = NULL;

		if(column_count>=3 && strcmp(tokens[2],"Cell_Type:") == 0)
		{
			gatepins_count = gatepins_count + (column_count - 13)/2;
			gatepins_count++;
		}
		column_count = 0;
		free(copy_line);

	}
	
	printf("gatepins_count = %d\n",gatepins_count);
	//free(copy_line);
	free(line);

	return;
}

// This function counts the number of different library cells //
void count_library_cells(FILE *fp)
{
	char *line = NULL;
	char *copy_line = NULL;
	char *token = NULL;
	char* tokens[LINE_MAX] = {NULL};
	size_t length = 0;
	int column_count = 0;
	int i = 0;

	while((getline(&line, &length, fp)) != -1)
	{
		copy_line = strdup(line);
		token = strtok(copy_line, DELIMITERS);
		
		while (token != NULL && column_count <  LINE_MAX -1)
		{
			tokens[column_count++] = token;
			token = strtok(NULL, DELIMITERS);
		}
		tokens[column_count] = NULL;

		if(column_count>=3 && strcmp(tokens[2],"Cell_Type:") == 0)
		{
			for(i=0; i<100; i++)
			{
				if(library_cells[i] == NULL)
				{
					library_cells[i] = strdup(tokens[3]);
					lib_count++;
					break;
				}
				else if(strcmp(library_cells[i],tokens[3]) == 0)
				{
					break;
				}
			}
		}
		column_count = 0;
		free(copy_line);

	}
	printf("lib_count = %d\n",lib_count);
	
	//free(copy_line);
	free(line);

	return;
}


void parse_design_file(FILE *fp)
{
	char *line = NULL;
	char *copy_line = NULL;
	char *token = NULL;
	size_t length = 0;
	int first_token = 0;

    count_components(fp);
	rewind(fp);
	count_gatepins(fp);
	rewind(fp);
	count_library_cells(fp);
	create_comp_hash_table(component_count);
	create_lib_hash_table(lib_count);
	create_gatepins_hash_table(gatepins_count);
	rewind(fp);

	for(times_parsing = 0; times_parsing < 3; times_parsing++)
	{
		while((getline(&line, &length, fp)) != -1)
		{
			copy_line = strdup(line);
			token = strtok(copy_line, DELIMITERS);
			first_token = check_first_token(token);
			free(copy_line);
			switch (first_token)
			{
				case IS_COMMENT:
					continue;
				case IS_ROW:
					continue;
				case IS_IO:
					parse_io_line(line);
					continue;
				case IS_COMPONENT:
					parse_comp_line(line);
					continue;
			}
		}
		rewind(fp);
	}
	
	fclose(fp);
	free(line);

	printf(GRN"File parsed successfully!\n"NRM);
}

int check_first_token(char *string)
{
	const char comment = '#';
	size_t length = 0;
	int i;

	length = strlen(string);

	// Check if the string is the size of one character
	if (length == 1)
	{
		if (string[0] == comment) // If the character is equal to the comment 
			return IS_COMMENT;  // Return IS_comment
		else // If it is not
			return IS_SYNTAX_ERRROR;  // Return Syntax Error
	}

	// Check if string is ROW
	if (!strcmp(string, ROW))
		return IS_ROW;
	// Check if string is IO
	if (!strcmp(string, IO))
		return IS_IO;
	// Check if string is COMPONENT
	if (!strcmp(string, COMPONENT))
		return IS_COMPONENT;
    // Check if line is blank
    for (i = 0; i < length; i++)
    {
        if (string[i] != ' ' && string[i] != '\t' && string[i] != '\n')
            return IS_SYNTAX_ERRROR;
        
        if(i == length - 1)
            return IS_BLANK_LINE;
    }

	return SUCCESS; 
}

void parse_io_line(char *line)
{
	char *tokens[LINE_MAX];
	char *token = NULL;
    int token_count = 0;

	// Ignore first token, "Component:"
	token = strtok(line, DELIMITERS);
    while(token != NULL)
    {
        tokens[token_count++] = token;
		token = strtok(NULL, DELIMITERS);
        //token_count++;
    }

	if(!strcmp(tokens[2],"CCs:") && times_parsing == 2)
    {
        parse_io(tokens, token_count);
    }
}

void parse_io(char *tokens[], int token_count)
{
	char *io_name = NULL;
	//int i = 0;
	int j = 0;
	int k = 0;
	int flag_output = 0;

	io_name = strdup(tokens[1]);

	if(token_count > 2)
	{
		for(int i=3; i<token_count; i=i+2)
		{
			struct gatepins *connections = NULL;
			char *connections_names = NULL;
			
			connections_names = malloc(strlen(tokens[i]) + strlen(tokens[i+1]) + 1);
			connections_names = strcpy(connections_names, tokens[i]);
			connections_names = strcat(connections_names, tokens[i+1]);

			connections = find_gatepins(connections_names);
			if(connections == NULL)
			{
				connections = create_gatepins(connections_names, find_component(tokens[i]), NULL, NULL, NULL, 0);
			}
			else{
				if(connections->io == 1){
					flag_output = 1;
				}
			}
			
			create_io(io_name, connections,0);
			
			free(connections_names);
			j++;
		}
	}
	if(flag_output == 1)
	{
		create_io(io_name, NULL, 1);
	}
	else
	{
		create_io(io_name, NULL, 0);
	}

	free(io_name);

}



void parse_comp_line(char *line)
{
    char *tokens[LINE_MAX];
	char *token = NULL;
    int token_count = 0;

	// Ignore first token, "Component:"
	token = strtok(line, DELIMITERS);
    while(token != NULL)
    {
        tokens[token_count++] = token;
		token = strtok(NULL, DELIMITERS);
        //token_count++;
    }

	if(!strcmp(tokens[2],"Cell_Type:") && times_parsing == 0)
    {
        parse_comp1(tokens, token_count);
    }
    else if(!strcmp(tokens[2],"Output") && times_parsing == 0)
    {
        parse_comp2(tokens, token_count);
    }
	else if(!strcmp(tokens[2],"Cell_Type:") && times_parsing == 1)
	{
		parse_gatepins1(tokens, token_count);
	}
	else if(!strcmp(tokens[2],"Output") && times_parsing == 1)
    {
        parse_gatepins2(tokens, token_count);
    }
}

void parse_comp1(char *tokens[], int token_count)
{
    char *comp_name = NULL;
    char *lib_name = NULL;
    struct lib_hash *lib = NULL;

    comp_name = strdup(tokens[1]);
    lib_name = strdup(tokens[3]);

    lib = create_lib_hash(lib_name,NULL);
    create_component(comp_name,NULL,lib);
    
	free(comp_name);
	free(lib_name);
}

void parse_comp2(char *tokens[], int token_count)
{
    char *comp_name = NULL;
    struct component *component = NULL;
    struct lib_hash *lib = NULL;
    char *function = NULL;
    int i = 0;
	int length_function = 0;

	comp_name = malloc(sizeof(char) * strlen(tokens[1]) + 1 -1); // +1 for '\0' -1 for ','
    strncpy(comp_name,tokens[1], strlen(tokens[1]) - 1); // -1 for ','
	comp_name[strlen(tokens[1]) - 1] = '\0';
    component = find_component(comp_name);
    lib = component->lib_cell;


	for(i=7; i<token_count; i++)
	{
		length_function = length_function + strlen(tokens[i]);
	}
    function = malloc(length_function + 1);
    function = strcpy(function, tokens[7]);
    for(i=8; i<token_count; i++)
    {
        function = strcat(function, tokens[i]);
    }

    create_lib_hash(lib->lib_cell_name,NULL);

	free(comp_name);
	free(function);
}

void parse_gatepins1(char *tokens[], int token_count)
{
	char *gatepin_name = NULL;
	char *comp_name = NULL;
	char *lib_name = NULL;
	struct component *component = NULL;
	struct lib_hash *lib = NULL;
	struct gatepins *gatepin = NULL;
	//char** connections_names = NULL;
	int j = 0;

	comp_name = strdup(tokens[1]);
    lib_name = strdup(tokens[3]);
	
	component = find_component(comp_name);
	lib = find_lib_hash(lib_name);

	// allocate memory for the gatepin name //
	gatepin_name = malloc(strlen(tokens[11]) + strlen(tokens[12]) + 1);
	gatepin_name = strcpy(gatepin_name, tokens[11]);
	gatepin_name = strcat(gatepin_name, tokens[12]);

	gatepin = create_gatepins(gatepin_name, component, lib, NULL, NULL, 1);
	create_component(comp_name, gatepin, lib);

	for(int i=13; i<token_count; i=i+2)
	{
		struct gatepins *connections = NULL;
		char *connections_names = NULL;
		
		connections_names = malloc(strlen(tokens[i]) + strlen(tokens[i+1]) + 1);
		connections_names = strcpy(connections_names, tokens[i]);
		connections_names = strcat(connections_names, tokens[i+1]);

		connections = create_gatepins(connections_names, find_component(tokens[i]), NULL, NULL, NULL, 0);
		create_gatepins(gatepin_name, component, NULL, connections, NULL, 1);
		create_component(tokens[i], connections, lib);
		free(connections_names);
		j++;
	}


	free(gatepin_name);
	free(comp_name);
	free(lib_name);
	/* for(int i=0; i<(token_count - 13)/2; i++)
	{
		free(connections_names);
	}
	 */

}


void parse_gatepins2(char *tokens[], int token_count)
{
	char *gatepin_name = NULL;
	char *comp_name = NULL;
	char *lib_name = NULL;
	struct component *component = NULL;
	struct lib_hash *lib = NULL;
	//struct gatepins *gatepin = NULL;
	char *function = NULL;
	char* output_pin = NULL;
	char* output_pin_name = NULL;
	int i = 0;
	int length_function = 0;

	comp_name = malloc(sizeof(char) * strlen(tokens[1]) + 1 -1); // +1 for '\0' -1 for ','
	strncpy(comp_name,tokens[1], strlen(tokens[1]) - 1); // -1 for ','
	comp_name[strlen(tokens[1]) - 1] = '\0';
	component = find_component(comp_name);
	lib = component->lib_cell;

	output_pin = malloc(strlen(tokens[4]) -1 + 1); // -1 for the ',' +1 for \0 //
	strncpy(output_pin,tokens[4], strlen(tokens[4]) - 1);
	output_pin[strlen(tokens[4]) - 1] = '\0';
	output_pin_name = malloc(sizeof(char) * (strlen(output_pin) + 3) + 1);
	sprintf(output_pin_name, "(/%s)", output_pin);

	// allocate memory for the gatepin name //
	gatepin_name = malloc(strlen(comp_name) + strlen(output_pin_name) + 1);
	gatepin_name = strcpy(gatepin_name, comp_name);
	gatepin_name = strcat(gatepin_name, output_pin_name);

	for(i=7; i<token_count; i++)
	{
		length_function = length_function + strlen(tokens[i]);
	}
    function = malloc(length_function + 1);
    function = strcpy(function, tokens[7]);
    for(i=8; i<token_count; i++)
    {
        function = strcat(function, tokens[i]);
    }

	find_gatepins(gatepin_name);
	create_gatepins(gatepin_name, component, lib, NULL, function, 1);

	free(gatepin_name);
	free(comp_name);
	free(function);
	free(lib_name);
	free(output_pin);
	free(output_pin_name);
}

