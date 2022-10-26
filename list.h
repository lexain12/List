#include <stdlib.h>

enum Errors 
{
    noErrors = 0,

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
    //status
};

void listCtor (List_t* list, size_t size);
void listDtor (List_t* list);

void listDump (List_t* list);

Errors listVerify (List_t* list);

size_t listInsertPrev (List_t* list, Elem_t element, size_t anchorIndex);
size_t listHeadInsert (List_t* list, Elem_t element);
size_t listTailAdd (List_t* list, Elem_t element);
Elem_t listDelete (List_t* list, size_t anchorIndex);


size_t listBegin ();
size_t listEnd ();
