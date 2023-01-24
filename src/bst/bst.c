/******************************************************************************
 *                                  IFJ22
 *                                  bst.c
 * 
 *      Authors: Kniazkin Daniil (xkniaz00), Nikita Kotvitskiy (xkotvi01)
 *      Purpose: Definition of operations on binary search trees
 * 
 *                        Last change: 16. 11. 2022
 *****************************************************************************/

#include "bst.h"
#include <stdio.h>
#include <stdlib.h>

void bstInit(bst_t ** bst) {
    *bst = NULL;
}
void bstDestroy(bst_t ** bst) { 
    if (*bst != NULL) {
        bstDestroy(&((*bst)->left));
        bstDestroy(&((*bst)->right));
        free(*bst);
        *bst = NULL;
    }
}
bstData_t bstGet(bst_t * bst, key_t key) {
    if (bst == NULL)
        return NULL;
    if (key > bst->key)
        return bstGet(bst->right, key);
    if (key < bst->key)
        return bstGet(bst->left, key);
    return bst->data;
}
int bstInsert(bst_t ** bst, key_t key, bstData_t data) {
    if (*bst == NULL) {
        if ((*bst = malloc(sizeof(bst_t))) == NULL)
            return 1;
        (*bst)->data = data;
        (*bst)->key = key;
        (*bst)->right = NULL;
        (*bst)->left = NULL;
        return 0;
    }
    if (key > (*bst)->key)
        return bstInsert(&((*bst)->right), key, data);
    if (key < (*bst)->key)
        return bstInsert(&((*bst)->left), key, data);
    return 1;
}

bool bstSearch(bst_t * bst, key_t key) {
    if (bst == NULL)
        return false;
    if (key > bst->key)
        return bstSearch(bst->right, key);
    if (key < bst->key)
        return bstSearch(bst->left, key);
    return true;
}

//Source: https://en.wikipedia.org/wiki/Jenkins_hash_function
key_t get_key(const char *str)
{
    key_t hash = 0;
    size_t i = 0;
    while (str[i]) {
        hash += str[i];
        hash += hash << 10;
        hash ^= hash >> 6;
        i++;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}