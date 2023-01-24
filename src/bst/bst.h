/******************************************************************************
 *                                  IFJ22
 *                                  bst.h
 * 
 *      Authors: Kniazkin Daniil (xkniaz00)
 *      Purpose: Definition of operations on binary search trees
 * 
 *                        Last change: 16. 11. 2022
 *****************************************************************************/

#ifndef IFJ_BST
#define IFJ_BST

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t key_t;
typedef void * bstData_t;

typedef struct BST {
    key_t key;
    bstData_t data;
    struct BST * left;
    struct BST * right;
} bst_t;

//Initialization of tree
void bstInit(bst_t ** bst);
//Tree destroying
void bstDestroy(bst_t ** bst);
//Cheching if item with key is in tree
bool bstSearch(bst_t * bst, key_t key);
//Returns data of item with key or NULL, if this item doesn't exist
bstData_t bstGet(bst_t * bst, key_t key);
//Inserting new item in tree and returnes 0. If item with the key already exists in tree, returnes 1.
int bstInsert(bst_t ** bst, key_t key, bstData_t data);
//Generation of key
key_t get_key(const char *str);

#endif