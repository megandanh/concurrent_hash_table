#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chash.h"

void insert_record(const char *name, uint32_t salary) {
    // Find hash 
    uint32_t hash_val = jenkins_one_at_a_time_hash(name);

    pthread_rwlock_wrlock(&rwlock);
    lock_acquisitions++;

    log_event("%ld,WRITE LOCK ACQUIRED", time(NULL));

    // Loop to find a pre-existing record with the same name 
    hashRecord *curr = head;
    while (curr != NULL) {
        // If found
        if (curr->hash == hash_val && strcmp(curr->name, name) == 0) {
            // Update the salary
            curr->salary = salary;
            log_event("%ld,INSERT,%s,%u", time(NULL), name, salary);
            pthread_rwlock_unlock(&rwlock);
            lock_releases++;
            log_event("%ld,WRITE LOCK RELEASED", time(NULL));
            return;
        }

        // Go to next node if not found
        curr = curr->next;
    }

    // Create a new node and allocate space
    hashRecord *newRecord = (hashRecord *)malloc(sizeof(hashRecord));
    
    // Space wasn't properly allocated
    if (newRecord == NULL) {
        perror("malloc");
        pthread_rwlock_unlock(&rwlock);
        lock_releases++;
        return;
    }

    // Set up new node
    newRecord->hash = hash_val;
    strncpy(newRecord->name, name, sizeof(newRecord->name) - 1);
    newRecord->name[sizeof(newRecord->name) - 1] = '\0';  // Ensure null-termination
    newRecord->salary = salary;
    newRecord->next = head;
    head = newRecord;

    log_event("%ld,INSERT,%s,%u", time(NULL), name, salary);

    // Release the write lock
    pthread_rwlock_unlock(&rwlock);
    lock_releases++;
    log_event("%ld,WRITE LOCK RELEASED", time(NULL));
}
