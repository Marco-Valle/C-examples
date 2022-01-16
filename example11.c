#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#define MAX_STRING_LEN 9
#define DIFFERENT_CHARS 10
#define INT_N 10

#define TIME(begin, end) (double)((end) - (begin)) / CLOCKS_PER_SEC

typedef struct{
    char c;
    int val;
} letter_t;

int permute(letter_t *table, int *mark, int N, int pos, char *str_1, char *str_2, char *result);
int check_solution(letter_t *table, char *str_1, char *str_2, char *result, int N);
int convert_string(char *string, letter_t *table, int N);
int int_from_char(char c, const letter_t *table, int N);
int new_table(letter_t *table, char *str_1, char *str_2, char *str_3);
int extract_chars(letter_t *table, char *string, int N);
int check_duplicate(char c, const letter_t *table, int N);
void quick_sort (letter_t *A, int l, int r);
int quick_partition (letter_t *A, int l, int r);
int check_input(int argc, char **argv);
void swap (letter_t *A, int i, int j);
void init_mark(int *mark, int N);
void print_solution(letter_t *table, int N, double time);
void print_input(char *str_1, char *str_2, char *result);
void usage(char *name);

int main(int argc, char **argv){

    clock_t tic, toc;
    int mark[DIFFERENT_CHARS], N_different_chars;
    letter_t table[DIFFERENT_CHARS];

    if (!check_input(argc, argv)){
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    print_input(argv[1], argv[2], argv[3]);
    N_different_chars = new_table(table, argv[1], argv[2], argv[3]);
    init_mark(mark, N_different_chars);
    tic = clock();
    if (!permute(table, mark, N_different_chars, 0, argv[1], argv[2], argv[3])){
        fprintf(stdout, "\nCan't solve this problem.");
        exit(EXIT_SUCCESS);
    }
    toc = clock();
    print_solution(table, N_different_chars, TIME(tic, toc));

    exit(EXIT_SUCCESS);
}

int permute(letter_t *table, int *mark, int N, int pos, char *str_1, char *str_2, char *result){
    if (pos >= N){
        return check_solution(table, str_1, str_2, result, N);
    }
    for (int i = 0; i < INT_N; i++) {
        if (mark[i]) goto end_cycle;
        mark[i]++;
        table[pos].val = i;
        if (permute(table, mark, N, pos + 1, str_1, str_2, result)){
            return 1;
        }
        mark[i] = 0;
        end_cycle:;
    }
    return 0;
}

int check_solution(letter_t *table, char *str_1, char *str_2, char *result, int N){
    int tmp_1, tmp_2, tmp_sol;
    tmp_1 = convert_string(str_1, table, N);
    tmp_2 = convert_string(str_2, table,  N);
    tmp_sol = convert_string(result, table, N);
    return (tmp_1 + tmp_2) == tmp_sol;
}

int convert_string(char *string, letter_t *table, int N){
    int str_len = (int) strnlen(string, MAX_STRING_LEN);
    int result = 0, i, my_exp = str_len - 1;
    for (i = 0; i < str_len; i++, my_exp--){
        result += int_from_char(string[i], table, N) * (int) pow(10, my_exp);
    }
    return result;
}

int int_from_char(char c, const letter_t *table, int N){
    int i = -1;
    while (++i < N && table[i].c != c);
    return i < N ? table[i].val : 0;
}

int new_table(letter_t *table, char *str_1, char *str_2, char *str_3){
    int N = 0;
    N = extract_chars(table, str_1, N);
    N = extract_chars(table, str_2, N);
    N = extract_chars(table, str_3, N);
    for (int i = 0; i < N; ++i) table[i].val = 0;
    quick_sort(table, 0, N - 1);
    return N;
}

int extract_chars(letter_t *table, char *string, int N){
    int str_len = (int) strnlen(string, MAX_STRING_LEN);
    for (int i = 0; i < str_len; i++){
        if (check_duplicate(string[i], table, N)){
            table[N++].c = string[i];
        }
    }
    return N;
}

int check_duplicate(char c, const letter_t *table, int N){
    for (int i = 0; i < N; ++i) {
        if (c == table[i].c)
            return 0;
    }
    return 1;
}

void quick_sort (letter_t *A, int l, int r){
    if (l >= r) return;
    int c = quick_partition(A, l, r);
    quick_sort(A, l, c - 1);
    quick_sort(A, c + 1, r);
}

int quick_partition (letter_t *A, int l, int r){
    int i = l - 1, j = r;
    char pivot = A[r].c;
    while (i < j){
        while (A[++i].c < pivot);
        while (j > l && A[--j].c >= pivot);
        if (i < j) swap(A, i, j);
    }
    swap(A, i, r);
    return i;
}

int check_input(int argc, char **argv){
    if (argc < 4)   return 0;
    int lower, str_len, i, j;
    for (i = 1; i < 4; ++i) {
       if ((str_len = (int) strlen(argv[i])) >= MAX_STRING_LEN) {
            return 0;
       }
       lower = islower(argv[i][0]);
       for (j = 1; j < str_len; j++){
           if (lower && !islower(argv[i][j]))
               return 0;
       }
    }
    return 1;
}

void swap(letter_t *A, int i, int j){
    char tmp = A[i].c;
    A[i].c = A[j].c;
    A[j].c = tmp;
}

void init_mark(int *mark, int N){
    for (int i = 0; i < N; ++i) {
        mark[i] = 0;
    }
}

void print_solution(letter_t *table, int N, double time){
    fprintf(stdout, "\nSolution (in %.2f seconds):", time);
    for (int i = 0; i < N; ++i) {
        fprintf(stdout, "\n\t%c:\t%d", table[i].c, table[i].val);
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}

void print_input(char *str_1, char *str_2, char *result){
    fprintf(stdout, "\n%s + %s = %s", str_1, str_2, result);
    fflush(stdout);
}

void usage(char *name){
    fprintf(stdout, "\nString do not include more than 10 different characters ");
    fprintf(stdout, "and they have a maximum length of 8 characters (with only uppercase or lowercase characters).");
    fprintf(stdout, "\n\nUsage:");
    fprintf(stdout, "\n[*] %s x y tot", name);
    fprintf(stdout, "\nExample:");
    fprintf(stdout, "\n[*] verbal_arithmetic SEND MORE MONEY\n");
    fflush(stdout);
}