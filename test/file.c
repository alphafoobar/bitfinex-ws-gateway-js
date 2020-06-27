#include <stdio.h>
#include <stdlib.h>
#include "file.h"

char *
read_file(char *filename, size_t file_size) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }
    size_t chunk = (file_size + 1) * sizeof(char);
    char *string = (char *) malloc(chunk);
    size_t result = fread(string, sizeof(char), file_size, file);
    string[result] = '\0';

    if (file_size != result) {
        free(string);
        string = NULL;
    }
    fclose(file);
    return string;
}
