#include "chash.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

hashRecord* search(const char* key) {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  // Format the timestamp with seconds and microseconds
  char timestamp[64];
  snprintf(timestamp, sizeof(timestamp), "%ld%05d", tv.tv_sec, tv.tv_usec);

  //Create hash value
  uint32_t hash = jenkins_one_at_a_time_hash(key);
  int waiting = 0;
  //If lock not acquired, print timestamp and what is waiting for
  while(pthread_rwlock_rdlock(&rwlock) != 0){
    waiting = 1;
    //print timestamp & wait
    log_event("%s: WAITING ON INSERTS", timestamp);
    pthread_cond_wait(&cv_insert_done, &cv_mutex);
  }
  //If thread has ever waited, state that it is now awakened
  if(waiting == 1){
    log_event("%s: SEARCH AWAKENED", timestamp);
  }
  log_event("%s,READ LOCK ACQUIRED", timestamp);
  lock_acquisitions++;
  hashRecord *current = head;
  
  //Search through hash table and return current key-data pair if found
  while (current != NULL){
    if(current->hash == hash){
      log_event("%s,SEARCH:%s,%u", timestamp, current->name, current->salary);
      //Unlock read lock
      lock_releases++;
      log_event("%s,READ LOCK RELEASED", timestamp);
      pthread_rwlock_unlock(&rwlock);
      return current;
    }
    current = current->next;
  }

  if(current == NULL){
      log_event("%s,SEARCH: NOT FOUND", timestamp);
  }

  //If not found, Unlock read lock and return NULL
  lock_releases++;
  log_event("%s,READ LOCK RELEASED", timestamp);
  pthread_rwlock_unlock(&rwlock);
  return NULL;
}