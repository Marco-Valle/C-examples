#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define EX 1
#define DEBUG 0

#define TIME(begin, end) (double)((end) - (begin)) / CLOCKS_PER_SEC
#define MAX_LEN 21

void my_malloc (void **ptr, size_t size);
void my_realloc (void **ptr, size_t size);
void my_calloc (void **ptr, size_t size, unsigned n);
char * my_strdup (char *string);

#if EX == 1
/* Exercise 1 */
void quick_sort (int *A, int l, int r);
int quick_partition (int *A, int l, int r);
void swap (int *A, int i, int j);
int * get_rand_array (int size, int range);
void print_array (char *str, int *array, int n);
void get_sizes (int *na, int *nb, int *nc, int *final);
void sort (int *array, int size);
int * merge3 (const int *a, const int *b, const int *c, int na, int nb, int nc);
enum Which {A, B};
#elif EX == 2
/* Exercise 2 */
struct list{
    char *surname;
    struct name_list *names;
    struct list *next;
};
struct name_list{
    char *name;
    struct name_list *next;
};
typedef struct list list_t;
typedef struct name_list name_list_t;
int list_insert_in_order (list_t **head, char *name, char *surname);
int insert_name_in_list (list_t *ptr, char *name);
name_list_t * find_name_position (char *name, name_list_t *head);
name_list_t * insert_name (char *name, name_list_t *previous, name_list_t *next);
list_t  * insert_surname (char *surname, list_t *previous, list_t *next);
int check_name(name_list_t *ptr, char *name);
void print_list(list_t *head, int in_sequence);
void dispose_list(list_t *head);
void input_name(char *name, char *surname);
#elif EX == 3
/* Exercise 3 */
float difference_r(const float *flows, unsigned i, unsigned N, float balance, float max, float min);
float permute(float *unique_values, float *best, float *solution, float best_distance,
             int *mark, unsigned N, unsigned N_uniques, unsigned pos);
unsigned get_operations(float **flows, float **ordered, int **mark, unsigned *N_uniques);
void quick_sort (float *A, int l, int r);
int quick_partition (float *A, int l, int r);
void swap (float *A, int i, int j);
int in_array(float val, const float *array, unsigned N);
void update_mark(int *mark, const float *array, unsigned N, unsigned N_uniques);
void print_flows(float *flows, unsigned N);
void print_array(float *array, unsigned N);
void print_int_array(int *array, unsigned N);
#endif

int MAX_INT_LEN = 3 * sizeof(int)*CHAR_BIT/8 + 2;

int main() {
    clock_t begin, end;
#if EX == 1
    // Exercise 1
    int *a, *b, *c, *result;
    int na, nb, nc, result_size, range;
    char *output;

    get_sizes(&na, &nb, &nc, &result_size);
    range = (int) pow(result_size, 2);
    a = get_rand_array(na, range);
    sort(a, na);
    b = get_rand_array(nb, range);
    sort(b, nb);
    c = get_rand_array(nc, range);
    sort(c, nc);
#if DEBUG
    print_array("A:", a, na);
    print_array("B:", b, nb);
    print_array("C:", c, nc);
#endif

    begin = clock();
    result = merge3(a, b, c, na, nb, nc);
    end = clock();

    my_malloc((void **) &output, sizeof(char) * (MAX_INT_LEN * 2 + 58));
    sprintf(output, "Result (len = %d, time required for merging = %.2f seconds):", result_size, TIME(begin, end));
    print_array(output, result, result_size);

    free(output);
    free(a);
    free(b);
    free(c);
    free(result);
#elif EX == 2
    // Exercise 2
    list_t *my_list = NULL;
    char name[MAX_LEN], surname[MAX_LEN];
    fprintf(stdout, "Type exit to quit.");
    while (1) {
        input_name(name, surname);
        if (!strcmp(name, "exit") || !strcmp(surname, "exit")){
            break;
        }
        if (list_insert_in_order(&my_list, name, surname)){
            fprintf(stdout, "Name already existing");
        } else {
            print_list(my_list, 0);
        }
    }
    print_list(my_list, 1);
    dispose_list(my_list);
#elif EX == 3
    // Exercise 3
    float *flows, *best, *ordered, *tmp_solution, best_distance;
    unsigned N, N_uniques;
    int *mark;

    N = get_operations(&flows, &ordered, &mark, &N_uniques);
    my_malloc((void **) &best, sizeof(float) * N);
    my_malloc((void **) &tmp_solution, sizeof(float) * N);

    begin = clock();
    best_distance = permute(ordered, best, tmp_solution, (float) INT_MAX, mark, N, N_uniques, 0);
    end = clock();

    fprintf(stdout, "\n\nBest max distance: %2.f\n", best_distance);
    fprintf(stdout, "Permutations required: %f seconds", TIME(begin, end));
    print_flows(best, N);

    free(flows);
    free(best);
    free(ordered);
    free(tmp_solution);
    free(mark);
#endif
    fprintf(stdout, "\n\n");
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void my_malloc(void **ptr, size_t size){
    if ((*ptr = malloc(size)) != NULL){
        return;
    }
    fprintf(stderr, "\nMemory allocation error. Abort.\n\n");
    exit(EXIT_FAILURE);
}

void my_realloc(void **ptr, size_t size) {
    void *tmp = realloc(*ptr, size);
    if (tmp != NULL){
        *ptr = tmp;
        return;
    }
    fprintf(stderr, "\nMemory reallocation error. Abort.\n\n");
    exit(EXIT_FAILURE);
}

void my_calloc(void **ptr, size_t size, unsigned n){
    if ((*ptr = calloc(n, size)) != NULL){
        return;
    }
    fprintf(stderr, "\nMemory allocation error. Abort.\n\n");
    exit(EXIT_FAILURE);
}

char * my_strdup(char *string){
    char *dest;
    dest = strdup(string);
    if (dest != NULL){
        return dest;
    }
    fprintf(stderr, "\nMemory allocation error (strdup). Abort.\n\n");
    exit(EXIT_FAILURE);
}

#if EX == 1
int * merge3 (const int *a, const int *b, const int *c, int na, int nb, int nc){
    int i = 0, j = 0, k = 0, size = na + nb + nc, min;
    enum Which which;
    int *result;
    my_malloc((void **) &result, sizeof(int) * size);
    for (int l = 0; l < size; ++l) {
        if (i < na){
            min = a[i];
            which = A;
        } else if (j < nb){
            min = b[j];
            which = B;
        } else {
            result[l] = c[k++];
            goto end;
        }
        if (which == A && j < nb && min > b[j]){
            min = b[j];
            which = B;
        }
        if (k < nc && min > c[k]){
            result[l] = c[k++];
            goto end;
        }
        switch (which) {
            case A:
                result[l] = a[i++];
                break;
            case B:
                result[l] = b[j++];
                break;
        }
        end:;
    }
    return result;
}

void quick_sort(int *A, int l, int r){
    if (l >= r) return;
    int c = quick_partition(A, l, r);
    quick_sort(A, l, c - 1);
    quick_sort(A, ++c, r);
}

int quick_partition(int *A, int l, int r){
    int i = l - 1, j = r, pivot = A[r];
    while (i < j){
        while (A[++i] < pivot);
        while (j > l && A[--j] >= pivot);
        if (i < j) swap(A, i, j);
    }
    swap(A, i, r);
    return i;
}

void swap (int *A, int i, int j){
    int tmp = A[i];
    A[i] = A[j];
    A[j] = tmp;
}

void print_array(char *str, int *array, int n){
    fprintf(stdout, "\n\n%s\n", str);
#if DEBUG
    for (int i = 0; i < n; ++i) {
        fprintf(stdout, "\n%d", array[i]);
    }
#endif
}

void get_sizes(int *na, int *nb, int *nc, int *final){
    fprintf(stdout, "Type the sizes:");
    if (fscanf(stdin, "%d%*c%d%*c%d", na, nb, nc) != 3){
        fprintf(stderr, "\nCan't get the input. Abort.\n\n");
        exit(EXIT_FAILURE);
    }
    *final = *na + *nb + *nc;
}

int * get_rand_array (int size, int range){
    int *new;
    my_malloc((void **) &new, sizeof(int) * size);
    for (int i = 0; i < size; ++i) {
        new[i] = rand() % (range + 1);
    }
    return new;
}

void sort(int *array, int size){
    quick_sort(array, 0, --size);
}
#elif EX ==2
int list_insert_in_order (list_t **head, char *name, char *surname){
    int comparison_tmp;
    list_t *list_ptr = *head, *new_element;
    name_list_t *new_name;
    if (list_ptr == NULL)   goto end;
    comparison_tmp = strcmp(list_ptr->surname, surname);
    if (!comparison_tmp) return insert_name_in_list(list_ptr, name);
    if (comparison_tmp > 0) goto end;
    while (list_ptr->next != NULL && strcmp(list_ptr->next->surname, surname) < 0){
        list_ptr = list_ptr->next;
    }
    if (list_ptr->next != NULL && strcmp(list_ptr->next->surname, surname) == 0){
        // Surname already existent
        return insert_name_in_list(list_ptr->next, name);
    }
    end:
    if (*head == NULL){
        new_element = insert_surname(surname, NULL, NULL);
        *head = new_element;
    } else if (comparison_tmp > 0){
        new_element = insert_surname(surname, NULL, list_ptr);
        *head = new_element;
    }else {
        new_element = insert_surname(surname, list_ptr, list_ptr->next);
    }
    new_name = insert_name(name, NULL, NULL);
    new_element->names = new_name;
    return 0;
}

int insert_name_in_list (list_t *ptr, char *name){
    if (ptr == NULL) return -1;
    name_list_t *name_ptr = find_name_position(name, ptr->names);
    if (check_name(name_ptr, name)) return 1;
    if (name_ptr == ptr->names && strcmp(name_ptr->name, name) > 0){
        ptr->names = insert_name(name, NULL, name_ptr);
    } else {
        insert_name(name, name_ptr, name_ptr->next);
    }
    return 0;
}

name_list_t * find_name_position (char *name, name_list_t *head){
    if (head == NULL)   return NULL;
    if (!strcmp(head->name, name)){
        return head;
    }
    while (head->next != NULL && strcmp(head->next->name, name) < 0){
        head = head->next;
    }
    return head;
}

list_t * insert_surname (char *surname, list_t *previous, list_t *next){
    list_t *new_item;
    my_malloc((void **) &new_item, sizeof(list_t));
    new_item->surname = my_strdup(surname);
    new_item->next = next;
    if (previous != NULL && strcmp(previous->surname, surname) < 0){
        previous->next = new_item;
    }
    return new_item;
}

name_list_t * insert_name (char *name, name_list_t *previous, name_list_t *next){
    name_list_t *new_name;
    my_malloc((void **) &new_name, sizeof(name_list_t));
    new_name->name = my_strdup(name);
    new_name->next = next;
    if (previous != NULL){
        previous->next = new_name;
    }
    return new_name;
}

int check_name(name_list_t *ptr, char *name){
    if (ptr == NULL)    return 0;
    if (strcmp(ptr->name, name) == 0)   return 1;
    return (ptr->next != NULL && strcmp(ptr->next->name, name) == 0);
}

void print_list(list_t *head, int in_sequence){
    name_list_t *ptr;
    for (int i = 0; head != NULL; i++){
        if (!in_sequence){
            fprintf(stdout, "\n%d) %s", i, head->surname);
        }
        ptr = head->names;
        while (ptr != NULL){
            if (in_sequence){
                fprintf(stdout, "%s %s; ", head->surname, ptr->name);
            } else {
                fprintf(stdout, "\n\t%s", ptr->name);
            }
            ptr = ptr->next;
        }
        head = head->next;
    }
}

void dispose_list(list_t *head){
    list_t *list_ptr;
    name_list_t *names_ptr, *names_ptr_tmp;
    while (head != NULL){
        free(head->surname);
        names_ptr = head->names;
        while (names_ptr != NULL){
            free(names_ptr->name);
            names_ptr_tmp = names_ptr;
            names_ptr = names_ptr->next;
            free(names_ptr_tmp);
        }
        list_ptr = head;
        head = head->next;
        free(list_ptr);
    }
}

void input_name(char *name, char *surname){
    fprintf(stdout, "\nInput (name surname):");
    fflush(stdout);
    fscanf(stdin, "%20s", name);
    if (!strcmp(name, "exit")){
        return;
    }
    fscanf(stdin, "%20s", surname);
}
#elif EX == 3
float difference_r(const float *flows, unsigned i, unsigned N, float balance, float max, float min){
    if (i >= N){
        return max - min;
    }
    balance += flows[i];
    if (balance > max){
        max = balance;
    } else if (balance < min){
        min = balance;
    }
    return difference_r(flows, ++i, N, balance, max, min);
}

float permute(float *unique_values, float *best, float *solution, float best_distance,
             int *mark, unsigned N, unsigned N_uniques, unsigned pos){
    if (pos >= N){
        float tmp_dst = difference_r(solution, 0, N, 0, 0, 0);
        if (tmp_dst < best_distance){
            memcpy(best, solution, sizeof(float) * N);
            best_distance = tmp_dst;
        }
        return best_distance;
    }
    for (int i = 0; i < N_uniques; ++i) {
        if (mark[i] <= 0) goto loop_bottom;
        mark[i]--;
        solution[pos] = unique_values[i];
        best_distance = permute(unique_values, best, solution, best_distance, mark, N, N_uniques, pos+1);
        mark[i]++;
        loop_bottom:;
    }
    return best_distance;
}

void quick_sort(float *A, int l, int r){
    if (l >= r) return;
    int c = quick_partition(A, l, r);
    quick_sort(A, l, c - 1);
    quick_sort(A, ++c, r);
}

int quick_partition(float *A, int l, int r){
    int i = l - 1, j = r;
    float pivot = A[r];
    while (i < j){
        while (A[++i] < pivot);
        while (j > l && A[--j] >= pivot);
        if (i < j) swap(A, i, j);
    }
    swap(A, i, r);
    return i;
}

void swap (float *A, int i, int j){
    float tmp = A[i];
    A[i] = A[j];
    A[j] = tmp;
}

unsigned get_operations(float **flows, float **ordered, int **mark, unsigned *N_uniques){
    float *flows_ptr, *ordered_ptr;
    unsigned actual_size = 1, actual_size_uniques = 1, N = 0;
    my_malloc((void **) flows, sizeof(float));
    my_malloc((void **) ordered, sizeof(float));
    flows_ptr = *flows;
    ordered_ptr = *ordered;
    *N_uniques = 0;
    while (fscanf(stdin, "%f%*c", &flows_ptr[N]) == 1 && flows_ptr[N]) {
#if DEBUG
        fprintf(stdout, "\nFlows:");
        print_array(flows_ptr, N + 1);
#endif
        if (!in_array(flows_ptr[N], *flows, N)){
            ordered_ptr[*N_uniques] = flows_ptr[N];
            *N_uniques += 1;
#if DEBUG
            fprintf(stdout, "\n\nUniques:");
            print_array(ordered_ptr, *N_uniques);
#endif
        }
        N++;
        if (N >= actual_size) {
            actual_size *= 2;
            my_realloc((void **) flows, sizeof(float) * actual_size);
            flows_ptr = *flows;
        }
        if (*N_uniques >= actual_size_uniques) {
            actual_size_uniques *= 2;
            my_realloc((void **) ordered, sizeof(float) * actual_size_uniques);
            ordered_ptr = *ordered;
        }
    }
    quick_sort(ordered_ptr, 0, (int) *N_uniques - 1);
    quick_sort(flows_ptr, 0, (int) N - 1);
    my_calloc((void **) mark, sizeof(int), *N_uniques);
    update_mark(*mark,  flows_ptr, N, *N_uniques);
#if DEBUG
    fprintf(stdout, "\n\n\nFlows (sorted):");
    print_array(flows_ptr, N);
    fprintf(stdout, "\n\nOrdered:");
    print_array(ordered_ptr, *N_uniques);
    fprintf(stdout, "\n\nMark:");
    print_int_array(*mark, *N_uniques);
#endif
    return N;
}

int in_array(float val, const float *array, unsigned N){
    for (int i = 0; i < N; ++i) {
        if (val == array[i])    return 1;
    }
    return 0;
}

void update_mark(int *mark, const float *array, unsigned N, unsigned N_uniques){
    int i, j = -1;
    float prev = 0;
    for (i = 0; i < N; ++i) {
        if (prev != array[i]){
            prev = array[i];
            if (++j > N_uniques){
                break;
            }
        }
        mark[j]++;
    }
}

void print_flows(float *flows, unsigned N){
    fprintf(stdout, "\n\n%u flows:", N);
    for (int i = 0; i < N; ++i) {
        if (flows[i] < 0){
            fprintf(stdout, "\n\t-");
        } else{
            fprintf(stdout, "\n\t+");
        }
        fprintf(stdout, "\t%.2f", fabs((double) flows[i]));
    }
}

void print_array(float *array, unsigned N){
    fprintf(stdout, "\n");
    for (int i = 0; i < N; ++i) {
        fprintf(stdout, "\t%.2f", array[i]);
    }
    fflush(stdout);
}

void print_int_array(int *array, unsigned N){
    fprintf(stdout, "\n");
    for (int i = 0; i < N; ++i) {
        fprintf(stdout, "\t%d", array[i]);
    }
    fflush(stdout);
}
#endif