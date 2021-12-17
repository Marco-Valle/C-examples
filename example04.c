#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Past examination 15/09/2017 (version C)
 * today date: 07/04/2021
 * my signature: 49dbffdad5428e0a32ba850a6e5e0d45a550c70ef110a748058feb78590ca4dd
*/


unsigned open_files(FILE**, char[], char[]);
void update_primes(unsigned);
unsigned is_truncable_prime(char[10], unsigned, unsigned);
unsigned import_numbers(FILE*, unsigned**, unsigned, unsigned);
void get_output(unsigned*, unsigned, unsigned, unsigned, FILE*);

unsigned* primes_array;     // All the integers in the range [0, n] corresponds to 0: prime, 1: not prime

int main(int argc, char** argv) {

    unsigned right_limit = 32767;               // Max integer is the default right limit
    unsigned left_limit = 0;                    // 0 is the lowest unsigned int so it is the default left limit
    unsigned number_truncated_prime;            // The number of the right-truncatable prime
    char input_filename[25] = "input.txt";      // The default filename for input
    char output_filename[25] = "output.txt";    // The filename for output
    unsigned* truncated_primes_array;
    FILE** files[2];

    // Allocate some memory for truncated_primes_array
    truncated_primes_array = malloc(sizeof(unsigned));

    if(argc > 1)
        strcpy_s(input_filename, sizeof(input_filename), argv[1]);
    if(argc > 2)
        left_limit = (unsigned) strtol(argv[2], NULL, 10);
    if(argc > 3)
        right_limit = (unsigned) strtol(argv[3], NULL, 10);

    if (!(open_files(files, input_filename, output_filename))){
        printf_s("Can't open the files\n");
        exit(EXIT_FAILURE);
    }

    // Compute all the prime numbers in the given range
    update_primes(right_limit);

    // Import the numbers from the file, compare them with our list of prime numbers
    // and get the numbers of truncated prime numbers in our range
    number_truncated_prime = import_numbers(files[0], &truncated_primes_array, left_limit, right_limit);

    // Print the output
    get_output(truncated_primes_array, number_truncated_prime, left_limit, right_limit, files[1]);

    // Close the files
    _fcloseall();

    // Deallocate the memory
    free(truncated_primes_array);
    // This sometimes causes some issues, because the primes_array changes arbitrary its address
    // free(primes_array);

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

void update_primes(unsigned n){

    unsigned p;     // the prime number on which we are working on (we begin at 2)
    unsigned i;     // tmp indexes

    // Allocate memory
    primes_array = malloc(sizeof(unsigned) * (n+1));

    // Set all the value of the array equal 0
    for(i= 0; i< (n); i++)
        primes_array[i] = 0;

    // Eratosthenes' sieve implementation
    for(p = 2; p<=n; p++){
        // For each integer between 2 and n
        if (!primes_array[p]) {
            // If it isn't already marked as not prime
            for (i = 2; i*p <= n; i++) {
                // For each of its multiples
                if (!primes_array[i * p]) {
                    // If its multiple is not already marked as not prime
                    primes_array[i * p] = 1;  //  mark it
                }
            }
        }
    }
}

unsigned is_truncable_prime(char n[10], unsigned left, unsigned right){

    unsigned number;    // numerical value of n
    char tmp[10];       // tmp substring

    // Get the numeric value of n
    number = (unsigned) strtol(n, NULL, 10);

    // If n is between our bounds
    if (number > right || number < left)
        return 0;

    // If n is prime
    if (primes_array[number])
        return 0;

    // Check it it is right-truncated prime number
    for (unsigned i = 0; i<(sizeof(tmp)-1) ;i++){
        // If we find the end of string break
        if (n[i] == '\0')
            break;
        memcpy(tmp, n, (i+1));  // copy the substring into tmp (from the beginning to the i+1 char)
        tmp[i+1] = '\0';             // add the end of string in the last position of tmp

        // Check if the number in tmp is a prime one
        if (primes_array[ strtol(tmp, NULL, 10) ])
            return 0;
    }

    return 1;

}

unsigned import_numbers(FILE* input, unsigned** array, unsigned left, unsigned right){

    char n[10];
    unsigned counter = 0;
    unsigned *ptr = NULL;

    while ( fscanf_s(input, "%s ", n, sizeof(n)) ){

        // If the EOF is reached, break
        if (feof(input))
            break;

        if (is_truncable_prime(n, left, right)){
            counter++;                                                          // increment the counter
            ptr = realloc(*array, counter*sizeof(unsigned));              // allocate the memory required
            if (ptr == NULL){
                printf("Memory allocation error. Abort.");
                exit(EXIT_FAILURE);
            }
            *array = ptr;
            ptr[counter-1] = (unsigned) strtol(n, NULL, 10);     // add to the array the number
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
