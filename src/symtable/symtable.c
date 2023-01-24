/******************************************************************************
 *                                  IFJ22
 *                                symtable.c
 * 
 *      Authors: Daniil Kniazkin (xkniaz00)
 *      Purpose: Symtable
 * 
 *                        Last change: 23. 11. 2022
 *****************************************************************************/
#include "symtable.h"

#define BASE_SIZE_ARRAY 5

/**
 * @brief Adds a variable and its type to the structure SymtableData
 * @param data pointers to structures SymtableData
 * @param type the type that will be assigned
 */ 

void add_var(SymtableData *data,TypesInd varaibale_type){
    data->type = S_VAR;
    data->dtype.var_type = varaibale_type;
}



/**
 * @brief Adds a func to the structure SymtableData
 * @param data pointers to structures SymtableData
 */ 

void add_func(SymtableData *data){
    data->type = S_FUNC;
    data->dtype.func_type.size = BASE_SIZE_ARRAY;
    data->dtype.func_type.lenght = 0;
    data->dtype.func_type.func_params = (TypesInd*) malloc(sizeof(TypesInd) * data->dtype.func_type.size);
    data->dtype.func_type.quest = (int*) malloc(sizeof(int) * data->dtype.func_type.size);
}



/**
 * @brief Adds a param for func to the structure SymtableData
 * @param data pointers to structures SymtableData
 * @param param the param that will be assigned
 */ 

void add_func_param(SymtableData *data,TypesInd param, int isQuest){
    if(data->dtype.func_type.lenght == data->dtype.func_type.size){
        data->dtype.func_type.size *= 2;
        data->dtype.func_type.func_params = realloc(data->dtype.func_type.func_params,data->dtype.func_type.size * (sizeof(TypesInd)));
        data->dtype.func_type.quest = realloc(data->dtype.func_type.quest,data->dtype.func_type.size * (sizeof(bool)));
    }
    data->dtype.func_type.func_params[data->dtype.func_type.lenght] = param;
    data->dtype.func_type.quest[data->dtype.func_type.lenght] = isQuest;
    data->dtype.func_type.lenght++;
    /*printf("============\n");
    printf("%d\n", (int)data->dtype.func_type.lenght);
    for (int i = 0; i < (int)data->dtype.func_type.lenght; i++)
        printf("%d\n", (int)data->dtype.func_type.func_params[i]);
    printf("============\n");*/
}


/**
 * @brief Adds a return type for func to the structure SymtableData
 * @param data pointers to structures SymtableData
 * @param param the param that will be assigned
 */ 

void add_retype(SymtableData *data,TypesInd param){
    data->dtype.func_type.retype = param;
}



/**
 * @brief Inserts a new element into existing symbol table or updates existing node
 * @param table symbol table in which should be searching executed
 * @param key key of element that should be found
 * @param data pointers to structures SymtableData
 * @return correct insert
 */ 

int insertSymtable(Symtable *table,uint32_t key,SymtableData * data){
    if (*table == NULL) {
        //printf("NULL\n");
        if ((*table = (TreeNode_t*)malloc(sizeof(TreeNode_t))) == NULL)
            return 1;
        (*table)->data = *data;
        (*table)->key = key;
        (*table)->right = NULL;
        (*table)->left = NULL;
        return 0;
    }
    if(key == (*table)->key){       // free predchadzajuce data?
        (*table)->data = *data;
        return 0;
    }
    if (key > (*table)->key)
        return insertSymtable(&((*table)->right), key, &(*data));
    if (key < (*table)->key)
        return insertSymtable(&((*table)->left), key, &(*data));
    return 1;
}



/**
 * @brief Returns a pointer to the parameter list
 * @param data structures SymtableData
 * @return Pointer to the parameter list
 */ 

TypesInd * return_param_func(SymtableData data){
    return data.dtype.func_type.func_params;
}

int * return_quest_func(SymtableData data) {
    return data.dtype.func_type.quest;
}



/**
 * @brief clears parameters 
 * @param data double pointers to structures SymtableData
 */ 

void freeParams(SymtableData **data){
    free((*data)->dtype.func_type.func_params);
}



/**
 * @brief Returns the structure depending on the key
 * @param table symbol table in which should be searching executed
 * @param key key of element that should be found
 * @return Pointer to SymtableData
 */ 

SymtableData *symtableGet(Symtable *table,uint32_t key){
    if(*table == NULL) return NULL;
    if(key > (*table)->key) 
        return symtableGet(&(*table)->right,key);
    if(key < (*table)->key)
        return symtableGet(&(*table)->left,key);
    return &(*table)->data;
}



/**
 * @brief Checking for availability in the table
 * @param table symbol table in which should be searching executed
 * @param key key of element that should be found
 * @return bool type
 */

bool symtableSearch(Symtable *table,uint32_t key){
    if(*table == NULL) return false;
    if(key > (*table)->key) 
        return symtableSearch(&(*table)->right,key);
    if(key < (*table)->key)
        return symtableSearch(&(*table)->left,key);
    return true;
}



/**
 * @brief Replaces the type with a variable depending on the key
 * @param table symbol table in which should be searching executed
 * @param key key of element that should be found
 * @param type the type that will be assigned
 */ 

void replaceType(Symtable * table,uint32_t key,TypesInd new_type){
    SymtableData * data = symtableGet(&(*table),key);
    data->dtype.var_type = new_type;
    insertSymtable(&(*table),key,data);
}



/**
 * @brief Insert in table built functions
 * @param table pointer to structures Symtable
 */ 
void built_function(Symtable * table){
    
    
    SymtableData reads;
    add_func(&reads);
    add_retype(&reads,STRING_IND);
    if(insertSymtable(&(*table),get_key("reads"),&reads)){
        fprintf(stderr,"Error insert table in build function!");
    }


    SymtableData readi;
    add_func(&readi);
    add_retype(&readi,INT_IND);
    if(insertSymtable(&(*table),get_key("readi"),&readi)){
        fprintf(stderr,"Error insert table in build function!");
    }


    SymtableData readf;
    add_func(&readf);
    add_retype(&readf,FLOAT_IND);
    if(insertSymtable(&(*table),get_key("readf"),&readf)){
        fprintf(stderr,"Error insert table in build function!");
    }
    

    SymtableData strlen;
    add_func(&strlen);
    add_func_param(&strlen,STRING_IND, 0);
    add_retype(&strlen,INT_IND);
    if(insertSymtable(&(*table),get_key("strlen"),&strlen)){
        fprintf(stderr,"Error insert table in build function!");
    }

    SymtableData substring;
    add_func(&substring);
    add_func_param(&substring,STRING_IND, 0);
    add_func_param(&substring,INT_IND, 0);
    add_func_param(&substring,INT_IND, 0);
    add_retype(&substring,STRING_IND);
    if(insertSymtable(&(*table),get_key("substring"),&substring)){
        fprintf(stderr,"Error insert table in build function!");
    }


    SymtableData write;
    add_func(&write);
    add_retype(&write, VOID_IND);
    if(insertSymtable(&(*table),get_key("write"),&write)){
        fprintf(stderr,"Error insert table in build function!");
    }

    SymtableData ord;
    add_func(&ord);
    add_func_param(&ord,STRING_IND, 0);
    add_retype(&ord,INT_IND);
    if(insertSymtable(&(*table),get_key("ord"),&ord)){
        fprintf(stderr,"Error insert table in build function!");
    }

    SymtableData chr;
    add_func(&chr);
    add_func_param(&chr,INT_IND, 0);
    add_retype(&chr,STRING_IND);
    if(insertSymtable(&(*table),get_key("chr"),&chr)){
        fprintf(stderr,"Error insert table in build function!");
    }

    SymtableData floatval;
    add_func(&floatval);
    add_retype(&floatval,FLOAT_IND);
    if(insertSymtable(&(*table),get_key("floatval"),&floatval)){
        fprintf(stderr,"Error insert table in build function!");
    }

    SymtableData intval;
    add_func(&intval);
    add_retype(&intval,INT_IND);
    if(insertSymtable(&(*table),get_key("intval"),&intval)){
        fprintf(stderr,"Error insert table in build function!");
    }

    SymtableData strval;
    add_func(&strval);
    add_retype(&strval,STRING_IND);
    if(insertSymtable(&(*table),get_key("strval"),&strval)){
        fprintf(stderr,"Error insert table in build function!");
    }
}

/**
 * @brief Return min Value in Tree
 * @param table pointer to structures Symtable
 */ 
Symtable minValueNode(Symtable table){
    Symtable current = table;

    while(current && current->left != NULL){
        current = current->left;
    }

    return current;
}


/**
 * @brief Delete synbol from symtable
 * @param table pointer to structures Symtable
 * @param key key to delete
 */ 
Symtable symtableDelete(Symtable *table,uint32_t key){
    if(*table == NULL){return *table;}

    if(key < (*table)->key){
        (*table)->left = symtableDelete(&(*table)->left,key);
    }

    else if(key > (*table)->key){
        (*table)->right = symtableDelete(&(*table)->right,key);
    }

    else{
        if((*table)->left == NULL){
            Symtable temp = (*table)->right;
            free((*table));
            return temp;
        }
        Symtable temp = minValueNode((*table)->right);
        (*table)->key = temp->key;
        (*table)->right = symtableDelete(&(*table)->right,key);
    }

    return *table;
}
