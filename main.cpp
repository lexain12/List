#include <cassert>
#include <stdio.h>
#include <stdarg.h>

typedef int Elem_t;
#include "list.h"
#define Format_ "%-3d"
const int Poison = 0xDEADBEEF;
const size_t SizePoison = 0xDEADBEEF;

extern FILE* LOGFILEPTR;

int listCtor (List_t* list, size_t capacity)
{
    int errors = noErrors;
    ListElement* newDataPtr = (ListElement*) calloc (sizeof(ListElement), capacity + 1);

    if (newDataPtr == nullptr)
        return errors |= listptrError;

    list->data     = newDataPtr;
    list->capacity = capacity;
    list->free     = 1;
    list->size     = 0;
    list->isSorted = listSorted;
    list->status   = noErrors;

    list->data[0].element = Poison;
    list->data[0].prevElementInd = 0;
    list->data[0].nextElementInd = 0;

    fillList (list);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error ctor\n");
    }

    return noErrors;
}

void listDtor (List_t* list)
{

    list->capacity               = 0;
    list->size                   = 0;
    list->free                   = 0;

    list->data[0].element        = 0;
    list->data[0].prevElementInd = 0;
    list->data[0].nextElementInd = 0;

    free(list->data);
    list->data = nullptr;

    return;
}

size_t listHeadAdd (List_t* list, Elem_t element)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listHeadAdd function, element: "Format_"\n", element);
    }

    if (list->size == list->capacity)
    {
        listResizeUp (list);
    }


    return listInsertPrev (list, list->data[0].nextElementInd, element);
}

size_t listTailAdd (List_t* list, Elem_t element)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listTailAdd function, element: "Format_"\n", element);
    }
    
    if (list->size == list->capacity)
    {
        listResizeUp (list);
    }

    return listInsertAfter (list, list->data[0].prevElementInd, element);
}

void myGraph (List_t* list)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in myGraph function, element: \n");
    }

    FILE* GraphFilePtr = fopen(GraphFile, "w");
#define dumpprint(...) fprintf(GraphFilePtr, __VA_ARGS__);

    dumpprint ("digraph MyGraph {\n")
    dumpprint ("    node [color=black, shape=box, style=\"rounded, filled\"];\n")
    dumpprint ("    rankdir=LR;\n")
    dumpprint ("    free [fillcolor=\"#74c2f2\", label=\"free = %lu\"];\n", list->free)
    dumpprint ("    size [fillcolor=\"#74c2f2\", label=\"size = %lu\"];\n", list->size)
    dumpprint ("    capacity [fillcolor=\"#74c2f2\", label=\"capacity = %lu\"];\n", list->capacity)
    dumpprint ("    status [fillcolor=\"#74c2f2\", label=\"isSorted = %s\"];\n", list->isSorted == listSorted ? "yes" : "no")
    
    dumpprint ("    node[color=black, shape=record, style=\"rounded, filled\"];\n")
    dumpprint ("    edge[style = invis, constraint=true];\n")

    dumpprint ("    nd%d [fillcolor=\"#a1a1a1\", label=\"node %d | value: Poison | { <p> prev: %lu | <n> next: %lu  }\"];\n",
                0, 0, list->data[0].prevElementInd, list->data[0].nextElementInd)

    for (size_t index = 1; index <= list->capacity; ++index)
    {
        dumpprint ("    nd%lu [", index)

        if (list->data[index].element == Poison)
            dumpprint ("fillcolor =\"#80f293\", label =\"node %lu | value: Poison", index)

        else if (index == list->data[0].nextElementInd)
            dumpprint ("fillcolor =\"#fcf400\", label =\"node %lu | value:"Format_, index, list->data[index].element)

        else if (index == list->data[0].prevElementInd)
            dumpprint ("fillcolor =\"#fc8b00\", label =\"node %lu | value:"Format_, index, list->data[index].element)

        else 
        {
            dumpprint ("fillcolor =\"#f280f0\", label =\"node %lu | value:"Format_, index, list->data[index].element)
        }
        dumpprint (" | { <p> prev: %lu | <n> next: %lu}\"];", list->data[index].prevElementInd, list->data[index].nextElementInd)
        dumpprint ("    nd%lu -> nd%lu;\n", index - 1, index)
    }
    
    dumpprint("    edge [style=solid, constraint=false];\n")

    for (size_t index = 0; index <= list->capacity; ++index)
    {
        dumpprint("    nd%lu -> nd%lu;\n", index, list->data[index].prevElementInd);
        dumpprint("    nd%lu -> nd%lu;\n\n", index, list->data[index].nextElementInd);
    }

    dumpprint ("    edge [style = bold, constraint=false];\n")
    dumpprint ("    free -> nd%lu;\n", list->free)

    dumpprint ("}\n")

    fclose(GraphFilePtr);
    static int picVersion = 0;

    char buf[100] = "";
    sprintf(buf, "dot -Tsvg GraphFile.txt > src/pic%d.svg", picVersion);
    picVersion++;

    system (buf);
#undef dumpprint
}

void listDump (List_t* list, const char* str, ...)
{
    assert(list != nullptr);
    fprintf(LOGFILEPTR, "<hr>\n");

    va_list argPtr = nullptr;
    va_start (argPtr, str);

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

size_t listInsertPrev (List_t* list, size_t anchorIndex, Elem_t element)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listInsertPrev function, element: "Format_", before: %lu\n", element, anchorIndex);
    }

    assert (list != nullptr);

    if (list->size == list->capacity)
    {
        listResizeUp (list);
    }

    size_t nextFree = list->data[list->free].nextElementInd;
    size_t curFree  = list->free;

    list->data[curFree].element = element;
    list->data[curFree].prevElementInd = list->data[anchorIndex].prevElementInd;

    list->data[list->data[anchorIndex].prevElementInd].nextElementInd = curFree;
    list->data[anchorIndex].prevElementInd = curFree;
    list->data[curFree].nextElementInd = anchorIndex;

    list->free = nextFree;
    list->size += 1;
    list->isSorted = listNotSorted;

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after listInsertPrev function, element: "Format_", before: %lu\n", element, anchorIndex);
    }
    
    return curFree;
}

size_t listInsertAfter (List_t* list, size_t anchorIndex, Elem_t element)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listInsertAfter function, element: "Format_", before: %lu\n", element, anchorIndex);
    }

    assert (list != nullptr);

    if (list->size == list->capacity)
    {
        listResizeUp (list);
    }

    return listInsertPrev (list, list->data[anchorIndex].nextElementInd, element);
}

Elem_t listDelete (List_t* list, size_t anchorIndex)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listDelete function, delete %lu element \n", anchorIndex);
    }

    if (list->data[anchorIndex].prevElementInd == SizePoison)
    {
        listDump (list, "Error in listDelete function, can't delete element %lu \n", anchorIndex);
        return Poison;
    }

    list->data[list->data[anchorIndex].prevElementInd].nextElementInd = list->data[anchorIndex].nextElementInd;
    list->data[list->data[anchorIndex].nextElementInd].prevElementInd = list->data[anchorIndex].prevElementInd;

    Elem_t element = list->data[anchorIndex].element;
    list->data[anchorIndex].element = Poison;
    list->data[anchorIndex].nextElementInd = list->free;
    list->data[anchorIndex].prevElementInd = SizePoison;

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after listDelete function, delete %lu element \n", anchorIndex);
    }

    return element;    
}

int listVerify (List_t* list)
{
    size_t epsiloh = -1;
    epsiloh       /=  2;

    if (list)
    {
        if (list->capacity     > epsiloh)
            list->status |= capacityError;
        
        if (list->size         > epsiloh)
            list->status |= sizeError;

        if (list->size         > list->capacity)
            list->status |= sizeAndCapacityError;

        if (!list->data)
            return list->status |= dataError;
        
    }
    else
        list->status |= listptrError;

    return list->status;
}

int listLinear (List_t* list)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listLinear function\n");
    }

    ListElement* newData = (ListElement*) calloc (list->capacity + 1, sizeof(ListElement));
    if (newData == nullptr) 
        return list->status |= memAllocError;
 
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
    list->data[0].prevElementInd = index1 - 1;
    list->isSorted = listSorted;

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after listLinear function\n");
    }

    return noErrors;
}

size_t logicalNumberToPhysical (List_t* list, size_t anchorIndex)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in logicalNumberToPhysical function, logicalNumber %lu\n", anchorIndex);
    }
    if (list->isSorted == listSorted)
    {
        return anchorIndex;
    }

    size_t index  = 0;
    size_t index1 = 0;
    while (index1 != anchorIndex)
    {
        index = list->data[index].nextElementInd;
        index1++;
    }
    
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after logicalNumberToPhysical function, logicalNumber %lu\n", anchorIndex);
    }

    return index;
}

size_t findElementByValue (List_t* list, Elem_t value)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in findElementByValue function, value "Format_"\n", value);
    }

    size_t index  = 0;
    while (list->data[index].element != value)
    {
        index = list->data[index].nextElementInd;
    }
    
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after findElementByValue function, value "Format_"\n", value);
    }

    return index;
}

void fillList (List_t* list)
{
    size_t index = list->free;
    for (; index <= list->capacity; ++index)
    {
        list->data[index].element = Poison;
        list->data[index].prevElementInd = SizePoison;
        list->data[index].nextElementInd = index + 1;
    }

    list->data[index - 1].nextElementInd = 0;
}

int listResizeUp (List_t* list)
{
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listResizeUp function\n");
    }

    ListElement* newData = (ListElement*) realloc (list->data, list->capacity * 4 * sizeof(ListElement));
    if (newData == nullptr)
        return listResizeUpError;

    list->data      = newData;
    list->free = list->capacity + 1;
    list->capacity *= 2;

    fillList (list);

    if (list->status |= listVerify (list))
    {
        listDump (list, "Error after listResizeUp function\n");
    }

    return noErrors;
}

int listResizeDown (List_t* list, size_t newCapacity)
{
    int errors = 0;
    if (list->status |= listVerify (list))
    {
        listDump (list, "Error in listResizeDown function\n");
    }

    
    if (newCapacity < list->size || list->isSorted == listNotSorted)
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
    listTailAdd (&list1, 5);
    listTailAdd (&list1, 5);
    listDump (&list1, "1\n");
    listInsertPrev (&list1, 3, 2);
    listInsertPrev (&list1, 2, 1);
    listDelete (&list1, 5);
    listDump (&list1, "asdf\n");

    listDtor (&list1);

    return 0;
}
