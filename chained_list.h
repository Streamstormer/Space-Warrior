#ifndef CHAINED_LIST_H_INCLUDED
#define CHAINED_LIST_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Data structure of the individual items in the list.
struct myListItemStruct
{
    int guid;             // a unique id to identify the list item
    char name[128];       // a name to represent the item
    void *data;  // arbitrary data 'payload'
    int size;             // size of data
    struct myListItemStruct *next_item;   // pointer to next item
};

// Define an own type for shorter declarations
typedef struct myListItemStruct itemType;

/*
 *  create_item:
 *  Creates and initializes a new item, or returns NULL, if it could not allocate memory.
 */
itemType *create_item();
itemType *create_item_generic(char* name, void* data);
itemType *create_item_generic_payload(char* name, void* data,int data_size);
/*
 *  Insert_item:
 *  Inserts an existing item into the (potentially empty) list
 */
void insert_item(itemType *new_item, itemType **list_ptr);
void print_item(itemType *item);
// print all items
void print_items(itemType *list_start);
// Finds an item with guid in the list
itemType *find_item(int guid, itemType *list_start);
/*
 * Deletes an item and returns the successor of the deleted item
 * The chaining of the previous and next element has to be done
 * by the caller.
 */
itemType *simple_delete_item(itemType *item);
/*
 * Complete delete item
 * Deletes item 'to_delete' from the 'list'
 * return 0, if the item does not exist in the list, otherwise 1
 */
int delete_item(itemType *to_delete, itemType **list_ptr);


#endif // CHAINED_LIST_H_INCLUDED
