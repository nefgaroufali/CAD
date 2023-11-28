#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "my_structs.h"
#include "my_parse.h"

/* 
#define HASH_DEPTH 10
#define HASH_SIZE 100

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
 */

struct comp_hash_table comp_hash_table;
struct gatepins_hash_table gatepins_hash_table;
struct lib_hash_table lib_hash_table;

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

// This function creates the hash table for components //
void create_comp_hash_table(int size)
{
    comp_hash_table.table = malloc(sizeof(struct component*) * size);
    comp_hash_table.size = size;
    if(comp_hash_table.table == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the table with NULL
    for(int i = 0; i < size; i++)
    {
        comp_hash_table.table[i] = NULL;
    }
}


// This function finds if a component exists in the hash table //
struct component* find_component(char* comp_name)
{
    unsigned long hash_value;
    struct component* comp;

    hash_value = hash(comp_name) % comp_hash_table.size;
    comp = comp_hash_table.table[hash_value];
    
    while(comp != NULL)
    {
        if(strcmp(comp->comp_name, comp_name) == 0)
        {
            return comp;
        }
        comp = comp->next;
    }
    return NULL;
}

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
            printf("Component connections: ");
            for(int j = 0; j < HASH_DEPTH; j++)
            {
                if(comp->connections[j] != NULL)
                {
                    printf("%s ", comp->connections[j]->gatepin_name);
                }
            }
            printf("\n");
            printf("Component gatepins: ");
            for(int j = 0; j < HASH_DEPTH; j++)
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
            free(temp->connections);
            free(temp);
        }
    }
    free(ht->table);
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
        if(strcmp(lib_cell->lib_cell_name, lib_cell_name) == 0)
        {
            return lib_cell;
        }
        lib_cell = lib_cell->next;
    }
    return NULL;
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
            printf("Library cell function: %s\n", lib_cell->function);
            printf("Library cell pins: ");
            for(int j = 0; j < HASH_DEPTH; j++)
            {
                if(lib_cell->pins[j] != NULL)
                {
                    printf("%s ", lib_cell->pins[j]);
                }
            }
            printf("\n");
            lib_cell = lib_cell->next;
        }
    }
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
            free(temp->function);
            free(temp->pins);
            free(temp);
        }
    }
    free(ht->table);
}

// This function creates a library cell hash table //
void create_lib_hash_table(int size)
{
    lib_hash_table.table = malloc(sizeof(struct lib_hash*) * size);
    lib_hash_table.size = size;
    if(lib_hash_table.table == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the table with NULL
    for(int i = 0; i < size; i++)
    {
        lib_hash_table.table[i] = NULL;
    }
}

// This function creates a gatepins hash table //
void create_gatepins_hash_table(int size)
{
    gatepins_hash_table.table = malloc(sizeof(struct gatepins*) * size);
    gatepins_hash_table.size = size;
    if(gatepins_hash_table.table == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the table with NULL
    for(int i = 0; i < size; i++)
    {
        gatepins_hash_table.table[i] = NULL;
    }
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
        if(strcmp(gatepin->gatepin_name, gatepin_name) == 0)
        {
            return gatepin;
        }
        gatepin = gatepin->next;
    }
    return NULL;
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

// This function prints all the gatepins in the hash table //
void print_gatepins_hash_table(struct gatepins_hash_table* ht)
{
    struct gatepins* gatepin;

    for(int i = 0; i < ht->size; i++)
    {
        gatepin = ht->table[i];
        while(gatepin != NULL)
        {
            printf("Gatepin name: %s\n", gatepin->gatepin_name);
            printf("Gatepin component: %s\n", gatepin->component->comp_name);
            printf("Gatepin lib cell: %s\n", gatepin->lib_cell->lib_cell_name);
            printf("Gatepin io: %d\n", gatepin->io);
            printf("\n");
            gatepin = gatepin->next;
        }
    }
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
            free(temp->gatepin_name);
            free(temp);
        }
    }
    free(ht->table);
}



int make_data_structures(char** tokens, int count, int flag)
{
    char* comp_name;
    char* lib_cell_name;
    char* gatepin_name;
    char* output_pin_name;
    char* function;
    char** pins = NULL; 
    char** connections_names = NULL;
    char** input_gatepins_names = NULL;
    struct gatepins** gatepins_array = NULL;
    struct gatepins** connections = NULL;
    struct lib_hash* lib_cell;
    struct component* comp;
    int i=13;
    int j=0;


    if(flag == 0)
    {
        // allocate memory for the component name //
        comp_name = malloc(strlen(tokens[1]) + 1);
        if(comp_name == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(comp_name, tokens[1]);

        // allocate memory for the library cell name //
        lib_cell_name = malloc(strlen(tokens[3]) + 1);
        if(lib_cell_name == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(lib_cell_name, tokens[3]);
        
        // create the library cell // 
        lib_cell = create_lib_hash(lib_cell_name,NULL,NULL);

        // allocate memory for the gatepin name //
        gatepin_name = malloc(strlen(tokens[11]) + strlen(tokens[12]) + 1);
        if(gatepin_name == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        gatepin_name = strcpy(gatepin_name, tokens[11]);
        gatepin_name = strcat(gatepin_name, tokens[12]);

        // allocate memory for the gatepins array - This is the array for the component //
        gatepins_array = malloc(sizeof(struct gatepins*) * HASH_DEPTH);
        if(gatepins_array == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }

        // allocate memory for the connections array - This is the array for the component //
        connections = malloc(sizeof(struct gatepins*) * HASH_DEPTH);
        if(connections == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        
        // allocate memory for the connections names array - This is a temp array to save the names of the connections //
        connections_names = malloc(sizeof(char*) * HASH_DEPTH);
        if(connections_names == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }

        // create the gatepin output and store it to the first position of the gatepins array //
        gatepins_array[0] = create_gatepins(gatepin_name,NULL,NULL,1);
        gatepins_array[0]->lib_cell = lib_cell; // the lib cell of the output gatepin is the lib cell of the component //

        // concat the connections names and create the connections array //
        for(i=13; i<count; i=i+2)
        {   
            connections_names[j] = malloc(strlen(tokens[i]) + strlen(tokens[i+1]) + 1);
            connections_names[j] = strcpy(gatepin_name, tokens[i]);
            connections_names[j] = strcat(gatepin_name, tokens[i+1]);

            connections[j] = create_gatepins(connections_names[j],NULL,NULL,0);
            j++;
        }

        // create the component having almost all the necessary information //
        comp = create_component(comp_name,gatepins_array,connections,lib_cell);
        gatepins_array[0]->component = comp; // the component of the output gatepin is the component itself //

        return 1;
    }

    else if(flag == 1)
    {
        // allocate memory for the component name //
        comp_name = malloc(strlen(tokens[1]) -1 + 1); // -1 for the ',' +1 for \0 //
        if(comp_name == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        strncpy(comp_name, tokens[1], strlen(tokens[1]) -1); // -1 for the ',' //
        
        // the component exists, so find it and return it //
        comp = find_component(comp_name);
        if(comp == NULL)
        {
            printf("Error! Component %s does not exist.\n", comp_name);
            exit(EXIT_FAILURE);
        }

        // allocate memory for the function name //
        function = malloc(strlen(tokens[7]) + 1);
        function = strcpy(function, tokens[7]);

        // allocate memory for the pins array //
        pins = malloc(sizeof(char*) * HASH_DEPTH);
        if(pins == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }

        // lib cell exists so find it and return it //
        lib_cell = find_lib_hash(comp->lib_cell->lib_cell_name);
        lib_cell->function = malloc(strlen(function) + 1);
        if(lib_cell->function == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(lib_cell->function, function);

        // create the pins array //
        lib_cell->pins = malloc(sizeof(char*) * HASH_DEPTH);
        if(lib_cell->pins == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }

        pins = extractVariables(function);

        j = 0;

        while(pins[j] != NULL)
        {
            lib_cell->pins[j] = malloc(strlen(pins[j]) + 3 + 1); // 3 for (/) and 1 for \0 //
            if(lib_cell->pins[j] == NULL)
            {
                printf("Error! Not enough memory.\n");
                exit(EXIT_FAILURE);
            }
            sprintf(lib_cell->pins[j], "(/%s)", pins[j]);
            j++;
        }

        output_pin_name = malloc(strlen(tokens[4]) -1 + 1); // -1 for the ',' +1 for \0 //
        if(output_pin_name == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        strncpy(output_pin_name, tokens[4], strlen(tokens[4]) -1); // -1 for the ',' //

        lib_cell->pins[j] = malloc(sizeof(char) * (strlen(output_pin_name) + 3) + 1);
        if(lib_cell->pins[j] == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        sprintf(lib_cell->pins[j], "(/%s)", output_pin_name);

        input_gatepins_names = malloc(sizeof(char*) * HASH_DEPTH);
        if(input_gatepins_names == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }

        i = 1;

        while(j>0)
        {
            input_gatepins_names[j] = malloc(strlen(comp_name) + strlen(lib_cell->pins[j-1]) + 1);
            strcpy(input_gatepins_names[j], comp_name);
            strcat(input_gatepins_names[j], lib_cell->pins[j-1]);

            comp->gatepins_array[i] = create_gatepins(input_gatepins_names[j],comp,lib_cell,0);
            i++;
            j--;
        }

        return 1;
    }

    return 1;
}


// This function creates a component //
struct component* create_component(char* comp_name, struct gatepins** gatepins_array, struct gatepins** connections, struct lib_hash* lib_cell)
{
    // variables //
    struct component* comp;

    // search if the component already exists in the hash table //
    comp = find_component(comp_name);

    // if the component exists, return it //
    if(comp != NULL)
    {
        return comp;
    }

    // if the component does not exist, create it //
    comp = malloc(sizeof(struct component));
    if(comp == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }

    // allocate memory for the component name //
    comp->comp_name = malloc(strlen(comp_name) + 1);
    if(comp->comp_name == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }
    // copy the component name //
    strcpy(comp->comp_name, comp_name);

    // assign the gatepins_array and connections pointers directly //
    comp->gatepins_array = gatepins_array;
    comp->connections = connections;

    // assign the lib_cell pointer directly //
    comp->lib_cell = lib_cell;

    insert_component(comp);

    return comp;
}

// This function creates a gatepin //
struct gatepins* create_gatepins(char* gatepin_name, struct component* component, struct lib_hash* lib_cell, int io)
{
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
        return gatepin;
    }

    // if the gatepin does not exist, create it //
    gatepin = malloc(sizeof(struct gatepins));
    if(gatepin == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }

    // allocate memory for the gatepin name //
    gatepin->gatepin_name = malloc(strlen(gatepin_name) + 1);
    if(gatepin->gatepin_name == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }
    // copy the gatepin name //
    strcpy(gatepin->gatepin_name, gatepin_name);

    // assign the component and lib_cell pointers directly //
    gatepin->component = component;
    gatepin->lib_cell = lib_cell;

    // copy the io //
    gatepin->io = io;

    insert_gatepins(gatepin);
    return gatepin;
}

// This function creates a library cell //
struct lib_hash* create_lib_hash(char* lib_cell_name, char* function, char** pins)
{
    // variables //
    int i = 0;
    struct lib_hash* lib_cell;

    // search if the library cell already exists in the hash table //
    lib_cell = find_lib_hash(lib_cell_name);

    // if the library cell exists, return it //
    if(lib_cell != NULL)
    {
        return lib_cell;
    }

    // if the library cell does not exist, create it //
    lib_cell = malloc(sizeof(struct lib_hash));
    if(lib_cell == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }

    // allocate memory for the library cell name //
    lib_cell->lib_cell_name = malloc(strlen(lib_cell_name) + 1);
    if(lib_cell->lib_cell_name == NULL)
    {
        printf("Error! Not enough memory.\n");
        exit(EXIT_FAILURE);
    }
    // copy the library cell name //
    strcpy(lib_cell->lib_cell_name, lib_cell_name);

    // allocate memory for the function //
    if(function != NULL)
    {
        lib_cell->function = malloc(strlen(function) + 1);
        if(lib_cell->function == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        // copy the function //
        strcpy(lib_cell->function, function);
    }
    else
    {
        lib_cell->function = NULL;
    }
    

    // allocate memory for the pins //
    if(pins != NULL)
    {
        lib_cell->pins = malloc(sizeof(char*) * HASH_DEPTH);
        if(lib_cell->pins == NULL)
        {
            printf("Error! Not enough memory.\n");
            exit(EXIT_FAILURE);
        }
        // copy the pins //
        for(i = 0; i < HASH_DEPTH; i++)
        {
            lib_cell->pins[i] = malloc(strlen(pins[i]) + 1);
            if(lib_cell->pins[i] == NULL)
            {
                printf("Error! Not enough memory.\n");
                exit(EXIT_FAILURE);
            }
            strcpy(lib_cell->pins[i], pins[i]);
        }
    }
    else
    {
        lib_cell->pins = NULL;
    }

    insert_lib_hash(lib_cell);

    return lib_cell;
}

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