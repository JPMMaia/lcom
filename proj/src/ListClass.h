#ifndef _LIST_CLASS_H_
#define _LIST_CLASS_H_

#include "boolType.h"

typedef struct list_class ListClass;

ListClass* newList(int elemSize);
void deleteList(ListClass* List);

bool ListIsEmpty(ListClass* List);
void ListPopBack(ListClass* List);
void ListPopFront(ListClass* List);
void ListPushBack(ListClass* List, void* data);
void ListPushFront(ListClass* List, void* data);

void* ListFront(ListClass* List);
void* ListBack(ListClass* List);

#endif
