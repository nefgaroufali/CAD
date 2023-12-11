#ifndef MY_PARSE_H
#define MY_PARSE_H

#define DELIMITERS " \t\r\n"

#define ROW "Row:"
#define IO "IO:"
#define COMPONENT "Component:"

#define FAILURE           -2
#define IS_SYNTAX_ERRROR  -1
#define IS_BLANK_LINE      0
#define SUCCESS            1
#define IS_COMMENT         2
#define IS_ROW             3
#define IS_IO              4
#define IS_COMPONENT       5

#define NRM  "\x1B[0m"		// Normal Color
#define RED  "\x1B[31m"		// Red Color
#define GRN  "\x1B[32m"		// Green Color
#define YEL  "\x1B[33m"		// Yellow Color
#define BLU  "\x1B[34m"		// Blue Color
#define MAG  "\x1B[35m"		// Magenta Color

void count_components(FILE *fp);
void parse_design_file(FILE *fp);
int check_first_token(char *token);
void parse_comp_line(char *line);
void parse_io_line(char *line);
void parse_io(char *tokens[], int token_count);
void parse_comp1(char *tokens[], int token_count);
void parse_comp2(char *tokens[], int token_count);
void parse_gatepins1(char *tokens[], int token_count);
void parse_gatepins2(char *tokens[], int token_count);

#endif