#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#define MAX_LEN 50

struct Product {
    char *name;
    unsigned price;
    struct Product *next;
};

struct Manufacturer {
    char *name;
    char *id;
    struct Product *products;
    struct Manufacturer *next;
};

void free_products(struct Product*);
void free_manufacturers(struct Manufacturer*);
void import(struct Manufacturer**, char*, char*);
void print_all(struct Manufacturer*);

int main(int argc, char **argv) {

    struct Manufacturer *head;

#if DEBUG
    char file_1[MAX_LEN], file_2[MAX_LEN];
    strcpy_s(file_1, _countof(file_1) * sizeof(char), "manufacturers.txt");
    strcpy_s(file_2, _countof(file_2) * sizeof(char), "products.txt");
    import(&head, file_1, file_2);
#else
    if (argc >= 3) {
        import(&head, argv[1], argv[2]);
    } else {
        fprintf(stderr, "Wrong command line arguments. Abort.");
        exit(EXIT_FAILURE);
    }
#endif

    print_all(head);
    free_manufacturers(head);

    exit(EXIT_SUCCESS);
}

void free_manufacturers(struct Manufacturer *head){

    struct Manufacturer *ptr;

    while (head != NULL){
        free(head->id);
        free(head->name);
        free_products(head->products);
        ptr = head;
        head = head->next;
        free(ptr);
    }
}

void free_products(struct Product *head){

    struct Product *ptr;

    while (head != NULL){
        free(head->name);
        ptr = head;
        head = head->next;
        free(ptr);
    }
}

void import(struct Manufacturer **head, char *file_manufacturers, char *file_products){

    struct Manufacturer *ptr_manufacturer, *before_manufacturer_ptr = NULL;
    struct Product *ptr_products, *before_products_ptr = NULL;
    unsigned tmp_unsigned, flag = 0;
    FILE *fp_manufacturers, *fp_products;
    char tmp1[MAX_LEN], tmp2[MAX_LEN];

    if (fopen_s(&fp_manufacturers, file_manufacturers, "r")){
        fprintf(stderr, "\nCan't open the manufacturers file. Abort.");
        exit(EXIT_FAILURE);
    }
    if (fopen_s(&fp_products, file_products, "r")){
        fprintf(stderr, "\nCan't open the products file. Abort.");
        exit(EXIT_FAILURE);
    }

    *head = NULL;

    // Import the manufacturers
    while (fscanf_s(fp_manufacturers, "%s%*c%s%*c", tmp1, _countof(tmp1), tmp2, _countof(tmp2)) == 2){
        ptr_manufacturer = malloc(sizeof(struct Manufacturer));
        if (ptr_manufacturer == NULL){
            fprintf(stderr, "\nCan't allocate memory. Abort.");
            exit(EXIT_FAILURE);
        }
        if (*head == NULL){
            *head = ptr_manufacturer;
        }
        ptr_manufacturer->name = _strdup(tmp1);
        ptr_manufacturer->id = _strdup(tmp2);
        if (ptr_manufacturer->name == NULL || ptr_manufacturer->id == NULL){
            fprintf(stderr, "\nCan't allocate memory. Abort.");
            exit(EXIT_FAILURE);
        }

        if (before_manufacturer_ptr != NULL){
            before_manufacturer_ptr->next = ptr_manufacturer;
        }
        before_manufacturer_ptr = ptr_manufacturer;
        ptr_manufacturer->next = NULL;
        ptr_manufacturer->products = NULL;
    }

    // Import the products
    while (fscanf_s(fp_products, "%s%*c%s%*c%u", tmp1, _countof(tmp1), tmp2, _countof(tmp2), &tmp_unsigned) == 3){
        ptr_products = malloc(sizeof(struct Product));
        if (ptr_products == NULL){
            fprintf(stderr, "\nCan't allocate memory. Abort.");
            exit(EXIT_FAILURE);
        }

        ptr_manufacturer = *head;
        while (ptr_manufacturer != NULL && !flag){
            if (!strcmp(tmp1, ptr_manufacturer->id)){
                if (ptr_manufacturer->products == NULL) {
                    ptr_manufacturer->products = ptr_products;
                } else {
                    before_products_ptr = ptr_manufacturer->products;
                    while (before_products_ptr != NULL && before_manufacturer_ptr->next != NULL){
                        before_products_ptr = before_products_ptr->next;
                    }
                    before_products_ptr->next = ptr_products;
                }
                flag = 1;
            }
            ptr_manufacturer = ptr_manufacturer->next;
        }
        if (!flag){
            fprintf(stderr, "\nManufacturer %s not in the list. Abort.", tmp1);
            exit(EXIT_FAILURE);
        }
        flag = 0;

        ptr_products->price = tmp_unsigned;
        ptr_products->name = _strdup(tmp2);
        if (ptr_products->name == NULL){
            fprintf(stderr, "\nCan't allocate memory. Abort.");
            exit(EXIT_FAILURE);
        }

        ptr_products->next = NULL;
    }

    fclose(fp_manufacturers);
    fclose(fp_products);
}

void print_all(struct Manufacturer *head){

    struct Product *ptr;

    while (head != NULL){

        fprintf(stdout, "\n%s:", head->name);
        ptr = head->products;
        while (ptr != NULL){
            fprintf(stdout, "\n\t%s\t%u", ptr->name, ptr->price);
            ptr = ptr->next;
        }

        head = head->next;
    }

}