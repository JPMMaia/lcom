#include "ListClass.h"

typedef struct list_node ListNode;

struct list_node
{
	void* data;
	ListNode* Previous;
	ListNode* Next;
};
struct list_class
{
	unsigned long size;
	unsigned long elemSize;
	ListNode* Front;
	ListNode* Back;
};


/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

ListNode* newListNode(void* data, ListNode* Previous, ListNode* Next, unsigned long elemSize)
{
	ListNode* Node;
	Node = (ListNode*) malloc(sizeof(ListNode));

	Node->data = (void*) malloc(elemSize);
	memcpy(Node->data, data, elemSize);
	Node->Previous = Previous;
	Node->Next = Next;

	return Node;
}
void deleteListNode(ListNode* Node)
{
	free(Node->data);
	free(Node);
}

ListClass* newList(int elemSize)
{
	ListClass* List;
	List = (ListClass*) malloc(sizeof(ListClass));

	List->Front = 0;
	List->Back = 0;
	List->size = 0;
	List->elemSize = elemSize;

	return List;
}
void deleteList(ListClass* List)
{
	unsigned long i;
	ListNode* node;

	while(ListIsEmpty(List) != true)
		ListPopBack(List);

	free(List);
}


//============================= OPERATIONS ===================================

void ListPopBack(ListClass* List)
{
	if(ListIsEmpty(List) != true)
	{
		ListNode* Node;
		Node = List->Back;
		if(List->size != 1)
		{
			List->Back = Node->Next;
			List->Back->Previous = 0;
		}
		deleteListNode(Node);
		List->size--;
	}
}
void ListPushBack(ListClass* List, void* data)
{
	ListNode* Node;
	Node = newListNode(data, 0, List->Back, List->elemSize);

	if(List->size != 0)
		List->Back->Previous = Node;
	else
		List->Front = Node;

	List->Back = Node;
	List->size++;
}
void ListPopFront(ListClass* List)
{
	if(!ListIsEmpty(List))
	{
		ListNode* Node;
		Node = List->Front;
		if(List->size != 1)
		{
			List->Front = Node->Previous;
			List->Front->Next = 0;
		}
		deleteListNode(Node);
		List->size--;
	}
}
void ListPushFront(ListClass* List, void* data)
{
	ListNode* Node;
	Node = newListNode(data, List->Front, 0, List->elemSize);

	if(List->size != 0)
		List->Front->Next = Node;
	else
		List->Back = Node;

	List->Front = Node;
	List->size++;
}


//=============================   ACCESS   ===================================

void* ListFront(ListClass* List)
{
	if(ListIsEmpty(List))
		return 0;

	return List->Front->data;
}
void* ListBack(ListClass* List)
{
	if(ListIsEmpty(List))
		return 0;

	return List->Back->data;
}


//============================= INQUIRY    ===================================

bool ListIsEmpty(ListClass* List)
{
	return List->size == 0;
}
