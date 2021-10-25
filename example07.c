#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 51
#define ID_LEN 17
#define BIRTH_LEN 11

struct Employer {
  struct Employer *next;
  struct Employer *previous;
  char *name;
  unsigned salary;
  char birthday[BIRTH_LEN];
  char id[ID_LEN];
};

void deallocate(struct Employer*);
void print(struct Employer*);
void push(struct Employer**, char[], char[], char[], unsigned);
void import(struct Employer**, char[]);
void find(struct Employer*, struct Employer**, char[]);
void move(struct Employer**, char);

int main() {

    struct Employer *head = NULL;
    struct Employer *ptr = NULL;
    char filename[MAX_LEN], name[MAX_LEN], movements[MAX_LEN];

    fprintf_s(stdout, "\n$");
    if (fscanf_s(stdin, "%s%*c%s%*c%s%*c",
                 filename, _countof(filename), name, _countof(name), movements, _countof(movements)) != 3){
        fprintf_s(stderr, "\nCan't understand the command.");
    }

    import(&head, filename);
    find(head, &ptr, name);

    for (int i = 0; i < strlen(movements); ++i)
        move(&ptr, movements[i]);

    deallocate(head);
    fprintf_s(stdout, "\n\n");
    fflush(stdout);

    exit(EXIT_SUCCESS);
}

void deallocate(struct Employer *head){

    struct Employer *ptr;

    while (head != NULL){
        ptr = head;
        head = head->next;
        free(ptr->name);
        free(ptr);
    }
}

void print(struct Employer *ptr){
    fprintf(stdout, "\n%s %s %s %u", ptr->name, ptr->id, ptr->birthday, ptr->salary);
}

void push(struct Employer **head, char name[], char id[], char birthday[], unsigned salary){

    struct Employer *new, *head_ptr = *head;

    new = (struct Employer*) malloc(sizeof(struct Employer));
    if (new == NULL){
        fprintf(stderr, "\nCan't allocate memory. Abort.");
        exit(EXIT_FAILURE);
    }
    new->name = _strdup(name);
    if (new->name == NULL){
        fprintf(stderr, "\nCan't allocate memory. Abort.");
        exit(EXIT_FAILURE);
    }
    strcpy_s(new->id, _countof(new->id), id);
    strcpy_s(new->birthday, _countof(new->birthday), birthday);
    new->salary = salary;

    if (*head == NULL){
        new->next = NULL;
        new->previous = NULL;
    } else {
        new->next = head_ptr;
        new->previous = NULL;
        head_ptr->previous = new;
    }
    *head = new;
}

void import(struct Employer **head, char filename[]){

    FILE *fp;
    unsigned salary;
    char name[MAX_LEN], id[MAX_LEN], birthday[MAX_LEN];

    if (fopen_s(&fp, filename, "r")){
        fprintf(stderr, "\nCan't open the input file. Abort.");
        exit(EXIT_FAILURE);
    }

    while (fscanf_s(fp, "%s%*c%s%*c%s%*c%u%*c", name, _countof(name), id, _countof(id),
                    birthday, _countof(birthday), &salary) == 4){
        push(head, name, id, birthday, salary);
    }
    fclose(fp);
}

void find(struct Employer *head, struct Employer **ptr, char name[]){

    while (head != NULL){
        if (!strcmp(name, head->name))
            break;
        head = head->next;
    }
    if (head != NULL) {
        *ptr = head;
        print(*ptr);
    } else {
        fprintf_s(stdout, "\nCan't find the user with name %s", name);
    }
}

void move(struct Employer **ptr, char sign){

    struct Employer *tmp = *ptr;
    switch (sign) {
        case '+':
            if (tmp->next != NULL)
                tmp = tmp->next;
            break;
        case '-':
            if (tmp->previous != NULL)
                tmp = tmp->previous;
            break;
        default:
            break;
    }
    *ptr = tmp;
    print(*ptr);
}