#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#include "my_parse.h"
#include "my_structs.h"

int component_count = 0;
int times_parsing = 0;


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
	create_comp_hash_table(component_count);
	create_lib_hash_table(component_count);
	create_gatepins_hash_table(component_count);
	rewind(fp);

	for(times_parsing = 0; times_parsing < 2; times_parsing++)
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
					//parse_io_line(line);
					continue;
				case IS_COMPONENT:
					parse_comp_line(line);
					continue;
			}
		}
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
/* 
void parse_io_line(char *line)
{
	const char location[] = "Location:";
	const char ccs[] = "CCs:";

	char *token = NULL;
	char *name = NULL;

	double x = 0;
	double y = 0;

	// Ignore first token, "IO:"
	token = strtok(line, DELIMITERS);

	token = strtok(NULL, DELIMITERS);
	if (token == NULL)
	{
		printf(RED"Syntax Error!\n"NRM);
		exit(1);
	}
	name = strdup(token);

	token = strtok(NULL, DELIMITERS);
	if (token == NULL)
	{
		printf(RED"Syntax Error!\n"NRM);
		exit(1);
	}

	// If token is location, the its the initial definition of the IO pin
	if (!strcmp(token, location))
	{
		token = strtok(NULL, DELIMITERS);
		if (token == NULL)
		{
			printf(RED"Syntax Error!\n"NRM);
			exit(1);
		}
		x = atof(token);

		token = strtok(NULL, DELIMITERS);
		if (token == NULL)
		{
			printf(RED"Syntax Error!\n"NRM);
			exit(1);
		}
		y = atof(token);
		
		token = strtok(NULL, DELIMITERS);
		if (token != NULL)
		{
			printf(RED"Syntax Error!\n"NRM);
			exit(1);
		}

		insert_io(name, x, y);
		return;
	}

	// If token is ccs, then its the connections of the IO pin
	// If not its syntax error
	if (strcmp(token, ccs))
	{
		printf(RED"Syntax Error!\n"NRM);
		exit(1);
	}

	//  Check for connections
	token = strtok(NULL, DELIMITERS);
	if (token == NULL)
		return;

	insert_io_net(strdup(name), strdup(token));

	token = strtok(NULL, DELIMITERS);
	
	// It is not a must to have a connection
	while (token != NULL)
	{
		insert_io_net(strdup(name), strdup(token));
		token = strtok(NULL, DELIMITERS);
	}
}
 */


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
	/* else if(!strcmp(tokens[2],"Cell_Type:") && times_parsing == 1)
	{
		parse_gatepins(tokens, token_count);
	} */
}

void parse_comp1(char *tokens[], int token_count)
{
    char *comp_name = NULL;
    char *lib_name = NULL;
    struct lib_hash *lib = NULL;

    comp_name = strdup(tokens[1]);
    lib_name = strdup(tokens[3]);

    lib = create_lib_hash(lib_name,NULL,NULL);
    create_component(comp_name,NULL,NULL, lib);
    
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

    create_lib_hash(lib->lib_cell_name, function, NULL);

	free(comp_name);
	free(function);
}

/* void parse_gatepins(char *tokens[], int token_count)
{
	
} */

/* 
int main(int argc, char *argv[])
{
	FILE *fp = NULL;

	if (argc != 2)
	{
		printf(RED"Usage: %s <design_file>\n"NRM, argv[0]);
		exit(1);
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		printf(RED"Error: %s\n"NRM, strerror(errno));
		exit(1);
	}

	parse_design_file(fp);
	print_comp_hash_table(&comp_hash_table);
	print_lib_hash_table(&lib_hash_table);

	return 0;
} */