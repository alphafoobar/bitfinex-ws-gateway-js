#include <errno.h>

#include <libwebsockets.h>

#include "strings.h"

double
string_to_double(char *string) {
    char *endptr;
    double d = strtold(string, &endptr);
    if (d == 0) {
        /* If a conversion error occurred, display a message and exit */
        if (errno == EINVAL) {
            lwsl_err("Conversion error occurred: %d.\n", errno);
        }

        if (errno == ERANGE) {
            lwsl_err("The value provided was out of range.\n");
        }
    }
    return d;
}
