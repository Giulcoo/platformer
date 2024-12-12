#include "error.h"

Location *copy_pos(Location pos) {
    Location *loc = malloc(sizeof(Location));
    loc->line = pos.line;
    loc->column = pos.column;
    return loc;
}

void add_error(Error *errors, Location *pos, const char *message){
    Location *loc = copy_pos(*pos);

    if(errors){
        Error *newError = malloc(sizeof(Error));
        newError->pos = loc;
        newError->message = message;
        insert_error(errors, newError);
    }
}

void insert_error(Error *errors, Error *error){
    while(errors->next){
        if(compare_location(errors->pos, error->pos) < 0) break;
        errors = errors->next;
    }

    error->next = errors->next;
    errors->next = error;
}

void print_errors(Error *errors){
    while(errors){
        printf("\nError %d:%d: %s", errors->pos->line, errors->pos->column, errors->message);
        free(errors->pos);
        errors = errors->next; 
    }

    printf("\n");
}

int compare_location(Location *l1, Location *l2){
    if(l1 == NULL){
        return 1;
    }
    if(l1->line < l2->line){
        return -1;
    }
    else if(l1->line > l2->line){
        return 1;
    }
    else if(l1->column < l2->column){
        return -1;
    }
    else if(l1->column > l2->column){
        return 1;
    }
    else{
        return 0;
    }
}