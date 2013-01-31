#include "chained_list.h"

// create_item:
// Creates and initializes a new item, or returns NULL, if it could not allocate memory.

itemType *create_item()
{
    static int guid_ctr=0;    // static counter to create unique ids
    itemType *ptr;

    ptr=malloc(sizeof(itemType));   // Allocate memory for the list item
    if(ptr!=NULL)
    {
        ptr->guid=guid_ctr++;   // Generate a unique ID
        printf("Enter name: ");
        scanf("%s",ptr->name);  // Enter a name
        ptr->size=1000;
        ptr->data=malloc(ptr->size*sizeof(unsigned char));  // Allocate some memory for a payload
        // TO DO: Have to check for NULL Pointer
    }
    return ptr;    // Return pointer to the new item
}
itemType *create_item_generic(char* name, void* data)
{

    static int guid_ctr=0;    // static counter to create unique ids
    itemType *ptr;

    ptr=malloc(sizeof(itemType));   // Allocate memory for the list item
    if(ptr!=NULL)
    {
        ptr->guid=guid_ctr++;   // Generate a unique ID
        if(name!=-1)strcpy(ptr->name,name);
        ptr->size=0;
        ptr->data=data;
        // TO DO: Have to check for NULL Pointer
    }
    return ptr;    // Return pointer to the new item
}
itemType *create_item_generic_payload(char* name, void* data,int data_size)
{

    static int guid_ctr=0;    // static counter to create unique ids
    itemType *ptr;

    ptr=malloc(sizeof(itemType));   // Allocate memory for the list item
    if(ptr!=NULL)
    {
        ptr->guid=guid_ctr++;   // Generate a unique ID
        strcpy(ptr->name,name);
        ptr->size=data_size;
        unsigned char *c=malloc(data_size*sizeof(unsigned char));
        for(int i=0;i<data_size;i++)
        {
            c[i]=((unsigned char*)data)[i];
        }
        ptr->data=c;
        // TO DO: Have to check for NULL Pointer
    }
    return ptr;    // Return pointer to the new item
}

// Insert_item:
// Inserts an existing item into the (potentially empty) list
void insert_item(itemType *new_item, itemType **list_ptr)
{
    new_item->next_item=*list_ptr;   // Append existing list to new item
    *list_ptr=new_item;              // Set list_ptr to points to new item
}
void print_item(itemType *item)
{
    if(item!=NULL)
        printf("Name: %s GUID: %d Addr: %d Addr.Successor: %d \n",item->name,item->guid,item,item->next_item);
}
// print all items
void print_items(itemType *list_start)
{
    while(list_start!=NULL)
    {
        print_item(list_start);
        list_start=list_start->next_item;
    }
}

// Finds an item with guid in the list
itemType *find_item(int guid, itemType *list_start)
{
    while(list_start!=NULL)
    {
        if(list_start->guid==guid)return list_start;
        list_start=list_start->next_item;
    }
    printf("Item not in List!");
    return NULL;
}

// Deletes an item and returns the successor of the deleted item
// The chaining of the previous and next element has to be done
// by the caller.
itemType *simple_delete_item(itemType *item)
{
    itemType *i2=item->next_item;
    if(item->size!=0)free(item->data);
    free(item);
    return i2;
}

// Complete delete item
// Deletes item 'to_delete' from the 'list'
// return 0, if the item does not exist in the list, otherwise 1
int delete_item(itemType *to_delete, itemType **list_ptr)
{
    itemType *help_ptr=*list_ptr;   //list_ptr has to point to the list start!
    if(to_delete!=NULL && *list_ptr!=NULL)
    {
        if(to_delete== *list_ptr)*list_ptr=simple_delete_item(to_delete);
        else
        {
            printf("%d",(*list_ptr)->next_item);
            while(help_ptr->next_item!=to_delete && help_ptr->next_item!=NULL)
            {
                help_ptr=help_ptr->next_item;
            }
            if(help_ptr->next_item!=NULL)
            {
                help_ptr->next_item=simple_delete_item(help_ptr->next_item);
            }
            else
            {
                printf("Warning: Item not in List! \n");
            }
        }
    }
    else printf("Warning: Item or list NULL!\n");
}


/*    @ Usage:
int main()
{
    itemType *my_list=NULL;
    itemType *current;
    int amount=1;
    printf("Amount of List Objects: ");
    scanf("%d",&amount);
    char c[500];
    for(int i=0; i<amount; i++)
    {
        char *c2="lukas";
        current=create_item_generic(c2,my_list);
        printf("%d",sizeof(c));
        insert_item(current,&my_list);
    }
    itemType *data=current->data;
    print_item(data);
    printf("\n");
    print_items(my_list);
    printf("\n");
    current=find_item(2,my_list);
    delete_item(current,&my_list);
    print_items(my_list);
}*/
