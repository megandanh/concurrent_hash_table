#include "chash.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

hashRecord* search(const char* key) {
  //Create hash value
  uint32_t hash = jenkins_one_at_a_time_hash(key);
  int waiting = 0;
  time_t now = time(NULL);
  //If lock not acquired, print timestamp and what is waiting for
  while(pthread_rwlock_rdlock(&rwlock) != 0){
    waiting = 1;
    //print timestamp & wait
    log_event("%ld: WAITING ON INSERTS", now);
    pthread_cond_wait(&cv_insert_done, &cv_mutex);
  }
  //If thread has ever waited, state that it is now awakened
  if(waiting == 1){
    log_event("%ld: SEARCH AWAKENED", now);
  }
  log_event("%ld,READ LOCK ACQUIRED", now);
  lock_acquisitions++;
  hashRecord *current = head;
  
  //Search through hash table and return current key-data pair if found
  while (current != NULL){
    if(current->hash == hash){
      //Unlock read lock
      pthread_rwlock_unlock(&rwlock);
      lock_releases++;
      log_event("%ld,READ LOCK RELEASED", now);
      return current;
    }
    current = current->next;
  }

  if(current == NULL){
      log_event("%ld,SEARCH: NOT FOUND", now);
  }
  else
  {
      log_event("%ld,SEARCH:%s,%u", now, current->name, current->salary);
  }

  //If not found, Unlock read lock and return NULL
  log_event("%ld,READ LOCK RELEASED", now);
  pthread_rwlock_unlock(&rwlock);
  lock_releases++;
  return NULL;
}
