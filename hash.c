#include "hash.h"
/**
    HASH TABLE INITIALIZATION
**/
tTable * initHashTable(int size)
{
    tTable *temp = NULL;
    if((temp =(tTable *)malloc( (sizeof(tTable)) + (sizeof(tHItem *) * size) )) == NULL)
    {
        printf("ERRk\n");
    }
    temp->size = size;
    printf("%d\n",temp->size );
    for (int i = 0; i < temp->size; ++i)
    {
        hTabItem *e = NULL;
        e =(hTabItem*) malloc ( sizeof(hTabItem) );
        e->isFree = true;
        e->next = NULL;
        temp->array[i] = e;
    }
    /*temp->array[5]->name = "Hell";
    temp->array[5]->value = 9;
    temp->array[5]->function= "Hell";
*/

/*    hTabItem *e;
    e =(hTabItem*) malloc ( sizeof(hTabItem) );
    temp->array[0] =e;

    temp->array[0]->value = 5;
    temp->array[0]->name  = "Martin";
    printf("%s\n", temp->array[0]->name);
    free(e);
    printf("lel\n" );*/
    return temp;
}
/**
    HASH TABLE FREE
**/
void freeHashTable(tTable *temp)
{
    if (temp == NULL) return;
    for (int i = 0; i < temp->size; ++i)
    {
        hTabItem *freeItem;
        while(temp->array[i]->next != NULL)
        {
            freeItem = temp->array[i]->next;
            temp->array[i]->next = freeItem->next;
            free(freeItem);
        }
        free(temp->array[i]);
    }
    free(temp);
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
    Insert into table
**/
void insertHashTable(tTable *t, char *k){
    static int counter = 0;
    int index = getIndex(k,t->size);
    ++counter;
    if (t->array[index]->isFree)
    {
        t->array[index]->isFree = false;
        t->array[index]->key = k;
        t->array[index]->name = k;
    }
    else
    {
        hTabItem *newItem;
        if( (newItem =(hTabItem*) malloc ( sizeof(hTabItem) ) ) == NULL)
        {
            fprintf(stderr, "Could not allocate new item \n");
        }
        newItem->next = NULL;
        newItem->isFree = false;
        newItem->key = k;
        if (t->array[index]->next == NULL)
        {
            t->array[index]->next = newItem;
        }
        else
        {
            while (t->array[index]->next != NULL)
            {
                printf("%d\n",counter);
                hTabItem *i;
                i = t->array[index]->next;
                t->array[index]->next = i->next;
            }
            t->array[index]->next = newItem;
        }

    }
    return;
}


int main()
{

    tTable *r = NULL;
    r = initHashTable(10);
    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");

    //printf("%d\n",r->size);
    /*insertHashTable(&t,"lel");
    insertHashTable(&t,"lol");



    */freeHashTable(r);
    printf("lel\n");
    return 0;
}
