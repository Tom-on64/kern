#include <stdio.h>
#include <string.h>

int evalExpr(char* expr, int* res);
int parseTerm(char** ptr);
int parseFactor(char** ptr);
int parseNumber(char** ptr);

// TODO: Parsing doesn't account for addition/subtraction inside brackets, fix that

__attribute__ ((section("entry")))
void main() {
    printf("Welcome to calc.\n");   
    char input[256];

    while (1) {
        printf("> ");
        read(input);
        *strchr(input, '\n') = '\0';

        // Remove whitespace
        char* old = input;
        char* new = input;
        while (*old != '\0') {
            if (*old == ' ') old++;
            *new++ = *old++;
        }
        *new = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        int res;
        if (evalExpr(input, &res)) {
            printf("%d\n", res);
        } else {
            printf("?\n");
        }
    }

    return;
}

int evalExpr(char* expr, int* res) {
    char* ptr = expr;
    *res = parseTerm(&ptr);

    while (*ptr != '\0') {
        if (*ptr == '+') {
            ptr++;
            *res += parseTerm(&ptr);
        } else if (*ptr == '-') {
            ptr++;
            *res -= parseTerm(&ptr);
        } else {
            return 0; // Invalid
        }
    }

    return 1; // Success
}

int parseTerm(char** ptr) {
    int res = parseFactor(ptr);

    while (**ptr == '*' || **ptr == '/') {
        if (**ptr == '*') {
            (*ptr)++;
            res *= parseFactor(ptr);
        } else if (**ptr == '/') {
            (*ptr)++;
            int div = parseFactor(ptr);
            if (div != 0) {
                res /= div;
            } else {
                return 0;
            }
        }
    }

    return res;
}

int parseFactor(char** ptr) {
    if (**ptr == '(') {
        (*ptr)++;
        int res = parseTerm(ptr);

        if (**ptr == ')') {
            (*ptr)++;
        } else {
            printf("Error: Missing ')'");
            return 0;
        }
        return res;
    } else {
        return parseNumber(ptr);
    }
}
int parseNumber(char** ptr) { 
    int res = 0;
    while (**ptr >= '0' && **ptr <= '9') {
        res *= 10;
        res += *(*ptr)++ - '0';
    }

    return res;
}

