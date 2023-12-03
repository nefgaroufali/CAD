#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "my_parse.h"
#include "my_structs.h"

/* 
#define IS_BLANK 1
#define ISNOT_BLANK 0
#define MAX_COLUMNS 100  // Adjust this based on the maximum number of columns expected
#define PARSING_ERROR -1
#define PARSING_SUCCESSFUL 1
#define IS_COMMENT 1
#define IS_NOT_COMMENT 0
#define IS_ROW 1
#define IS_NOT_ROW 0
#define IGNORE 1
#define NOT_IGNORE 0



void parse(char* filename);
int is_blank_line(const char* line);
int parse_line(char* line);
void number_of_lines(const char* filename);
int is_comment_line(const char* line);
int is_row_line(const char* line);

int component_count = 0; // count of lines starting with "Component"
 */

int component_count;

// This function counts the lines of the file that start with word "Component" //
// This is used to find the size of the hash table //
void number_of_lines(const char* filename) 
{
    FILE* input_file;
    char line[LINE_MAX]; // line buffer to store each line of the file

    // open the file for reading //
    input_file = fopen(filename, "r");
    if (input_file == NULL)
    {
        perror("Error opening file");
        exit(1);
    }

    // read each line of the file //
    while (fgets(line, sizeof(line), input_file)) 
    {

        // skip blank lines //
        if (is_blank_line(line) == IS_BLANK) 
        {
            continue;
        }

        // check if line starts with "Component" //
        if (strncmp(line, "Component", strlen("Component")) == 0) 
        {
            component_count++;
        }
    }

    component_count = component_count / 2;

    fclose(input_file);

    return;
}

void parse(char* filename)
{
    FILE* input_file;
    char line[LINE_MAX]; // line buffer to store each line of the file
    int parse_line_result; // result of parse_line function

    number_of_lines(filename);
    create_comp_hash_table(component_count);
    create_lib_hash_table(component_count);
    create_gatepins_hash_table(component_count);

    // open the file for reading
    input_file = fopen(filename, "r");
    if (input_file == NULL)
    {
        perror("Error opening file");
        exit(1);
    }

    // read each line of the file
    while (fgets(line, sizeof(line), input_file))
    {

        // skip blank lines
        if (is_blank_line(line) == IS_BLANK || is_comment_line(line) == IS_COMMENT || is_row_line(line) == IS_ROW)
        {
            continue;
        }

        // parse the line
        parse_line_result = parse_line(line);
        
        if (parse_line_result == PARSING_ERROR)
        {
            fprintf(stderr, "Error parsing line: %s\n", line);
            exit(1);
        }
    }

    printf("Number of components: %d\n", component_count);

    fclose(input_file);
}

// This function checks if the current line is blank //
int is_blank_line(const char* line)
{
    int i = 0;

    // Until the end of the line, check if there is a character //
    // that is not a space, tab or newline //
    while (line[i] != '\0')
    {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
        {
            return ISNOT_BLANK;
        }
        i++;
    }
    return IS_BLANK;
}

// This function checks if this line is about Rows that I handle as comments //
int is_row_line(const char* line)
{
    if (line[0] == 'R')
    {
        return IS_ROW;
    }
    return IS_NOT_ROW;
}

// This function checks if this line is a comment //
int is_comment_line(const char* line)
{
    if (line[0] == '#')
    {
        return IS_COMMENT;
    }
    return IS_NOT_COMMENT;
}

// This function parses the line //
int parse_line(char* line)
{
    char* tokens[MAX_COLUMNS];
    int column_count = 0;
    char* token;

    // tokenise the line, based on space, tab or newline //
    // store the tokens in an array //
    token = strtok(line, " \t\n\r");
    while (token != NULL && column_count < MAX_COLUMNS - 1)
    {
        tokens[column_count++] = token;
        token = strtok(NULL, " \t\n\r");
    }
    tokens[column_count] = NULL;

    // process tokens here
    // Here will be the calls for the data structures //
    // to store the useful information of the file //
    if(strcmp(tokens[0],"IO:") == 0)
    {
        return PARSING_SUCCESSFUL;
    }
    else if(strcmp(tokens[2],"Cell_Type:") == 0)
    {
        make_data_structures(tokens, column_count,0);
    }
    else if(strcmp(tokens[2],"Output") == 0)
    {
        make_data_structures(tokens, column_count,1);
    }

    //printf("Token %d: %s\n", i, tokens[i]);

    return 0;
}



int main(int argc, char* argv[])
{
    parse(argv[1]);
    printf("\n\nPrinting the components hash table\n\n");
    print_comp_hash_table(&comp_hash_table);
    printf("\n\nPrinting the library hash table\n\n");
    print_lib_hash_table(&lib_hash_table);
    printf("\n\nPrinting the gatepins hash table\n\n");
    print_gatepins_hash_table(&gatepins_hash_table);
    return 0;
}


















/* 


int main() {
    FILE *file = fopen("your_file.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[256];  // Adjust the buffer size according to your needs
    while (fgets(line, sizeof(line), file)) {
        char *tokens[MAX_COLUMNS];
        int column_count = 0;

        char *token = strtok(line, " \t\n"); // Tokenize based on space, tab, or newline
        while (token != NULL && column_count < MAX_COLUMNS - 1) {
            tokens[column_count++] = token;
            token = strtok(NULL, " \t\n");
        }
        tokens[column_count] = NULL;  // Null-terminate the array

        // Process tokens here
        for (int i = 0; i < column_count; ++i) {
            printf("Token %d: %s\n", i, tokens[i]);
        }
    }

    fclose(file);
    return 0;
}
 */