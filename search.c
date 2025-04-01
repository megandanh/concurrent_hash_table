#include "hash.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Reader-writer lock for synchronizing access to the hash table
extern pthread_rwlock_t rwlock;

// Head of the linked list (the hash table)
extern hashRecord *head;

// Global counters for lock acquisition 
extern int lock_acquisitions;
extern int lock_releases;

// Mutex and condition variable for coordinating thread execution 
extern pthread_mutex_t cv_mutex;
extern pthread_cond_t cv_insert_done;

hashRecord* search(const char* key) {
  //Create hash value
  uint32_t hash = jenkins_one_at_a_time_hash(key);
  int waiting = 0;
  time_t now = time(NULL);
  //If lock not acquired, print timestamp and what is waiting for
  while(pthread_rwlock_rdlock(rwlock) != 0){
    waiting = 1;
    //print timestamp & wait
    printf("%ld: WAITING ON INSERTS\n", now);
    pthread_cond_wait(&cv_insert_done, cv_mutex);
  }
  //If thread has ever waited, state that it is now awakened
  if(waiting == 1){
    printf("%ld: SEARCH AWAKENED\n", now);
  }
  printf("%ld: READ LOCK ACQUIRED", now);
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
  printf("%ld: READ LOCK RELEASED", now);
  pthread_rwlock_unlock(rwlock);
  lock_releases++;
  return NULL;
}
