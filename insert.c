#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chash.h"

void insert_record(const char *name, uint32_t salary) {
    // Compute hash using Jenkins's one-at-a-time hash function
    uint32_t hash_val = jenkins_one_at_a_time_hash(name);

    // Log before acquiring the write lock
    log_event("%ld,WRITE LOCK ACQUIRED", time(NULL));

    // Acquire the write lock to modify the hash table
    pthread_rwlock_wrlock(&rwlock);
    lock_acquisitions++;

    // Search for an existing record with the same name (and hash)
    hashRecord *curr = head;

    // Loop through linked list to search for name 
    while (curr != NULL) {
        if (curr->hash == hash_val && strcmp(curr->name, name) == 0) {
            // If found, update the salary
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

    // Record not found; create a new node and allocate space
    hashRecord *newRecord = (hashRecord *)malloc(sizeof(hashRecord));
    
    // Space wasn't properly allocated
    if (newRecord == NULL) {
        perror("malloc");
        pthread_rwlock_unlock(&rwlock);
        lock_releases++;
        return;
    }

    // Set values for node
    newRecord->hash = hash_val;
    strncpy(newRecord->name, name, sizeof(newRecord->name) - 1);
    newRecord->name[sizeof(newRecord->name) - 1] = '\0';  // Ensure null-termination
    newRecord->salary = salary;
    
    // Insert new node at the beginning of the list
    newRecord->next = head;
    head = newRecord;

    // Log the insert command
    log_event("%ld,INSERT,%s,%u", time(NULL), name, salary);

    // Release the write lock
    pthread_rwlock_unlock(&rwlock);
    lock_releases++;
    log_event("%ld,WRITE LOCK RELEASED", time(NULL));
}
