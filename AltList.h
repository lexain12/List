#include <stdlib.h>

const char GraphFile[20] = "GraphFileAlt.txt";

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
    ListElement* prevElementInd;
    ListElement* nextElementInd;
};

struct List_t
{
    ListElement* nullElement;
    size_t       size;
    int          status;
};

int  listCtor (List_t* list);
void listDtor (List_t* list);

void listDump (List_t* list, const char* str, ...);

int  listVerify (List_t* list);
size_t logicalNumberToPhysical (List_t* list, ListElement* anchorElement);
ListElement* findElementByValue (List_t* list, Elem_t value);
void myGraph (List_t* list);

ListElement* listInsertPrev (List_t* list, ListElement* anchorElement, Elem_t element);
ListElement* listHeadAdd (List_t* list, Elem_t element);
ListElement* listInsertAfter (List_t* list, ListElement* anchorIndex, Elem_t element);
ListElement* listTailAdd (List_t* list, Elem_t element);
void listDelete (List_t* list, ListElement* anchorElement);


size_t listBegin ();
size_t listEnd ();
