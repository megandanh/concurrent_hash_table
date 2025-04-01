#include "hash.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

// Reader-writer lock for synchronizing access to the hash table
extern pthread_rwlock_t rwlock;

// Head of the linked list (the hash table)
extern hashRecord *head;

// Global counters for lock acquisition 
extern int lock_acquisitions;
extern int lock_releases;

hashRecord* search(const char* key) {
  //Create hash value
  uint32_t hash = jenkins_one_at_a_time_hash(key);
  //If lock not acquired, print timestamp and what is waiting for
  if(pthread_rwlock_rdlock(rwlock) != 0){
  
  }
  lock_acquisitions++;
  
  //Search through hash table 
  while (current != NULL){
    if(current->hash == hash){
      //Unlock read lock
      pthread_rwlock_unlock(rwlock);
      lock_releases++;
      return current;
    }
    current = current->next;
  }

  //Unlock read lock
  pthread_rwlock_unlock(rwlock);
  lock_releases++;
  return NULL;
}
