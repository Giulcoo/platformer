#ifndef EDITOR_H
#define EDITOR_H

#include <stdio.h>
#include <ctype.h>

char* read_file(const char* filename);
char* replace_placeholder(const char* template, const char* placeholder, const char* replacement);
int count_substring(const char *string, const char *substring);
char* replace_char(const char *string, const char toreplace, const char replacement);
char* concat(const char *s1, const char *s2);
char* int_to_string(int value);
void write_file(const char* filename, const char* content);
char is_alphanumeric(const char *string) ;

#endif // EDITOR_H
