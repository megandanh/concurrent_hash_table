#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "chash.h"

// Global variable definitions
hashRecord *head = NULL;
pthread_rwlock_t rwlock;
pthread_mutex_t cv_mutex;
pthread_cond_t cv_insert_done;
int lock_acquisitions = 0;
int lock_releases = 0;
FILE *output_file = NULL;

// Structure to pass parameters to each thread
typedef struct {
    char command[10];  // Expecting "insert" (other commands might be here too)
    char name[50];
    uint32_t salary;
} CommandArgs;

// Thread function to handle commands (only insert in your case)
void *handle_command(void *args) {
    CommandArgs *cmd = (CommandArgs *)args;
    if (strcmp(cmd->command, "insert") == 0) {
        insert_record(cmd->name, cmd->salary);
    } else if (strcmp(cmd->command, "delete") == 0) {
        delete(cmd->name);
    }
    // If your role were expanded, you could add else if branches for delete/search etc.
    return NULL;
}

int main(void) {
    // Initialize locks and condition variable
    pthread_rwlock_init(&rwlock, NULL);
    pthread_mutex_init(&cv_mutex, NULL);
    pthread_cond_init(&cv_insert_done, NULL);

    // Open output file for logging
    output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("Failed to open output.txt");
        exit(EXIT_FAILURE);
    }

    // Open the commands.txt file
    FILE *cmdFile = fopen("commands.txt", "r");
    if (cmdFile == NULL) {
        perror("Failed to open commands.txt");
        exit(EXIT_FAILURE);
    }

    // Read the first line to determine the number of threads/commands
    char line[256];
    int threadCount = 0;
    if (fgets(line, sizeof(line), cmdFile)) {
        sscanf(line, "threads,%d,0", &threadCount);
    } else {
        fprintf(stderr, "commands.txt is empty or formatted incorrectly.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate arrays for thread handles and command arguments
    pthread_t *threads = malloc(threadCount * sizeof(pthread_t));
    CommandArgs *commands = malloc(threadCount * sizeof(CommandArgs));
    if (!threads || !commands) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // Read each command (assuming they are all "insert" for your part)
    int index = 0;
    while (fgets(line, sizeof(line), cmdFile) != NULL && index < threadCount) {
        // For an insert command: "insert,Name,Salary"
        char cmd[10], name[50];
        uint32_t salary;
        sscanf(line, "%[^,],%[^,],%u", cmd, name, &salary);
        strcpy(commands[index].command, cmd);
        strcpy(commands[index].name, name);
        commands[index].salary = salary;
        index++;
    }
    fclose(cmdFile);

    // Create threads to process each insert command
    for (int i = 0; i < threadCount; i++) {
        if (pthread_create(&threads[i], NULL, handle_command, (void *)&commands[i]) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }

    // Free memory, and close file
    pthread_rwlock_destroy(&rwlock);
    pthread_mutex_destroy(&cv_mutex);
    pthread_cond_destroy(&cv_insert_done);
    fclose(output_file);
    free(threads);
    free(commands);

    return 0;
}
