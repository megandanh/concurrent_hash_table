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

// Utility: Log event function
void log_event(const char *format, ...);

// Utility: Jenkins's one-at-a-time hash function
uint32_t jenkins_one_at_a_time_hash(const char *key);

// Inserts or updates a key-data pair in the hash table
void insert_record(const char *name, uint32_t salary);

// Deletes a key-data pair in the hash table if found
void delete(const char *key);

// Searches for key-data pair and returns value if found or NULL if not found
hashRecord* search(const char *key);

// Prints the entire list after all operations, sorted by hash value
void print();

#endif // CHASH_H