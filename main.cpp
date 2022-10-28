#include <cassert>
#include <stdio.h>
typedef int Elem_t;
#include "list.h"
#define Format_ "%-3d"

extern FILE* LOGFILEPTR;

int listCtor (List_t* list, size_t capacity)
{
    int errors = noErrors;
    ListElement* newDataPtr = (ListElement*) calloc (sizeof(ListElement), capacity + 1);

    if (newDataPtr == nullptr)  
        return errors |= listptrError;

    list->data     = newDataPtr;
    list->capacity = capacity;
    list->head     = 0;
    list->tail     = 0;
    list->free     = 1;
    list->size     = 0;
    list->status   = listSorted;

    list->data[0].element = 0;
    list->data[0].prevElementInd = 0;
    list->data[0].nextElementInd = 0;

    fillList (list);

    if (errors |= listVerify (list))
    {
    }

    return noErrors;
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
    int errors = noErrors;    
    if (errors |= listVerify (list))
    {
    }
    
    if (list->size == list->capacity)
    {
        listResizeUp (list);
    }

    size_t tail = list->tail;
    list->tail  = list->free;
    list->free  = list->data[list->tail].nextElementInd;

    list->data[tail].nextElementInd = list->tail;
    
    list->data[list->tail].prevElementInd = tail;
    list->data[list->tail].element        = element;

    list->size += 1;

    if (errors |= listVerify (list))
    {
    }

    return list->tail;
}


void listDump (List_t* list)
{
    assert(list != nullptr);
    fprintf(LOGFILEPTR, "<pre>\n");

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
    int errors = noErrors;    
    if (errors |= listVerify (list))
    {
    }

    assert (list != nullptr);

    if (list->size == list->capacity)
    {
        listResizeUp (list);
    }

    size_t prevInd = list->data[anchorIndex].prevElementInd;
    list->free  = list->data[list->tail].nextElementInd;

    list->data[list->free].element         = element; 
    list->data[list->free].prevElementInd  = prevInd;
    list->data[list->free].nextElementInd  = anchorIndex;

    list->data[prevInd].nextElementInd     = list->free;
    list->data[anchorIndex].prevElementInd = list->free;

    list->size += 1;
    list->tail += 1;
    list->status   = listNotSorted;

    if (errors |= listVerify (list))
    {
    }
    
    return list->free;
}

Elem_t listDelete (List_t* list, size_t anchorIndex)
{
    int errors = noErrors;    
    if (errors |= listVerify (list))
    {
    }

    size_t prevInd = list->data[anchorIndex].prevElementInd;
    size_t nextInd = list->data[anchorIndex].nextElementInd;
    Elem_t element = list->data[anchorIndex].element;

    list->data[prevInd].nextElementInd = nextInd;
    list->data[nextInd].prevElementInd = prevInd;

    list->data[anchorIndex].nextElementInd = list->free;
    list->data[anchorIndex].prevElementInd = 0;
    list->data[anchorIndex].element = 0;

    list->free = anchorIndex;
    list->size -= 1;
    list->status   = listNotSorted;

    if (errors |= listVerify (list))
    {
    }

    return element;    
}

int listVerify (List_t* list)
{
    size_t epsiloh = -1;
    epsiloh       /=  2;

    int errors     = noErrors;
    if (list)
    {
        if (list->capacity     > epsiloh)
            errors |= capacityError;
        
        if (list->size         > epsiloh)
            errors |= sizeError;

        if (list->size         > list->capacity)
            errors |= sizeAndCapacityError;

        if (!list->data)
            return errors |= dataError;
        
    }
    else
        errors |= listptrError;

    return errors;
}

int listLinear (List_t* list)
{
    int errors = noErrors;    
    if (errors |= listVerify (list))
    {
    }

    ListElement* newData = (ListElement*) calloc (list->capacity + 1, sizeof(ListElement));
    if (newData == nullptr) 
        return errors |= memAllocError;
 
    size_t index  = 0;
    size_t index1 = 0;
    while (list->size  != index1 - 1)
    {
        newData[index1]                = list->data[index];
        newData[index1].prevElementInd = index1 - 1;
        newData[index1].nextElementInd = index1 + 1;
 
        index = list->data[index].nextElementInd;
        index1++;
    }

    newData[index1 - 1].nextElementInd = 0;
    newData[0]         .prevElementInd = 0;

    list->free = index1;
    list->data = newData;    
    fillList (list);
    list->tail = index1 - 1;

    if (errors |= listVerify (list))
    {
    }

    return noErrors;
}

size_t logicalNumberToPhysical (List_t* list, size_t anchorIndex)
{
    int errors = noErrors;    
    if (errors |= listVerify (list))
    {
    }

    size_t index  = 0;
    size_t index1 = 0;
    while (index1 != anchorIndex)
    {
        index = list->data[index].nextElementInd;
        index1++;
    }
    
    if (errors |= listVerify (list))
    {
    }

    return index;
}

size_t findElementByValue (List_t* list, Elem_t value)
{
    int errors = noErrors;    
    if (errors |= listVerify (list))
    {
    }

    size_t index  = 0;
    while (list->data[index].element != value)
    {
        index = list->data[index].nextElementInd;
    }
    
    if (errors |= listVerify (list))
    {
    }

    return index;
}

void fillList (List_t* list)
{
    size_t index = list->free;
    for (; index <= list->capacity; ++index)
    {
        list->data[index].element = 0;
        list->data[index].prevElementInd = 0;
        list->data[index].nextElementInd = index + 1;
    }

    list->data[index - 1].nextElementInd = 0;
}

int listResizeUp (List_t* list)
{
    int errors = noErrors;    
    if (errors |= listVerify (list))
    {
    }

    ListElement* newData = (ListElement*) realloc (list->data, list->capacity * 4 * sizeof(ListElement));
    if (newData == nullptr)
        return listResizeUpError;

    list->data      = newData;
    list->free = list->capacity + 1;
    list->capacity *= 2;

    fillList (list);

    if (errors |= listVerify (list))
    {
    }

    return noErrors;
}

int listResizeDown (List_t* list, size_t newCapacity)
{
    int errors = 0;
    if (errors |= listVerify (list))
    {
    }

    
    if (newCapacity < list->size && list->status == listNotSorted)
        return errors |= listResizeDownError;

    ListElement* newData = (ListElement*) realloc (list->data, (newCapacity + 1)  * sizeof(ListElement));
    if (newData == nullptr)
        return listResizeDownError;

    list->capacity = newCapacity;
    list->data     = newData;
    list->data[list->capacity].nextElementInd = 0;

    return noErrors;
}

int main()
{
    List_t list1 = {};

    listCtor (&list1, 10);

    listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    listDump (&list1);

    listResizeDown (&list1, 5);
    listDump (&list1);

    listDtor (&list1);

    return 0;
}
