#include "editor.h"
#include <stdlib.h>
#include <string.h>

char* read_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory to store the file content
    char *content = malloc(fileSize + 1);
    if (content == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read file content and close the file
    fread(content, 1, fileSize, file);
    content[fileSize] = '\0';
    fclose(file);
    return content;
}

char* replace_placeholder(const char* template, const char* placeholder, const char* replacement) {
    char *pos = strstr(template, placeholder);
    if (!pos) return strdup(template);  // Key not found, return a copy of the original text

    size_t newLength = strlen(template) - strlen(placeholder) + strlen(replacement);
    char *newText = (char*)malloc(newLength + 1);
    if (!newText) return NULL;

    // Copy parts before and after the key
    strncpy(newText, template, pos - template);  // Copy up to the position of the key
    newText[pos - template] = '\0';          // Null-terminate after the first part
    strcat(newText, replacement);        // Append the replacement
    strcat(newText, pos + strlen(placeholder));  // Append the rest of the original text after the key

    return newText;
}

int count_substring(const char *string, const char *substring) {
    int count = 0;
    const char *temp = string;

    while ((temp = strstr(temp, substring)) != NULL) {
        count++;
        temp += strlen(substring); 
    }

    return count;
}

char* replace_char(const char *string, const char toreplace, const char replacement) {
    char *result = (char*)malloc(strlen(string) + 1);
    if (!result) return NULL; 

    int i;
    for (i = 0; string[i] != '\0'; i++) {
        if (string[i] != toreplace){
            result[i] = string[i];
        }
        else{
            result[i] = replacement;
        }
    }
    result[i] = '\0';
    return result;
}

char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1); 
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* int_to_string(int value) {
    int length = snprintf(NULL, 0, "%d", value);
    
    char *result = (char*)malloc(length + 1);
    if (!result) return NULL; 
    
    snprintf(result, length + 1, "%d", value);
    return result; 
}

void write_file(const char* filename, const char* content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }
    fputs(content, file);
    fclose(file);
}

char is_alphanumeric(const char *string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!isalnum((unsigned char)string[i])) {
            return 0; 
        }
    }
    return 1;
}
