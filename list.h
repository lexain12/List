#include <stdlib.h>

const char GraphFile[20] = "GraphFile.txt";

enum Errors 
{
    noErrors             = 0 << 0,
    listptrError         = 1 << 0,
    capacityError        = 1 << 1,
    dataError            = 1 << 2,
    sizeError            = 1 << 3,  
    sizeAndCapacityError = 1 << 4,
    listResizeUpError    = 1 << 5,
    memAllocError        = 1 << 6,
    listResizeDownError  = 1 << 7,
};

enum IsSorted
{
    listSorted    = 0,
    listNotSorted = 1,
};

struct  ListElement
{
    Elem_t element;
    size_t prevElementInd;
    size_t nextElementInd;
};

struct List_t
{
    ListElement* data;
    size_t       size;
    size_t       head;
    size_t       tail;
    size_t       free;
    size_t       capacity;
    int          status;
    IsSorted     isSorted;
};

int  listCtor (List_t* list, size_t size);
void listDtor (List_t* list);

void listDump (List_t* list, const char* str, ...);

int  listVerify (List_t* list);
int  listLinear (List_t* list);
size_t logicalNumberToPhysical (List_t* list, size_t anchorIndex);
size_t findElementByValue (List_t* list, Elem_t value);
void fillList (List_t* list);
int  listResizeUp (List_t* list);

size_t listInsertPrev (List_t* list, Elem_t element, size_t anchorIndex);
size_t listHeadInsert (List_t* list, Elem_t element);
size_t listTailAdd (List_t* list, Elem_t element);
Elem_t listDelete (List_t* list, size_t anchorIndex);
size_t listHeadAdd (List_t* list, Elem_t element);


size_t listBegin ();
size_t listEnd ();
