#include <iostream>
#include <vector>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

using std::string;
using std::vector;

typedef struct {
    uint32_t checksum;
    vector<string> matches;
} ilist_entry;

typedef struct {
    uint32_t checksum;
    string file;
} list_entry;

int compare(const void *v1, const void *v2)
{
    uint32_t av = **(uint32_t **)v1;
    uint32_t bv = **(uint32_t **)v2;
    
    if (av < bv) {
        return -1;
    } else if (av > bv) {
        return +1;
    } else {
        return 0;
    }
}

vector<ilist_entry *> findMatches(list_entry **list, size_t size)
{
    vector<ilist_entry *> matches;
    qsort((void *)list, size, sizeof(list_entry *), compare);

    // Find runs in the list
    for (size_t i = 0; i < size; i++)
    {
        size_t j = i;
        while (j != (size - 1) && list[j]->checksum == list[j + 1]->checksum) j++;
        size_t runlength = (j - i) + 1;

        if (runlength > 1)
        {
            ilist_entry *matchentry = new ilist_entry;
            matchentry->checksum = list[i]->checksum;

            matchentry->matches = vector<string>(runlength);
            for (size_t k = 0; k < runlength; k++) matchentry->matches[k] = list[i + k]->file;
            matches.push_back(matchentry);
            i += (runlength - 1);
        }
    }

    return matches;
}
