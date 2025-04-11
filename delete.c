#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "chash.h"

void delete(const char *key) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    char timestamp[64];
    snprintf(timestamp, sizeof(timestamp), "%ld%05d", tv.tv_sec, tv.tv_usec);

    uint32_t hash = jenkins_one_at_a_time_hash(key);

    int waiting = 0;
    while (pthread_rwlock_wrlock(&rwlock) != 0) {
        waiting = 1;

        pthread_mutex_lock(&cv_mutex);

        log_event("%s: WAITING ON INSERTS", timestamp);

        pthread_cond_wait(&cv_insert_done, &cv_mutex);

        pthread_mutex_unlock(&cv_mutex);
    }

    if (waiting == 1) {
        log_event("%s: DELETE AWAKENED", timestamp);
    }
    log_event("%s,WRITE LOCK ACQUIRED", timestamp);
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

        log_event("%s,DELETE,%s,%u", timestamp, name, salary);

        free(current);
    }

    lock_releases++;
    log_event("%s,WRITE LOCK RELEASED", timestamp);
    pthread_rwlock_unlock(&rwlock);
}