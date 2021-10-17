#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define strdup _strdup
#define IN_FILE "words.txt"
#define OUT_FILE "out.txt"
#define MAX_LEN 101

void deallocate(char**, int);
int import_matrix(char***, char[]);
void order(char***, int);
void out(char**, int, char[]);

int main() {

    char **matrix = NULL;
    int n;

    n = import_matrix(&matrix, IN_FILE);
    if (!n){
        fprintf(stdout, "\nZero entries in the file.");
        exit(EXIT_SUCCESS);
    } else if (n < 0){
        fprintf(stderr, "\nError %d has occurred during the matrix importation. Abort.", n);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "\n%d entries found.", n);

    order(&matrix, n);
    out(matrix, n, OUT_FILE);
    deallocate(matrix, n);

    exit(EXIT_SUCCESS);
}

void deallocate(char **matrix, int n){

    char **ptr = matrix;

    for (int i = 0; i < n; ++i, ptr++) {
        free(*ptr);
    }
    free(matrix);
}

void out(char **matrix, int n, char filename[]){

    FILE *fp;

    if (fopen_s(&fp, filename, "w"))
        return;

    fprintf(fp, "%d\n", n);
    fprintf(stdout, "\n\n%d\n", n);

    for (int i = 0; i < n; ++i, matrix++) {
        fprintf(fp, "%s\n", *matrix);
        fprintf(stdout, "%s\n", *matrix);
    }

    fclose(fp);
}

int import_matrix(char ***matrix, char filename[]){

    int n, i;
    FILE *fp;
    char **ptr;
    char tmp[MAX_LEN];

    if (fopen_s(&fp, filename, "r"))
        return -1;

    if (fscanf_s(fp, "%d%*c", &n) != 1)
        return -2;

    if (n <= 0)
        return 0;

    ptr = realloc(*matrix, n * sizeof(char*));
    if (ptr == NULL)
        return -3;
    *matrix = ptr;

    for (i = 0; i < n; i++, ptr++){
        if (fscanf_s(fp, "%s%*c", tmp, _countof(tmp)) != 1)
            break;
        *ptr = strdup(tmp);
        if (*ptr == NULL)
            break;
    }
    fclose(fp);

    return i;
}

void order(char ***matrix, int n){

    int i, j;

    char **ptr;
    char *tmp = malloc(sizeof(char) * MAX_LEN);
    if (tmp == NULL) {
        fprintf(stderr, "\nCan't allocate memory. Abort.");
        exit(EXIT_FAILURE);
    }

    // Bubble sort
    for (i = 0; i < (n-1); i++){
        for (j = 0, ptr = *matrix; j < (n - (1 + i)); j++, ptr++){
            if (strcmp(*ptr, *(ptr + 1)) > 0){
                tmp = *ptr;
                *ptr = *(ptr + 1);
                *(ptr + 1) = tmp;
            }
        }
    }
}