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
        temp->array[0] = e;
    }
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

    free(temp->array[10]);
    /*for (int i = 0; i < temp->size; ++i)
    {
        hTabItem *e = NULL;
        e = temp->array[i];
        free(e);

    }*/
    free(temp);
    return ;
}
/**
    Hash Function
**/
unsigned int getIndex(const char *str) {    //hash funkction return index value calculated for
          unsigned int h=0;                 //selected string
          const unsigned char *p;
          for(p=(const unsigned char*)str; *p!='\0'; p++)
              h = 65599*h + *p;
          return h % MAX_HTSIZE;
}
/**
    Insert into table
**/
/*void insertHashTable(tTable *t, char *k){
    static int counter = 0;
    int index = getIndex(k);
    ++counter;
    if (t->array[index].isFree)
    {
        t->array[index].isFree = false;
        t->array[index].key = k;
    }
    else
    {
        tHItem *temp;
        temp=t->array[index].next;
        while(1){
            if (temp == NULL)
            {
                if ((temp = malloc(sizeof(tHItem*))) == NULL)
                {
                    printf("ERRw\n");
                }
                temp->name = k;
                temp->key= k;
                printf("%s\n",temp->name);
                printf("c:%d index:%d\n",counter,index );
                free(temp);
                break;
            }
            else
            {
                temp=temp->next;
            }
        }
    }
}
*/

int main()
{

    tTable *r = NULL;
    r = initHashTable(10);
    //printf("%d\n",r->size);
    /*insertHashTable(&t,"lel");
    insertHashTable(&t,"lol");

    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");

    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");
    */freeHashTable(r);
    printf("lel\n");
    return 0;
}
