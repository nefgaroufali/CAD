#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "my_structs.h"
#include "my_parse.h"

#define MAX_NAME_LENGTH 256
#define MAX_ARRAY_SIZE 1000
#define MAX_VAR_LENGTH 256
#define MAX_VARIABLES 256

struct comp_hash_table comp_hash_table  = {NULL, 0};
struct gatepins_hash_table gatepins_hash_table = {NULL, 0};
struct lib_hash_table lib_hash_table = {NULL, 0};

// Function definitions //

// djb2 hash function
unsigned long hash(char *str) {
    int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return (hash);
}

//********************************************************************
//
// Hash table functions
//
//********************************************************************

// This function creates the hash table for components //
void create_comp_hash_table(int size)
{
    comp_hash_table.table = malloc(sizeof(struct component*) * size);
    comp_hash_table.size = size;

    // Initialize the table with NULL
    for(int i = 0; i < size; i++)
    {
        comp_hash_table.table[i] = NULL;
    }
}

// This function creates a gatepins hash table //
void create_gatepins_hash_table(int size)
{
    gatepins_hash_table.table = malloc(sizeof(struct gatepins*) * size);
    gatepins_hash_table.size = size;

    // Initialize the table with NULL
    for(int i = 0; i < size; i++)
    {
        gatepins_hash_table.table[i] = NULL;
    }
}

// This function creates a library cell hash table //
void create_lib_hash_table(int size)
{
    lib_hash_table.table = malloc(sizeof(struct lib_hash*) * size);
    lib_hash_table.size = size;

    // Initialize the table with NULL
    for(int i = 0; i < size; i++)
    {
        lib_hash_table.table[i] = NULL;
    }
}



//********************************************************************
//
// Search functions
//
//********************************************************************

// This function finds if a component exists in the hash table //
struct component* find_component(char* comp_name)
{
    unsigned long hash_value;
    struct component* comp;

    hash_value = hash(comp_name) % comp_hash_table.size;
    comp = comp_hash_table.table[hash_value];
    
    while(comp != NULL)
    {
        if(!strcmp(comp->comp_name, comp_name))
        {
            return comp;
        }
        comp = comp->next;
    }
    return NULL;
}

// This function finds if a gatepin exists in the hash table //
struct gatepins* find_gatepins(char* gatepin_name)
{
    unsigned long hash_value;
    struct gatepins* gatepin;

    hash_value = hash(gatepin_name) % gatepins_hash_table.size;
    gatepin = gatepins_hash_table.table[hash_value];
    
    while(gatepin != NULL)
    {
        if(!strcmp(gatepin->gatepin_name, gatepin_name))
        {
            return gatepin;
        }
        gatepin = gatepin->next;
    }
    return NULL;
}

// This function finds if library cell exists in the hash table //
struct lib_hash* find_lib_hash(char* lib_cell_name)
{
    unsigned long hash_value;
    struct lib_hash* lib_cell;

    hash_value = hash(lib_cell_name) % lib_hash_table.size;
    lib_cell = lib_hash_table.table[hash_value];
    
    while(lib_cell != NULL)
    {
        if(!strcmp(lib_cell->lib_cell_name, lib_cell_name))
        {
            return lib_cell;
        }
        lib_cell = lib_cell->next;
    }
    return NULL;
}

struct io* find_io(char* io_name)
{
    int i = 0;
    while(io_array[i].io_name != NULL)
    {
        if(!strcmp(io_array[i].io_name, io_name))
        {
            return &io_array[i];
        }
        i++;
    }
    return NULL;
}

//********************************************************************
//
// Create cell functions
//
//********************************************************************

// This function creates a component //
struct component* create_component(char* comp_name, struct gatepins* gatepins, struct lib_hash* lib_cell)
{
    // variables //
    struct component* comp;
    int i=0;

    // search if the component already exists in the hash table //
    comp = find_component(comp_name);

    // if the component exists, return it //
    if(comp != NULL)
    {
        // if gatepins is not NULL, add it to the array //
        if(gatepins != NULL)
        {
            while(comp->gatepins_array[i] != NULL)
            {
                i++;
            }
            comp->gatepins_array[i] = gatepins;
        }

        return comp;
    }

    // if the component does not exist, create it //
    comp = malloc(sizeof(struct component));

    // allocate memory for the component name //
    comp->comp_name = malloc(strlen(comp_name) + 1);

    // copy the component name //
    strcpy(comp->comp_name, comp_name);

    // allocate memory for the gatepins //
    comp->gatepins_array = malloc(sizeof(struct gatepins*) * MAX_ARRAY_SIZE);
    for(int i = 0; i < MAX_ARRAY_SIZE; i++)
    {
        comp->gatepins_array[i] = NULL;
    }

    // assign the lib_cell pointer directly //
    comp->lib_cell = malloc(sizeof(struct lib_hash));
    comp->lib_cell = lib_cell;

    comp->next = NULL;

    insert_component(comp);

    return comp;
}


// This function creates a gatepin //
struct gatepins* create_gatepins(char* gatepin_name, struct component* component, struct lib_hash* lib_cell, struct gatepins* connections, char* function, int io)
{
    int i=0;

    // variables //
    struct gatepins* gatepin;

    // search if the gatepin already exists in the hash table //
    gatepin = find_gatepins(gatepin_name);

    // if the gatepin exists, return it //
    if(gatepin != NULL)
    {   
        if(component != NULL)
        {
            gatepin->component = component;
        }
        if(lib_cell != NULL)
        {
            gatepin->lib_cell = lib_cell;
        }
        if(function != NULL)
        {
            gatepin->function = malloc(strlen(function) + 1);
            strcpy(gatepin->function, function);
        }
        else
        {
            gatepin->function = NULL;
        }

        i = 0;
        if(connections != NULL)
        {
            while(gatepin->connections[i] != NULL)
            {
                i++;
            }
            gatepin->connections[i] = connections;
        }
        gatepin->io = io;

        return gatepin;
    }

    // if the gatepin does not exist, create it //
    gatepin = malloc(sizeof(struct gatepins));

    // allocate memory for the gatepin name //
    gatepin->gatepin_name = malloc(strlen(gatepin_name) + 1);
    strcpy(gatepin->gatepin_name, gatepin_name);

    // assign the component and lib_cell pointers directly //
    gatepin->component = malloc(sizeof(struct component));
    if(component != NULL)
    {
        gatepin->component = component;
    }
    else
    {
        gatepin->component = NULL;
    }
    gatepin->lib_cell = malloc(sizeof(struct lib_hash));
    if(lib_cell != NULL)
    {
        gatepin->lib_cell = lib_cell;
    }
    else
    {
        gatepin->lib_cell = NULL;
    }

    // allocate memory for the connections //
    gatepin->connections = malloc(sizeof(struct gatepins*) * MAX_ARRAY_SIZE);
    for(int i = 0; i < MAX_ARRAY_SIZE; i++)
    {
        gatepin->connections[i] = NULL;
    }
    gatepin->function = NULL;

    // copy the io //
    gatepin->io = io;

    gatepin->next = NULL;

    insert_gatepins(gatepin);
    return gatepin;
}


// This function creates a library cell //
struct lib_hash* create_lib_hash(char* lib_cell_name, char** pins)
{
    // variables //
    int i = 0;
    struct lib_hash* lib_cell;

    // search if the library cell already exists in the hash table //
    lib_cell = find_lib_hash(lib_cell_name);

    // if the library cell exists, return it //
    if(lib_cell != NULL)
    {

        // allocate memory for the pins //
        if(pins != NULL)
        {
            lib_cell->pins = malloc(sizeof(char*) * MAX_ARRAY_SIZE);
            // copy the pins //
            for(i = 0; i < MAX_ARRAY_SIZE; i++)
            {
                lib_cell->pins[i] = malloc(strlen(pins[i]) + 1);
                strcpy(lib_cell->pins[i], pins[i]);
            }
        }
        return lib_cell;
    }

    // if the library cell does not exist, create it //
    lib_cell = malloc(sizeof(struct lib_hash));
    lib_cell->lib_cell_name = malloc(strlen(lib_cell_name) + 1);
    strcpy(lib_cell->lib_cell_name, lib_cell_name);
    

    // allocate memory for the pins //
    if(pins != NULL)
    {
        lib_cell->pins = malloc(sizeof(char*) * MAX_ARRAY_SIZE);
        // copy the pins //
        for(i = 0; i < MAX_ARRAY_SIZE; i++)
        {
            lib_cell->pins[i] = malloc(strlen(pins[i]) + 1);
            strcpy(lib_cell->pins[i], pins[i]);
        }
    }
    else
    {
        lib_cell->pins = NULL;
    }

    lib_cell->next = NULL;

    insert_lib_hash(lib_cell);

    return lib_cell;
}


struct io* create_io(char* io_name, struct gatepins* connections, int io)
{
    int i = 0;
    struct io* io_ptr;

    io_ptr = find_io(io_name);

    i = 0;
    if(io_ptr != NULL)
    {
        if(connections != NULL)
        {
            while(io_ptr->connections[i] != NULL)
            {
                i++;
            }
            io_ptr->connections[i] = connections;
        }

        io_ptr->io = io;
        return io_ptr;
    }

    io_ptr = malloc(sizeof(struct io));
    io_ptr->io_name = malloc(strlen(io_name) + 1);
    strcpy(io_ptr->io_name, io_name);

    io_ptr->connections = malloc(sizeof(struct gatepins*) * IO_ARRAY_SIZE);
    for(int i = 0; i < IO_ARRAY_SIZE; i++)
    {
        io_ptr->connections[i] = NULL;
    }

    i=0;
    if(connections != NULL)
    {
        while(io_ptr->connections[i] != NULL)
        {
            i++;
        }
        io_ptr->connections[i] = connections;
    }

    io_ptr->io = io;

    //io_ptr->next = NULL;

    insert_io(io_ptr);

    return io_ptr;
}

//********************************************************************
//
// Insert cell functions
//
//********************************************************************


// This function inserts a component to the hash table //
void insert_component(struct component* comp)
{
    unsigned long hash_value;
    struct component* comp_ptr;

    hash_value = hash(comp->comp_name) % comp_hash_table.size;
    comp_ptr = comp_hash_table.table[hash_value];

    if(comp_ptr == NULL)
    {
        comp_hash_table.table[hash_value] = comp;
    }
    else
    {
        while(comp_ptr->next != NULL)
        {
            comp_ptr = comp_ptr->next;
        }
        comp_ptr->next = comp;
    }
}

// This function inserts a gatepin to the hash table //
void insert_gatepins(struct gatepins* gatepin)
{
    unsigned long hash_value;
    struct gatepins* gatepin_ptr;

    hash_value = hash(gatepin->gatepin_name) % gatepins_hash_table.size;
    gatepin_ptr = gatepins_hash_table.table[hash_value];

    if(gatepin_ptr == NULL)
    {
        gatepins_hash_table.table[hash_value] = gatepin;
    }
    else
    {
        while(gatepin_ptr->next != NULL)
        {
            gatepin_ptr = gatepin_ptr->next;
        }
        gatepin_ptr->next = gatepin;
    }
}

// This function inserts a library cell to the hash table //
void insert_lib_hash(struct lib_hash* lib_cell)
{
    unsigned long hash_value;
    struct lib_hash* lib_cell_ptr;

    hash_value = hash(lib_cell->lib_cell_name) % lib_hash_table.size;
    lib_cell_ptr = lib_hash_table.table[hash_value];

    if(lib_cell_ptr == NULL)
    {
        lib_hash_table.table[hash_value] = lib_cell;
    }
    else
    {
        while(lib_cell_ptr->next != NULL)
        {
            lib_cell_ptr = lib_cell_ptr->next;
        }
        lib_cell_ptr->next = lib_cell;
    }
}

void insert_io(struct io* io_ptr)
{
    int i = 0;
    while(io_array[i].io_name != NULL)
    {
        i++;
    }
    io_array[i] = *io_ptr;
}


//********************************************************************
//
// Print functions
//
//********************************************************************

// This function prints all the components in the hash table //
void print_comp_hash_table(struct comp_hash_table* ht)
{
    struct component* comp;

    for(int i = 0; i < ht->size; i++)
    {
        comp = ht->table[i];
        while(comp != NULL)
        {
            printf("Component name: %s\n", comp->comp_name);
            printf("Component gatepins: ");
            for(int j = 0; j < MAX_ARRAY_SIZE; j++)
            {
                if(comp->gatepins_array[j] != NULL)
                {
                    printf("%s ", comp->gatepins_array[j]->gatepin_name);
                }
            }
            printf("\n");
            printf("Component lib cell: %s\n", comp->lib_cell->lib_cell_name);
            printf("\n");
            comp = comp->next;
        }
    }
}

// This function prints all the gatepins in the hash table //
void print_gatepins_hash_table(struct gatepins_hash_table* ht)
{
    struct gatepins* gatepin;

    for(int i = 0; i < ht->size; i++)
    {
        gatepin = ht->table[i];
        while(gatepin != NULL)
        {
            printf("\nGatepin name: %s\n", gatepin->gatepin_name);
            if(gatepin->component != NULL)
            {
                printf("Gatepin component: %s\n", gatepin->component->comp_name);
            }
            else
            {
                printf("Gatepin component: NULL\n");
            }
            if(gatepin->lib_cell != NULL)
            {
                printf("Gatepin lib cell: %s\n", gatepin->lib_cell->lib_cell_name);
            }
            else
            {
                printf("Gatepin lib cell: NULL\n");
            }
            printf("Gatepin connections: ");
            for(int j = 0; j < MAX_ARRAY_SIZE; j++)
            {
                if(gatepin->connections[j] != NULL)
                {
                    printf("%s ", gatepin->connections[j]->gatepin_name);
                }
            }
            printf("\n");
            if(gatepin->function != NULL)
            {
                printf("Gatepin function: %s\n", gatepin->function);
            }
            else
            {
                printf("Gatepin function: NULL\n");
            }
            printf("Gatepin io: %d\n", gatepin->io);
            printf("\n");
            gatepin = gatepin->next;
        }
    }
}

// This function prints all the library cells in the hash table //
void print_lib_hash_table(struct lib_hash_table* ht)
{
    struct lib_hash* lib_cell;

    for(int i = 0; i < ht->size; i++)
    {
        lib_cell = ht->table[i];
        while(lib_cell != NULL)
        {
            printf("Library cell name: %s\n", lib_cell->lib_cell_name);
            //printf("Library cell function: %s\n", lib_cell->function);

            if(lib_cell->pins != NULL)
            {
                printf("Library cell pins: ");
                for(int j = 0; j < MAX_ARRAY_SIZE; j++)
                {
                    if(lib_cell->pins[j] != NULL)
                    {
                        printf("%s ", lib_cell->pins[j]);
                    }
                }
                printf("\n");
            }
            lib_cell = lib_cell->next;
        }
    }
}

void print_io_array(struct io* io_array)
{
    int i = 0;
    while(io_array[i].io_name != NULL)
    {
        printf("IO name: %s\n", io_array[i].io_name);
        printf("IO connections: ");
        for(int j = 0; j < IO_ARRAY_SIZE; j++)
        {
            if(io_array[i].connections[j] != NULL)
            {
                printf("%s ", io_array[i].connections[j]->gatepin_name);
            }
        }
        printf("\n");
        printf("IO io: %d\n", io_array[i].io);
        printf("\n");
        i++;
    }
}


//********************************************************************
//
// Free functions
//
//********************************************************************

// This function frees the memory allocated for the hash table //
void free_comp_hash_table(struct comp_hash_table* ht)
{
    struct component* comp;
    struct component* temp;

    for(int i = 0; i < ht->size; i++)
    {
        comp = ht->table[i];
        while(comp != NULL)
        {
            temp = comp;
            comp = comp->next;
            free(temp->comp_name);
            free(temp->gatepins_array);
            //free(temp->lib_cell);
            free(temp);
        }
    }
    free(ht->table);
}


// This function frees the memory allocated for the hash table //
void free_gatepins_hash_table(struct gatepins_hash_table* ht)
{
    struct gatepins* gatepin;
    struct gatepins* temp;

    for(int i = 0; i < ht->size; i++)
    {
        gatepin = ht->table[i];
        while(gatepin != NULL)
        {
            temp = gatepin;
            gatepin = gatepin->next;
            //free(temp->gatepin_name);
            //free(temp->component);
            //free(temp->lib_cell);
            //free(temp->connections);
            free(temp->function);
            free(temp);
        }
    }
    free(ht->table);
}


// This function frees the memory allocated for the hash table //
void free_lib_hash_table(struct lib_hash_table* ht)
{
    struct lib_hash* lib_cell;
    struct lib_hash* temp;

    for(int i = 0; i < ht->size; i++)
    {
        lib_cell = ht->table[i];
        while(lib_cell != NULL)
        {
            temp = lib_cell;
            lib_cell = lib_cell->next;
            free(temp->lib_cell_name);
            free(temp->pins);
            free(temp);
        }
    }
    free(ht->table);
}

void free_io_array(struct io* io_array)
{
    int i = 0;
    while(io_array[i].io_name != NULL)
    {
        free(io_array[i].io_name);
        free(io_array[i].connections);
        i++;
    }
}


//********************************************************************
//
// Extra functions
//
//************************************************************



char** extractVariables(const char* function) {
    char** variables = (char**)malloc(MAX_VARIABLES * sizeof(char*));
    if (variables == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    int varCount = 0;
    int functionLen = strlen(function);

    for (int i = 0; i < functionLen; i++) {
        if (isalpha(function[i]) || function[i] == '_') {
            int j = i;
            char var[MAX_VAR_LENGTH] = {'\0'};
            int varLength = 0;

            while (isVariableChar(function[j]) && varLength < MAX_VAR_LENGTH - 1) {
                var[varLength++] = function[j++];
            }

            var[varLength] = '\0';
            i = j - 1;

            variables[varCount] = (char*)malloc((varLength + 1) * sizeof(char));
            if (variables[varCount] == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }

            strcpy(variables[varCount], var);
            varCount++;

            if (varCount >= MAX_VARIABLES) {
                break;
            }
        }
    }

    variables[varCount] = NULL; // Null-terminate the array
    return variables;
}

int isVariableChar(char c) {
    return (isalnum(c) || c == '_');
}