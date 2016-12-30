#include <carutil.h>

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = CAR File List Interface                                     = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

CARList *CARListCreate()
{
    CARList *list = malloc(sizeof(CARList));
    memset(list, 0, sizeof(CARList));
    return list;
}

CAREntry *CAREntryMake(void *data)
{
    CAREntry *entry = malloc(sizeof(CAREntry));
    entry->data = data;
    entry->next = NULL;
    return entry;
}

void CARListDestroy(CARList *list)
{
    CAREntry *entry = list->head, *next = NULL;

    while (entry)
    {
        next = entry->next;
        free(entry);

        entry = next;
    }

    free(list);
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = String Interface                                            = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

CARString *CARStringCreateWithCString(const char *cstring)
{
    size_t length = strlen(cstring);
    CARString *string = malloc(sizeof(CARString));
    string->length = length;
    string->string = calloc((length + 1), sizeof(char));
    strcpy(string->string, cstring);
    return string;
}

void CARStringDestroy(CARString *string)
{
    free(string->string);
    free(string);
}
