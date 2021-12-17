#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_LEN 25

/*
 * Past examination 15/09/2017 (version C)
 * today date: 07/04/2021
 * my signature: 49dbffdad5428e0a32ba850a6e5e0d45a550c70ef110a748058feb78590ca4dd
*/


unsigned open_files(FILE**, char[], char[]);
unsigned * update_primes(unsigned);
unsigned is_truncable_prime(char[10], const unsigned*, unsigned, unsigned);
unsigned import_numbers(FILE*, unsigned**, const unsigned*, unsigned, unsigned);
void get_output(unsigned*, unsigned, unsigned, unsigned, FILE*);

int main(int argc, char** argv) {

    unsigned right_limit = UINT_MAX;               // Max integer is the default right limit
    unsigned left_limit = 1;                    // 0 is the lowest unsigned int so it is the default left limit
    unsigned number_truncated_prime;            // The number of the right-truncatable prime
    unsigned *truncated_primes_array, *primes_array;
    FILE* files[2];
    char input_filename[MAX_LEN] = "input.txt";
    char output_filename[] = "output.txt";

    // Allocate some memory for truncated_primes_array
    truncated_primes_array = malloc(sizeof(unsigned));

    if(argc > 1)
        strcpy_s(input_filename, _countof(input_filename), argv[1]);
    if(argc > 2)
        left_limit = (unsigned) strtol(argv[2], NULL, 10);
    if(argc > 3)
        right_limit = (unsigned) strtol(argv[3], NULL, 10);

    if (left_limit < 1)
        left_limit = 1;

    if (!(open_files(files, input_filename, output_filename))){
        printf_s("Can't open the files\n");
        exit(EXIT_FAILURE);
    }

    // Compute all the prime numbers in the given range
    primes_array = update_primes(right_limit);

    // Import the numbers from the file, compare them with our list of prime numbers
    // and get the numbers of truncated prime numbers in our range
    number_truncated_prime = import_numbers(files[0], &truncated_primes_array, primes_array, left_limit, right_limit);

    // Print the output
    get_output(truncated_primes_array, number_truncated_prime, left_limit, right_limit, files[1]);

    // Deallocate the memory
    free(truncated_primes_array);
    free(primes_array);

    // Close the files and exit
    exit(EXIT_SUCCESS);

}

unsigned open_files(FILE** files, char filename_in[], char filename_out[]){

    // INPUT
    if(fopen_s(&files[0], filename_in, "r"))
        return 0;
    // OUTPUT
    if(fopen_s(&files[1], filename_out, "w"))
        return 0;

    return 1;
}

unsigned * update_primes(unsigned n){

    unsigned p;     // the prime number on which we are working on (we begin at 2)
    unsigned i;     // tmp indexes
    unsigned *primes_array;

    // Allocate memory
    primes_array = calloc(n + 1, sizeof(unsigned));    // All the integers in the range [0, n] corresponds to 0: prime, 1: not prime
    if (primes_array == NULL){
        printf("\nMemory allocation error. Abort.");
        exit(EXIT_FAILURE);
    }

    // Eratosthenes' sieve implementation
    for(p = 2; p <= n; p++){
        // For each integer between 2 and n
        if (!primes_array[p]) {
            // If it isn't already marked as not prime
            for (i = 2; i * p <= n; i++) {
                // For each of its multiples
                primes_array[i * p] = 1;
            }
        }
    }
    return primes_array;
}

unsigned is_truncable_prime(char n[10], const unsigned *primes_array, unsigned left, unsigned right){

    unsigned number;    // numerical value of n
    char tmp[10];       // tmp substring

    // Get the numeric value of n
    number = (unsigned) strtol(n, NULL, 10);

    // If n is between our bounds
    if (number > right || number < left)
        return 0;

    // If n is not prime
    if (primes_array[number])
        return 0;

    // Check it it is right-truncated prime number
    for (unsigned i = 0; i < (_countof(tmp) - 1); i++){
        // If we find the end of string break
        if (n[i] == '\0')
            break;
        memcpy(tmp, n, (i + 1) * sizeof(char));       // copy the substring into tmp (from the beginning to the i+1 char)
        tmp[i + 1] = '\0';             // add the end of string in the last position of tmp

        // Check if the number in tmp is a prime one
        if (primes_array[ strtol(tmp, NULL, 10) ])
            return 0;
    }

    return 1;

}

unsigned import_numbers(FILE* input, unsigned** array, const unsigned *primes_array, unsigned left, unsigned right){

    char n[10];
    unsigned counter = 0;
    unsigned *ptr = NULL;

    while ( fscanf_s(input, "%s%*c", n, _countof(n)) ){

        // If the EOF is reached, break
        if (feof(input))
            break;

        if (is_truncable_prime(n, primes_array, left, right)){
            counter++;                                                          // increment the counter
            ptr = realloc(*array, counter*sizeof(unsigned));              // allocate the memory required
            if (ptr == NULL){
                printf("Memory allocation error. Abort.");
                exit(EXIT_FAILURE);
            }
            *array = ptr;
            ptr[counter - 1] = (unsigned) strtol(n, NULL, 10);     // add to the array the number
        }
    }

    return counter;
}

void get_output(unsigned *array, unsigned number, unsigned left, unsigned right, FILE* out){

    // No numbers
    if (!number){
        printf("The file does not contain any right-truncatable prime number between %u and %u\n", left, right);
        fprintf_s(out, "No Number Found");
        return;
    }

    // Some numbers
    printf("The file contains %u right-truncatable prime number between %u and %u\n", number, left, right);
    for (unsigned  i = 0; i < number; i++)
        fprintf_s(out, "%u\n", array[i]);

}
