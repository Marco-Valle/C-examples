#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define strdup _strdup
#define MAX_LEN 21
#define IN_FILENAME "words.txt"
#define OUT_FILENAME "out.txt"

int import(char****, char[], unsigned*, unsigned*);
int allocate_array(char ***, unsigned n);
void deallocate_matrix(char ***, unsigned, unsigned);
void sort(char ***, char ***, unsigned, unsigned);
void out(char **, char[], unsigned);
void print_matrix(char ***, unsigned, unsigned);
void merge(char**, char**, unsigned, unsigned, char***);

int main() {

    char ***matrix = NULL;
    char **array = NULL;
    int n;
    unsigned R, C;

    n = import(&matrix, IN_FILENAME, &R, &C);
    if (!n){
        fprintf(stdout, "\nNo entries");
        exit(EXIT_SUCCESS);
    } else if (n < 0){
        fprintf(stderr, "\nError %d. Abort.", n);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "\n%d entries.", n);

    if (!allocate_array(&array, (unsigned) n)){
        fprintf(stderr, "\nCan't allocate the array. Abort.");
        exit(EXIT_FAILURE);
    }

    print_matrix(matrix, R, C);
    sort(matrix, &array, R, C);
    out(array, OUT_FILENAME, (unsigned) n);
    fprintf(stdout, "\n\n");
    fflush(stdout);

    deallocate_matrix(matrix, R, C);
    free(array);

    exit(EXIT_SUCCESS);
}

int import(char ****matrix, char filename[], unsigned *R, unsigned *C){

    char ***row_ptr, **col_ptr;
    FILE *fp;
    char tmp[MAX_LEN];

    if (fopen_s(&fp, filename, "r"))
        return -1;

    if (fscanf_s(fp, "%u%*c%u%*c", R, C) != 2)
        return -2;

    // Allocate rows
    row_ptr = realloc(*matrix, sizeof(char**) * *R);
    if (row_ptr == NULL)
        return -3;
    *matrix = row_ptr;

    // Allocate columns
    for (int i = 0; i < *R; ++i, row_ptr++) {
        *row_ptr = malloc(sizeof(char*) * *C);
        if (*row_ptr == NULL)
            return -4;
    }

    // Import words
    row_ptr = *matrix;
    for (int i = 0; i < *R; ++i, row_ptr++) {
        col_ptr = *row_ptr;
        for (int j = 0; j < *C; ++j, col_ptr++) {
            if (fscanf_s(fp, "%s%*c", tmp, _countof(tmp)) != 1)
                return -5;
            *col_ptr = strdup(tmp);
            if (*col_ptr == NULL)
                return -6;
        }
    }
    fclose(fp);

    return (int)(*R * *C);
}

int allocate_array(char ***array, unsigned n){

    *array = realloc(*array, sizeof(char*) * n);
    if (*array == NULL)
        return 0;
    return 1;
}

void deallocate_matrix(char ***matrix, unsigned R, unsigned C){

    char ***row_ptr, **col_ptr;

    row_ptr = matrix;
    for (int i = 0; i < R; ++i, row_ptr++) {
        col_ptr = *row_ptr;
        for (int j = 0; j < C; ++j, col_ptr++) {
            free(*col_ptr);
        }
        free(*row_ptr);
    }
    free(matrix);
}

void merge(char **a, char **b, unsigned n_a, unsigned n_b, char ***dest){

    int flag = 1;
    int i = 0, j = 0;
    char **dst = *dest;

    while (flag) {

        if (strcmp(*a, *b) < 0){
            *dst = *a;
            a++;
            i++;
        } else {
            *dst = *b;
            b++;
            j++;
        }
        dst++;

        if (i == n_a){
            while (j < n_b){
                *dst = *b;
                b++;
                j++;
                dst++;
            }
            flag = 0;
        } else if (j == n_b){
            while (i < n_a){
                *dst = *a;
                a++;
                i++;
                dst++;
            }
            flag = 0;
        }
    }
}

void sort(char ***matrix, char ***array, unsigned R, unsigned C){

    char **tmp = NULL, **a, **b;
    int flag, el = 3;

    if(!allocate_array(&tmp, (R*C))){
        fprintf(stderr, "\nCan't allocate memory. Abort.");
        exit(EXIT_FAILURE);
    }

    if ((R - 1) % 2 == 0) {
        flag = 1;
    } else {
        flag = 0;   // start from array
    }

    for (int i = 0; i < (R - 1); ++i) {

        b = *(matrix + i + 1);
        if (i == 0) {
            a = *matrix;
        } else if (!flag){
            a = tmp;
        } else{
            a = *array;
        }

        if(!flag){
            merge(a, b, el, C, array);
            flag = 1;
        } else{
            merge(a, b, el, C, &tmp);
            flag = 0;
        }
        el+=3;
    }
    free(tmp);
}

void out(char **array, char filename[], unsigned n){

    FILE *fp;

    if (fopen_s(&fp, filename, "w"))
        return;

    fprintf(stdout, "\n\n%d", n);
    fprintf(fp, "%d", n);

    for (int i = 0; i < n; ++i, array++) {
        fprintf(stdout, "\n%s", *array);
        fprintf(fp, "\n%s", *array);
    }
    fclose(fp);
}

void print_matrix(char ***matrix, unsigned R, unsigned C){

    char **ptr;

    fprintf(stdout, "\n\nMatrix %u %u:\n", R, C);
    for (int i = 0; i < R; ++i, matrix++) {
        ptr = *matrix;
        for (int j = 0; j < C; ++j, ptr++) {
            fprintf(stdout, "\t\t%s", *ptr);
        }
        fprintf(stdout, "\n");
    }
}