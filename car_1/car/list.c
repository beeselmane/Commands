#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"

FList *FListCreate(void)
{
    FList *list = malloc(sizeof(FList));
    list->head = list->tail = NULL;
    list->size = 0;
    return list;
}

bool FListAddDirectory(FList *list, const char *directory)
{
    FListAddEntry(list, (char *)directory, kFLEntryTypeDirectory, 0);
    if (access(directory, X_OK)) return false;
    DIR *dir = opendir(directory);
    struct dirent *dirent = NULL;

    while ((dirent = readdir(dir)))
    {
        if (dirent->d_name[0] == '.' && dirent->d_name[1] == '.' && dirent->d_namlen == 2) continue;
        if (dirent->d_name[0] == '.' && dirent->d_namlen == 1) continue;

        char *entrypath; asprintf(&entrypath, "%s/%s", directory, dirent->d_name);
        struct stat stats;

        if (access(entrypath, R_OK))
        {
            free(entrypath);
            closedir(dir);

            return false;
        }

        if (lstat(entrypath, &stats))
        {
            free(entrypath);
            closedir(dir);

            return false;
        }

        if (S_ISDIR(stats.st_mode)) {
            if (!FListAddDirectory(list, entrypath))
            {
                free(entrypath);
                closedir(dir);
                return false;
            }
        } else if (S_ISREG(stats.st_mode)) {
            FListAddEntry(list, entrypath, kFLEntryTypeRegular, stats.st_size);
        } else if (S_ISLNK(stats.st_mode)) {
            char linkbuffer[256];
            ssize_t linksize = readlink(entrypath, linkbuffer, 256);

            if (linksize <= 0)
            {
                free(entrypath);
                closedir(dir);
                return false;
            }

            FListAddEntry(list, entrypath, kFLEntryTypeSymlink, linksize);
        } else {
            fprintf(stderr, "Warning: File '%s' is of unknown type\n", entrypath);
            free(entrypath);
        }
    }

    closedir(dir);
    return true;
}

void FListAddEntry(FList *list, char *name, FLEntryType type, size_t size)
{
    FListEntry *entry = malloc(sizeof(FListEntry));
    entry->name = name;
    entry->type = type;
    entry->size = size;
    entry->next = NULL;

    if (list->tail) {
        entry->prev = list->tail;
        list->tail->next = entry;
        list->tail = entry;
    } else {
        list->head = list->tail = entry;
        entry->prev = NULL;
    }

    list->size++;
}

void FListDestroy(FList *list)
{
    FListEntry *entry = list->head;

    while (entry != list->tail->next)
    {
        if (entry != list->head) free(entry->name);
        free(entry);

        entry = entry->next;
    }

    free(list);
}
