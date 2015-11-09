#include "hash.h"
/**
    HASH TABLE INITIALIZATION
**/
tTable * initHashTable(int size)
{
    tTable *t = NULL;
    if((t = malloc(sizeof(tTable))) == NULL)
    {
        printf("ERRk\n");
    }
    t->size = size;
    if ((t->array = malloc(sizeof(struct hItem ) * size )) == NULL)
    {
        printf("ERRa\n");
    }
    for (int i = 0; i < size; ++i)
    {
        t->array[i].isFree = true;
        t->array[i].next = NULL;
    }
    return t;
}
/**
    HASH TABLE FREE
**/
    /*
void freeHashTable(tTable *t)
{
   /for (int i = 0; i < t->size; ++i)
    {
        if (t->array[i].isFree)
        {   printf("HEY\n");
            tHItem *temp;
            temp=t->array[i].next;
            while(1){
                if (temp == NULL)
                {
                    break;
                }
                else
                {
                    tHItem *freedom;
                    freedom = temp;
                    free(freedom);
                    temp=temp->next;
                }
            }
        }
    }
    free(t->array);
    free(t);
    return ;
}*/
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
void insertHashTable(tTable *t, char *k){
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


int main()
{

    tTable *r = NULL;
    r = initHashTable(10);

    /*insertHashTable(&t,"lel");
    insertHashTable(&t,"lol");
    */
    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");

    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");
    //freeHashTable(r);
    printf("lel\n");
    return 0;
}
