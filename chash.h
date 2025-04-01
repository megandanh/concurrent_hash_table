#ifndef CHASH_H
#define CHASH_H

#include <stdint.h>
#include <pthread.h>
#include <stdarg.h>

typedef struct hash_struct
{
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;


// Head of the linked list (the hash table)
extern hashRecord *head;

// Reader-writer lock for synchronizing access to the hash table
extern pthread_rwlock_t rwlock;

// Mutex and condition variable for coordinating thread execution 
extern pthread_mutex_t cv_mutex;
extern pthread_cond_t cv_insert_done;

// Global counters for lock acquisition 
extern int lock_acquisitions;
extern int lock_releases;

// Function Prototypes

// Utility: Jenkins's one-at-a-time hash function
uint32_t jenkins_one_at_a_time_hash(const char *key);

// Inserts or updates a key-data pair in the hash table
void insert_record(const char *name, uint32_t salary);

#endif // CHASH_H