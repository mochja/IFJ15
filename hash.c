#include "hash.h"
/**
    HASH TABLE INITIALIZATION
**/
tTable * initHashTable(int size)
{
    tTable *t = NULL;   //initialization of table itself
    if((t =(tTable *)malloc( (sizeof(tTable)) + (sizeof(tHItem *) * size) )) == NULL)
    {
        return NULL;
    }
    t->size = size;
    for (int i = 0; i < t->size; ++i)   //setting up startup values for correct
    {                                   //evaluation of conditions
        t->array[i] = NULL;
    }
    return t;                           //returning pointer to new table
}
/**
    HASH TABLE FREE
**/
void freeHashTable(tTable *t)
{
    static int x =0;
    ++x;
    if (t == NULL) return;              //check is table is initialized

    for (int i = 0; i < t->size; ++i)   //run through table to check all fields of array
    {
        hTabItem *freeItem;
        freeItem = t->array[i];
        while(true)                     // going trough list of items
        {
            if (t->array[i] == NULL || t->array[i]->next == NULL)
            {
                break;
            }
            else
            {
                freeItem = t->array[i]->next;
                t->array[i]->next = freeItem->next;
                free(freeItem);
            }
        }
        free(t->array[i]);
    }
    free(t);
    return ;
}
/**
    Hash Function
**/
unsigned int getIndex(const char *str,int tableSize) {    //hash funkction return index value calculated for
          unsigned int h=0;                 //selected string
          const unsigned char *p;
          for(p=(const unsigned char*)str; *p!='\0'; p++)
              h = 65599*h + *p;
          return h % tableSize;
}
/**
    Creating new item in hash table
**/
hTabItem * createNewItem()
{
    static int counter = 0;
    ++counter;
    hTabItem *newItem;
    if( (newItem =(hTabItem*) malloc ( sizeof(hTabItem) ) ) == NULL)
    {
        return NULL;
    }
    newItem->next = NULL;       // setting default values

    newItem->id = NULL;
    newItem->name = NULL;
    newItem->function = NULL;
    newItem->dataType = -1;

    newItem->isDefined = false;
    newItem->params = -1;
    newItem->paramPosition = -1;

    return newItem;
}
/**
    Insert into table
**/
void insertHashTable(tTable *t, hTabItem *newItem){

    int index = getIndex(newItem->id,t->size);          //get index of key

    if (t->array[index] == NULL)                        // first item on position
    {
        t->array[index] = newItem;
    }
    else                                                //item is already in position
    {                                                   // going through list to find
        hTabItem *i;                                    // nnext free position
        i = t->array[index];
        while(true)
        {
            if (i->next == NULL)
            {
                i->next = newItem;
                break;
            }
            else
            {
                i=i->next;
            }
        }
    }
    return;
}
/**
    search in table and return pointer to item
**/
hTabItem * searchItem(tTable *t, char *id)
{
    int index = getIndex(id,t->size);
    if(t->array[index] == NULL )                    // checking if Item exists
    {
        return NULL;
    }
    else if ( strcmp(t->array[index]->id,id) == 0 )
    {

        return t->array[index];                     //item found in first place
    }
    else                                            //item not found going through list
    {
        hTabItem *i;
        i = t->array[index];
        while(i != NULL)
        {
            if( strcmp(i->id,id) == 0 )
            {
                return i;
            }
            else
            {
                i=i->next;
            }
        }
    }
    return NULL;
}
/*

Main for testing purposes

*/
/*int main()
{

    tTable *r = NULL;
    r = initHashTable(10);

    hTabItem *temp;

    temp = createNewItem();
    if (temp == NULL) return 1;
    temp->id = "martin";
    temp->value.i = 0;
    insertHashTable(r,temp);

    temp = createNewItem();
    if (temp == NULL) return 1;
    temp->id = "martin";
    temp->value.i = 1;
    insertHashTable(r,temp);

    temp = createNewItem();
    if (temp == NULL) return 1;
    temp->id = "martin";
    temp->value.i = 2;
    insertHashTable(r,temp);

    for (hTabItem *i = r->array[7]; i != NULL; i = i->next)
    {
        printf("1:%d \t%s\n",i->value.i,i->id);
    }
    hTabItem *f;
    char *e;
    e = "martin";
    f=searchItem(r,e);
    if (f != NULL)
    {
        printf("1:%d \t%s\n",f->value.i,f->id);
    }
    else
        printf("Not found\n");
    freeHashTable(r);

    return 0;
}
*/
