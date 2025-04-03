#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chash.h"

void delete(const char *key) {
    uint32_t hash = jenkins_one_at_a_time_hash(key);

    int waiting = 0;
    while (pthread_rwlock_wrlock(&rwlock) != 0) {
        waiting = 1;

        pthread_mutex_lock(&cv_mutex);

        log_event("%ld: WAITING ON INSERTS", time(NULL));

        pthread_cond_wait(&cv_insert_done, &cv_mutex);

        pthread_mutex_unlock(&cv_mutex);
    }

    if (waiting == 1) {
        log_event("%ld: DELETE AWAKENED", time(NULL));
    }
    
    log_event("%ld,WRITE LOCK ACQUIRED", time(NULL));
    lock_acquisitions++;

    hashRecord *prev = NULL;
    hashRecord *current = head;

    while (current) {
        if (current->hash == hash) break;

        prev = current;
        current = current->next;
    }

    if (current) {
        if (prev) {
            prev->next = current->next;
        } else {
            head = current->next;
        }

        char *name = current->name;
        uint32_t salary = current->salary;

        log_event("%ld,DELETE,%s,%u", time(NULL), name, salary);

        //free(current->name);
        free(current);
    }

    pthread_rwlock_unlock(&rwlock);
    lock_releases++;
    log_event("%ld,WRITE LOCK RELEASED", time(NULL));
}