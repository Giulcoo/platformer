#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Location {
    int line, column;
} Location;

typedef struct Error {
    struct Error *next;
    struct Location *pos;
    const char* message;
} Error;

Location *copy_pos(Location pos);
void add_error(Error *errors, Location *pos, const char *message);
void insert_error(Error *errors, Error *error);
void print_errors(Error *errors);
int compare_location(Location *l1, Location *l2);

#endif  // ERROR_H
