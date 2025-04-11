#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "chash.h"

void insert_record(const char *name, uint32_t salary) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Format the timestamp with seconds and microseconds
    char timestamp[64];
    snprintf(timestamp, sizeof(timestamp), "%ld%05d", tv.tv_sec, tv.tv_usec);

    // Compute hash using Jenkins's one-at-a-time hash function
    uint32_t hash_val = jenkins_one_at_a_time_hash(name);

    pthread_rwlock_wrlock(&rwlock);
    log_event("%s,WRITE LOCK ACQUIRED", timestamp);
    lock_acquisitions++;

    // Search for an existing record with the same name (and hash)
    hashRecord *curr = head;
    while (curr != NULL) {
        // If found
        if (curr->hash == hash_val && strcmp(curr->name, name) == 0) {
            // If found, update the salary
            log_event("%s,INSERT,%s,%u", timestamp, name, salary);
            curr->salary = salary;

            // Release the write lock
            lock_releases++;
            log_event("%s,WRITE LOCK RELEASED", timestamp);
            pthread_rwlock_unlock(&rwlock);

            return;
        }
        curr = curr->next;
    }

    // Create a new node and allocate space
    hashRecord *newRecord = (hashRecord *)malloc(sizeof(hashRecord));
    if (newRecord == NULL) {
        perror("malloc");

        lock_releases++;
        pthread_rwlock_unlock(&rwlock);

        return;
    }

    // Set values for the new node
    newRecord->hash = hash_val;
    strncpy(newRecord->name, name, sizeof(newRecord->name) - 1);
    newRecord->name[sizeof(newRecord->name) - 1] = '\0';  // Ensure null-termination
    newRecord->salary = salary;

    // Insert the new node at the beginning of the list
    newRecord->next = head;
    head = newRecord;

    // Log the insert command
    log_event("%s,INSERT,%s,%u", timestamp, name, salary);

    // Release the write lock
    lock_releases++;
    log_event("%s,WRITE LOCK RELEASED", timestamp);
    pthread_rwlock_unlock(&rwlock);
}