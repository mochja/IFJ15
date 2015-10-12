#include "list.h"


void listInit(tvarList *L){
	L->First=NULL;
	L->Last=NULL;
}

void listDispose(tvarList *L){
	tItemPtr tmp;
	tmp=L->First;

	while(tmp != NULL){
		L->First=L->First->next;
		free(tmp);
		tmp=L->First;
	}
}

void insertLast(tItemPtr item, tvarList *L){
	item->next=NULL;
	item->prev=L->Last;

	if(L->Last != NULL){
		L->Last->next = item;
	}
	else{
		L->First = item;
	}
		L->Last = item;
}

void deleteLast(tvarList *L){
	tItemPtr tmp;
	tmp=L->Last;

	if(tmp != NULL){
		if(L->Last == L->First){
			L->First = NULL;
		}
		L->Last=L->Last->prev;
		free(tmp);
	}
}

char * varSearch(tvarList *L, char * name){
	tItemPtr tmp;
	tmp=L->Last;

	char * result = NULL;

	while(tmp != NULL){
	int i = 0;	
		while(tmp->data[i].id != NULL){
			if(!strcmp(name, tmp->data[i].id))
			{
				result = tmp->data[i].hid;
				return result;
			}
			i++;
		}
	tmp=tmp->prev;
	}

	return result;
}

char * paramSearch(tvarList *L, char * fid, char *name){
	tItemPtr tmp;
	tmp=L->First;
	char * result = NULL;

	while(tmp != NULL){
		if(!strcmp(tmp->functionId,fid)){
			int i = 0;

			while(tmp->data[i].id != NULL){
				if(!strcmp(name, tmp->data[i].id)){
					result = tmp->data[i].hid;
					break;
				}
				i++;
			}
			return result;
		}
		tmp=tmp->next;
	}
}