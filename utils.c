#include "chash.h"
#include <stdio.h>
#include <stdarg.h>


// Global file pointer for output (make sure to open it in your main function)
extern FILE *output_file;

void log_event(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Write to the output file
    vfprintf(output_file, format, args);
    fprintf(output_file, "\n");
    
    va_end(args);
}

uint32_t jenkins_one_at_a_time_hash(const char *key) {
    uint32_t hash = 0;
    while (*key) {
        hash += (unsigned char)(*key++);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

