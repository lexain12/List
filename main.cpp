#include <cassert>
#include <stdio.h>
typedef int Elem_t;
#include "list.h"
#define Format_ "%-3d"

extern FILE* LOGFILEPTR;

void listCtor (List_t* list, size_t capacity)
{
    ListElement* newDataPtr = (ListElement*) calloc (sizeof(ListElement), capacity + 1);
    assert(newDataPtr != nullptr);

    list->data     = newDataPtr;
    list->capacity = capacity;
    list->head     = 0;
    list->tail     = 0;
    list->free     = 1;

    list->data[0].element = 0;
    list->data[0].prevElementInd = 0;
    list->data[0].nextElementInd = 0;

    for (size_t index = 1; index < capacity; ++index)
    {
        list->data[index].element = (Elem_t) (index + 1);
    }

    return;
}

void listDtor (List_t* list)
{

    list->capacity = 0;
    list->size     = 0;
    list->head     = 0;
    list->tail     = 0;
    list->free     = 0;

    list->data[0].element        = 0;
    list->data[0].prevElementInd = 0;
    list->data[0].nextElementInd = 0;

    free(list->data);
    list->data = nullptr;

    return;
}

size_t listTailAdd (List_t* list, Elem_t element)
{
    //listVerify (&list);
    
    assert (list != nullptr);
    assert (list->capacity - list->size > 0);

    size_t tail = list->tail;
    list->tail  = list->free;
    list->free  = (size_t) list->data[list->tail].element;

    list->data[tail].nextElementInd = list->tail;
    
    list->data[list->tail].prevElementInd = tail;
    list->data[list->tail].element        = element;

    list->size += 1;

    return list->tail;
}

void listDump (List_t* list)
{
    assert(list != nullptr);

    fprintf(LOGFILEPTR, "List size: %lu, head: %lu, tail: %lu, free: %lu, capacity: %lu\n",
          list->size, list->head, list->tail, list->free, list->capacity);
    fprintf(LOGFILEPTR, "------------------------------------------------------\n");

    fprintf(LOGFILEPTR, "index   ");
    for ( size_t index = 0; index <= list->capacity; ++index)
    {
        fprintf(LOGFILEPTR, "%-3lu ", index);
    }
    fprintf(LOGFILEPTR, "\n");

    fprintf(LOGFILEPTR, "elem    ");
    for ( size_t index = 0; index <= list->capacity; ++index)
    {
        fprintf(LOGFILEPTR, "%-3d ", list->data[index].element);
    }
    fprintf(LOGFILEPTR, "\n");

    fprintf(LOGFILEPTR, "prevInd ");
    for ( size_t index = 0; index <= list->capacity; ++index)
    {
        fprintf(LOGFILEPTR, "%-3lu ", list->data[index].prevElementInd);
    }
    fprintf(LOGFILEPTR, "\n");

    fprintf(LOGFILEPTR, "nextInd ");
    for ( size_t index = 0; index <= list->capacity; ++index)
    {
        fprintf(LOGFILEPTR, "%-3lu ", list->data[index].nextElementInd);
    }
    fprintf(LOGFILEPTR, "\n");

    return;
}

size_t listInsertPrev (List_t* list, Elem_t element, size_t anchorIndex)
{
    assert (list != nullptr);
    size_t prevInd = list->data[anchorIndex].prevElementInd;

    list->data[list->free].element         = element; 
    list->data[list->free].prevElementInd  = prevInd;
    list->data[list->free].nextElementInd  = anchorIndex;

    list->data[prevInd].nextElementInd     = list->free;
    list->data[anchorIndex].prevElementInd = list->free;

    list->size += 1;
    list->free += 1;
    list->tail += 1;
    
    return list->free;
}

Elem_t listDelete (List_t* list, size_t anchorIndex)
{
    size_t prevInd = list->data[anchorIndex].prevElementInd;
    size_t nextInd = list->data[anchorIndex].nextElementInd;
    Elem_t element = list->data[anchorIndex].element;

    list->data[prevInd].nextElementInd = nextInd;
    list->data[nextInd].prevElementInd = prevInd;

    list->data[anchorIndex].element = (int) list->free;
    list->free = anchorIndex;
    list->data[anchorIndex].prevElementInd = 0;
    list->data[anchorIndex].nextElementInd = 0;

    return element;    
}

int main()
{
    List_t list1 = {};

    listCtor (&list1, 10);

    listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    listInsertPrev (&list1, 8, 3);
    listDelete (&list1, 3);

    listDump(&list1);

    return 0;
}
