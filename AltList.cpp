#include <cassert>
#include <stdio.h>
#include <stdarg.h>
#include <cstddef>

typedef int Elem_t;
#define Format_ "%-3d"
#include "AltList.h"

const int Poison        = 0xDEADBEEF;
const size_t SizePoison = 0xDEADBEEF;

extern FILE* LOGFILEPTR;

int listCtor (List_t* list)
{
    assert (list != nullptr);

    int errors = noErrors;
    ListElement* newDataPtr = (ListElement*) calloc (1, sizeof(*newDataPtr));

    if (newDataPtr == nullptr)
        return errors |= listptrError;

    list->nullElement = newDataPtr;
    list->size        = 1;
    list->status      = noErrors;

    list->nullElement->element = Poison;
    list->nullElement->prevElementInd = newDataPtr;
    list->nullElement->nextElementInd = newDataPtr;

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error ctor\n");
    }

    return noErrors;
}

void listDtor (List_t* list)
{
    assert (list != nullptr);

    list->size     = 0;

    list->nullElement->element        = 0;
    list->nullElement->prevElementInd = 0;
    list->nullElement->nextElementInd = 0;

    free(list->nullElement);
    list->nullElement = nullptr;

    return;
}

ListElement* listInsertPrev (List_t* list, ListElement* anchorElement, Elem_t element)
{
    assert (list != nullptr);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listInsertPrev function, element: "Format_", before: %lu\n", element, anchorElement);
    }

    ListElement* newListElement = (ListElement*) calloc (1, sizeof (ListElement));
    assert (newListElement != nullptr);

    if (newListElement == nullptr)
        return nullptr;

    newListElement->element = element;
    newListElement->prevElementInd = anchorElement->prevElementInd;
    newListElement->nextElementInd = anchorElement;

    anchorElement->prevElementInd->nextElementInd = newListElement;
    anchorElement->prevElementInd = newListElement;

    list->size += 1;
    printf("%lu\n", list->size);
    printf("%lu\n", list->size);
    printf("%lu\n", list->size);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after listInsertPrev function, element: "Format_", before: %lu\n", element, anchorElement);
    }
    
    return newListElement;
}

ListElement* listHeadAdd (List_t* list, Elem_t element)
{
    assert (list != nullptr);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listHeadAdd function, element: "Format_"\n", element);
    }

    return listInsertPrev (list, list->nullElement->nextElementInd, element);
}

ListElement* listTailAdd (List_t* list, Elem_t element)
{
    assert (list != nullptr);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listTailAdd function, element: "Format_"\n", element);
    }

    return listInsertAfter (list, list->nullElement->prevElementInd, element);
}

ListElement* listInsertAfter (List_t* list, ListElement* anchorIndex, Elem_t element)
{
    if (list->status |= listVerify (list))
        listDump (list, "Error in listInsertAfter function, element: "Format_", before: %lu\n", element, anchorIndex);
    {
    }

    assert (list != nullptr);

    return listInsertPrev (list, anchorIndex->nextElementInd, element);
}

void myGraph (List_t* list)
{
    assert (list != nullptr);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in myGraph function, element: \n");
    }

    FILE* GraphFilePtr = fopen(GraphFile, "w");
    assert (GraphFilePtr != nullptr);
#define dumpprint(...) fprintf(GraphFilePtr, __VA_ARGS__);

    dumpprint ("digraph MyGraph {\n")
    dumpprint ("    node [color=black, shape=box, style=\"rounded, filled\"];\n")
    dumpprint ("    rankdir=LR;\n")
    dumpprint ("    size [fillcolor=\"#74c2f2\", label=\"size = %lu\"];\n", list->size)
    
    dumpprint ("    node[color=black, shape=record, style=\"rounded, filled\"];\n")
    dumpprint ("    edge[style = invis, constraint=true];\n")

    dumpprint ("    nd%p [fillcolor=\"#a1a1a1\", label=\"node %p | value: Poison | { <p> prev: %p | <n> next: %p  }\"];\n",
                list->nullElement, list->nullElement, list->nullElement->prevElementInd, list->nullElement->nextElementInd)

    ListElement* curElement = list->nullElement->nextElementInd;

    for (size_t index = 1; index <= list->size; ++index)
    {

        dumpprint ("    nd%p [", curElement)

        if (curElement->element == Poison)
            dumpprint ("fillcolor =\"#80f293\", label =\"node %p | value: Poison", curElement)

        else if (curElement == list->nullElement->nextElementInd)
            dumpprint ("fillcolor =\"#fcf400\", label =\"node %p | value:"Format_, curElement, curElement->element)

        else if (curElement == list->nullElement->prevElementInd)
            dumpprint ("fillcolor =\"#fc8b00\", label =\"node %p | value:"Format_, curElement, curElement->element)

        else 
        {
            dumpprint ("fillcolor =\"#f280f0\", label =\"node %p | value:"Format_, curElement, curElement->element)
        }
        dumpprint (" | { <p> prev: %p | <n> next: %p}\"];", curElement->prevElementInd, curElement->nextElementInd)
        dumpprint ("    nd%p -> nd%p;\n", curElement->prevElementInd, curElement)

        curElement = curElement->nextElementInd;
    }
    
    dumpprint("    edge [style=solid, constraint=false];\n")

        curElement = list->nullElement;

    for (size_t index = 0; index <= list->size; ++index)
    {
        if (curElement->prevElementInd != nullptr)
        {
            dumpprint("    nd%p -> nd%p;\n", curElement, curElement->prevElementInd);
        }
        dumpprint("    nd%p -> nd%p;\n\n", curElement, curElement->nextElementInd);

        curElement = curElement->nextElementInd;
    }

    dumpprint ("    edge [style = bold, constraint=false];\n")

    dumpprint ("}\n")

    fclose(GraphFilePtr);
    static int picVersion = 0;

    char buf[100] = "";
    sprintf(buf, "dot -Tsvg GraphFileAlt.txt > src/pic%d.svg", picVersion);
    picVersion++;

    system (buf);
#undef dumpprint
}

void listDump (List_t* list, const char* str, ...)
{
    assert(list != nullptr);

    va_list argPtr = nullptr;
    va_start (argPtr, str);

    fprintf(LOGFILEPTR, "<hr>\n");

    fprintf (LOGFILEPTR, "<h2>");
    vfprintf (LOGFILEPTR, str, argPtr);
    fprintf (LOGFILEPTR, "</h2>\n");

    fprintf (LOGFILEPTR, "<h2>Status:</h2>\n");
    
if (list->status)
    {
        fprintf(LOGFILEPTR, "<h2>(ERROR: %d)</h2>\n", list->status);
        fprintf(LOGFILEPTR, "-----------------Errors-------------------\n");
    #define ErrorPrint(error, text)                                       \
                if (error & list->status)                                       \
                {                                                         \
                    fprintf(LOGFILEPTR, "ERROR ["#error"] " #text "\n"); \
                }

        ErrorPrint(noErrors,             No errors);
        ErrorPrint(listptrError,          Wrong ptr on structure with list);
        ErrorPrint(capacityError,        Wrong capacity);
        ErrorPrint(dataError,            No pointer on data (list with elements));
        ErrorPrint(sizeError,            Bad Size of list);
        ErrorPrint(sizeAndCapacityError, Size bigger than capacity => problem with list size)
        ErrorPrint(listResizeUpError,    Cannot resizeUp the list);
        ErrorPrint(memAllocError,        Cannot allocate memory);
        ErrorPrint(listResizeDownError,  Cannot resize down);
        fprintf(LOGFILEPTR, "-------------End-of-errors----------------\n");
    }
    else
        fprintf(LOGFILEPTR, "<h2> (no errors) </h2>\n");
    
    myGraph (list);
    static int picVersion = 0;
    fprintf(LOGFILEPTR, "<img src = \"src/pic%d.svg\"/>\n", picVersion++);

    return;
}


void listDelete (List_t* list, ListElement* anchorElement)
{
    assert (list != nullptr);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listDelete function, delete %lu element \n", anchorElement);
    }

    if (anchorElement->prevElementInd == nullptr)
    {
        listDump (list, "Error in listDelete function, can't delete element %lu \n", anchorElement);
        return;
    }

    anchorElement->prevElementInd->nextElementInd = anchorElement->nextElementInd;
    anchorElement->nextElementInd->prevElementInd = anchorElement->prevElementInd;

    anchorElement->element        = Poison;
    anchorElement->nextElementInd = nullptr;
    anchorElement->prevElementInd = nullptr;

    list->size--;
    free (anchorElement);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after listDelete function, delete %lu element \n", anchorElement);
    }

    return;    
}

int listVerify (List_t* list)
{
    assert (list != nullptr);

    size_t epsiloh = -1;
    epsiloh       /=  2;

    if (list)
    {
        if (list->size         > epsiloh)
            list->status |= sizeError;

        if (!list->nullElement)
            return list->status |= dataError;
    }
    else
        list->status |= listptrError;

    return list->status;
}

size_t logicalNumberToPhysical (List_t* list, ListElement* anchorElement)
{
    assert (list != nullptr);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in logicalNumberToPhysical function, logicalNumber %lu\n", anchorElement);
    }

    size_t index  = 0;
    ListElement* curElement = list->nullElement->nextElementInd;
    while (curElement != anchorElement)
    {
        curElement = curElement->nextElementInd;
        index++;
    }
    
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after logicalNumberToPhysical function, logicalNumber %lu\n", anchorElement);
    }

    return index;
}

ListElement* findElementByValue (List_t* list, Elem_t value)
{
    assert (list != nullptr);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in findElementByValue function, value "Format_"\n", value);
    }

    ListElement* curElement = list->nullElement->nextElementInd;

    while (curElement->element != value)
    {
        curElement = curElement->nextElementInd;
    }
    
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after findElementByValue function, value "Format_"\n", value);
    }

    return curElement;
}

int main()
{
    List_t list1 = {};

    listCtor (&list1);

    listTailAdd (&list1, 5);
    printf("%lu\n", list1.size);
    listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    ListElement* element = listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    listDump (&list1, "Added 5\n");
    listInsertPrev (&list1, element, 4);
    listDump (&list1, "inserPrev %p\n", element);
    listDelete (&list1, element);
    listDump (&list1, "delete %p\n", element);

    listDtor (&list1);

    return 0;
}
