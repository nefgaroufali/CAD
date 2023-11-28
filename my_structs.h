#ifndef MY_STRUCTS_H
#define MY_STRUCTS_H

#define HASH_DEPTH 10000
#define HASH_SIZE 10000
#define MAX_VARIABLES 10000
#define MAX_VAR_LENGTH 20000

// thestructure of the component //
struct component
{
    char *comp_name;
    struct gatepins** gatepins_array;
    struct gatepins** connections;
    struct lib_hash* lib_cell;
    struct component* next;
};

// the structure of the gatepins //
struct gatepins
{
    char *gatepin_name;
    struct component* component;
    struct lib_hash* lib_cell;
    int io; // 0 for input, 1 for output
    struct gatepins* next;
};

// the structure of the library cells //
struct lib_hash
{
    char *lib_cell_name;
    char *function; 
    char **pins;
    struct lib_hash* next;
};

// the structure of general IOs //
struct io
{
    char *io_name;
    int io; // 0 for input, 1 for output
    struct gatepins** connections;
};

// the structure of the hash table for components //
struct comp_hash_table
{
    struct component** table;
    int size;
};

// the structure of the hash table for gatepins //
struct gatepins_hash_table
{
    struct gatepins** table;
    int size;
};

// the structure of the hash table for library cells //
struct lib_hash_table
{
    struct lib_hash** table;
    int size;
};

extern struct comp_hash_table comp_hash_table;
extern struct gatepins_hash_table gatepins_hash_table;
extern struct lib_hash_table lib_hash_table;

// Function declarations //
struct component* create_component(char* comp_name, struct gatepins** gatepins_array, struct gatepins** connections, struct lib_hash* lib_cell);
void create_comp_hash_table(int size);
void insert_component(struct component* comp);
struct component* find_component(char* comp_name);
void print_comp_hash_table(struct comp_hash_table* ht);
void free_comp_hash_table(struct comp_hash_table* ht);

struct gatepins* create_gatepins(char* gatepin_name, struct component* component, struct lib_hash* lib_cell, int io);
void create_gatepins_hash_table(int size);
void insert_gatepins(struct gatepins* gatepin);
struct gatepins* find_gatepins(char* gatepin_name);
void print_gatepins_hash_table(struct gatepins_hash_table* ht);
void free_gatepins_hash_table(struct gatepins_hash_table* ht);

struct lib_hash* create_lib_hash(char* lib_cell_name, char* function, char** pins);
void create_lib_hash_table(int size);
void insert_lib_hash(struct lib_hash* lib_cell);
struct lib_hash* find_lib_hash(char* lib_cell_name);
void print_lib_hash_table(struct lib_hash_table* ht);
void free_lib_hash_table(struct lib_hash_table* ht);

int make_data_structures(char** tokens, int count, int flag);
char**  extractVariables(const char* function);
int isVariableChar(char c);

#endif