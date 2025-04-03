#include <stdio.h>
#include <stdlib.h>
#include "chash.h"

int compare(const void *a, const void *b) {
    hashRecord *recordA = *(hashRecord **)a;
    hashRecord *recordB = *(hashRecord **)b;

    if (recordA->hash < recordB->hash) return -1;
    if (recordA->hash > recordB->hash) return 1;
    return 0;
}

int sort(hashRecord ***list) {
    hashRecord *current = head;
    int count = 0;

    while (current) {
        count++;
        *list = realloc(*list, count * sizeof(hashRecord *));
        if (!list) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        (*list)[count - 1] = current;
        current = current->next;
    }

    qsort(*list, count, sizeof(hashRecord *), compare);

    return count;
}

void print() {
    log_event("");
    log_event("Number of lock acquisitions: %d", lock_acquisitions);
    log_event("Number of lock releases: %d", lock_releases);
    
    hashRecord **list = NULL;
    int len = sort(&list);

    for (int i = 0; i < len; i++) {
        log_event("%u,%s,%u", list[i]->hash, list[i]->name, list[i]->salary);
    }
}