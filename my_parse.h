#ifndef SEC_H
#define SEC_H

#define IS_BLANK 1
#define ISNOT_BLANK 0
#define MAX_COLUMNS 1000  // Adjust this based on the maximum number of columns expected
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

extern int component_count;

#endif