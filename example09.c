#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG 0
#define MAX_LEN 15
#define MAX_REASONABLE_SIZE 9

#define IDX(x, y, n) (((y)*(n) + (x)))

int compute_sum(int);
int allocate(int**, int**, int**, int);
void deallocate(int*, int*, int*);
int permutations(int*, int*, int*, int, int, int, int, FILE*);
int check(int*, int, int);
void print_square(int*, int, FILE*);

int offset_x[] = {1, 0, 1, -1};
int offset_y[] = {0, 1, 1, 1};

int main(int argc, char **argv) {

    int n, sum, c, *values, *mark, *square;
    FILE *fp;
    char filename[MAX_LEN];

#if DEBUG
    strcpy_s(filename, sizeof(char) * _countof(filename), "out.txt");
    n = 3;
#else
    if (argc < 3){
        fprintf(stderr, "\nWrong command line arguments. Abort.");
        exit(EXIT_FAILURE);
    }
    strcpy_s(filename, sizeof(char) * _countof(filename), argv[2]);
    n = strtol(argv[1], ((argv + 1) + 1), 10);
#endif
    sum = compute_sum(n);
    fprintf(stdout, "\nParameters set filename: %s, n: %d, sum: %d", filename, n, sum);

    if (!allocate(&square, &values, &mark, n)){
        fprintf(stderr, "\nMemory allocation error. Abort.");
        exit(EXIT_FAILURE);
    }

#if DEBUG
    fprintf(stdout, "\nPermutation elements:");
    for (int i = 0; i < (n * n); ++i) {
        fprintf(stdout, "\t%d", values[i]);
    }
#endif

    if (fopen_s(&fp, filename, "w")){
        fprintf(stderr, "\nCan't open the output file. Abort.");
        exit(EXIT_FAILURE);
    }

    c = permutations(square, values, mark, (n * n), 0, 0, sum, fp);
    fprintf(stdout, "\n%d permutations.", c);
    deallocate(square, values, mark);

    exit(EXIT_SUCCESS);
}

int compute_sum(int n){
    return (int) (0.5 * n * (pow(n, 2) + 1));
}

int allocate(int **square, int **values, int **mark, int n){

    int *ptr;

    for (int i = 0; i < 3; ++i) {
        if (i == 2){
            ptr = calloc((n * n), sizeof(int));
        }else {
            ptr = malloc(sizeof(int) * (n * n));
        }
        if (ptr == NULL){
            return 0;
        }
        switch (i) {
            case 0:
                *square = ptr;
                break;
            case 1:
                *values = ptr;
                break;
            case 2:
                *mark = ptr;
                break;
            default:
                break;
        }
    }

    ptr = *values;
    for (int i = 0; i < (n * n); ++i, ptr++) {
        *ptr = (i + 1);
    }

    return 1;
}

void deallocate(int *square, int *values, int *mark){
    free(square);
    free(values);
    free(mark);
}

int permutations(int *square, int *values, int *mark, int n, int count, int square_idx, int sum, FILE *fp){

    int i;

    if (square_idx >= n){
        if (check(square, n, sum)){
            print_square(square, n, fp);
            if (n > MAX_REASONABLE_SIZE){
                // If the length of the square is > 3, when a solution is found abort.
                deallocate(square, values, mark);
                exit(EXIT_SUCCESS);
            }
        }
#if DEBUG == 2
        print_square(square, n, NULL);
#endif
        return (count+1);
    }
    for (i = 0; i < n; i++) {
        if (!mark[i]){
            mark[i] = 1;
            square[square_idx] = values[i];
            count = permutations(square, values, mark, n, count, (square_idx + 1), sum, fp);
            mark[i] = 0;
        }
    }
    return count;
}

void print_square(int *square, int n, FILE *fp){

    fprintf(stdout, "\n\n");
#if DEBUG == 2
    fprintf(stdout, "--------------BEGIN-----------------\n");
#endif
    if (fp != NULL) {
        fprintf(fp, "\n");
    }
    for (int i = 0; i < n; ++i) {
        fprintf(stdout, "%d\t", square[i]);
        if (fp != NULL) {
            fprintf(fp, "%d\t", square[i]);
        }
        if (!((i + 1) % (int) sqrt(n))){
            fprintf(stdout, "\n");
            if (fp != NULL) {
                fprintf(fp, "\n");
            }
        }
    }
#if DEBUG == 2
    fprintf(stdout, "--------------END-----------------");
#endif
}

int check(int *square, int n, int sum){

    int x, y, tmp, i, j, k, end = 1, n_elements = (int) sqrt(n);

    for (i = 0; i < 4; ++i) {
        // For all the checks
        for (j = 0; j < end; ++j) {
            // For all the rows, columns and diagonals
            for (k = 0, tmp = 0; k < n_elements; ++k) {
                // For all the elements of that rows or columns or diagonals
                if (!k){
                    switch (i) {
                        case 0:
                            x = 0;
                            y = j;
                            end = n_elements;
                            break;
                        case 1:
                            x = j;
                            y = 0;
                            end = n_elements;
                            break;
                        case 2:
                            x = 0;
                            y = 0;
                            end = 2;
                            break;
                        case 3:
                            x = (n_elements-1);
                            y = 0;
                            end = 2;
                            break;
                        default:
                            break;
                    }
                }
                tmp += square[IDX(x, y, n_elements)];
                x += offset_x[i];
                y += offset_y[i];
            }
            if (sum != tmp){
                return 0;
            }
        }
    }

    return 1;
}