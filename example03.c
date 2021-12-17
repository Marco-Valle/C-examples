#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/*
 * Past examination 16/02/2018 (version D)
 * today date: 06/04/2021
 * my signature: 49dbffdad5428e0a32ba850a6e5e0d45a550c70ef110a748058feb78590ca4dd
*/

#define N 3
#define MAX_COLUMNS 100

unsigned  short open_files(FILE**, char[], char[]);
unsigned short import_samples(int[MAX_COLUMNS], FILE*);
void print_results(int[MAX_COLUMNS], unsigned short, FILE*);

int main(int argc, char *argv[]) {

    FILE* files[2];                            // 1: the input file, 2: the output file
    char filename_input[25] = "input.txt";      // the default input file's name
    char filename_output[25] = "output.txt";    // the default output file's name
    int samples_array[MAX_COLUMNS] = {0};       // the array containing the samples
    unsigned short samples_size;                // the number of samples imported

    // If there are 1 or 2 argv (more than the first), get the files' name from there
    if (argc >= 2)
        strcpy_s(filename_input, sizeof(filename_input), argv[1]);
    if (argc >= 3)
        strcpy_s(filename_output, sizeof(filename_output), argv[2]);

    // Open the file with the samples
    if (!(open_files(files, filename_input, filename_output))){
        printf("\nCan't open the file");
        exit(EXIT_FAILURE);
    }

    // Import the samples from the input file
    if(!(samples_size = import_samples(samples_array, files[0]))){
        printf("\nCan't import the samples");
        exit(EXIT_FAILURE);
    }

    print_results(samples_array, samples_size, files[1]);
    _fcloseall();   // close all the file streams

    exit(EXIT_SUCCESS);
}

unsigned short open_files(FILE** files, char filename_input[], char filename_output[]){
    // Open the input and the output files (if fopen_s returns 0 everything looks good)
    if (fopen_s(&files[0], filename_input, "r"))
        return 0;
    if (fopen_s(&files[1], filename_output, "w"))
        return 0;
    // All fine
    return 1;
}

unsigned short import_samples(int samples_array[MAX_COLUMNS], FILE* file){

    unsigned short max_i = 0;       // the maximum index (the number of samples)
    unsigned short actual_idx = 0;  // the actual index of the samples array
    short j = N;                    // every row has a different j value (from N to -N)
    char c;                         // the char read

    while (fscanf_s(file, "%c", &c, 1) && !feof(file)){
        if (c=='X' && ((samples_array[actual_idx] < j && j > 0) || (samples_array[actual_idx] > j && j < 0))) {
            // If c == X and if the abs of j is greater than the abs of the value already saved
            samples_array[actual_idx] = j;
        }else if(c=='\n') {
            // If the actual index is grater than the maximum one, update the latter
            if (max_i < (actual_idx-1))
                max_i = (actual_idx-1);
            // Decrease j
            j--;
            // j can't be 0 (0 is represented with no X)
            if(!j) j--;
            // Reset the actual index
            actual_idx = -1;
        }
        actual_idx++;
    }

    return max_i;
}

void print_results(int array[MAX_COLUMNS], unsigned short max_index, FILE* out){

    unsigned short i;
    unsigned short max_idx = -1;    // the position of the maximum value in the array
    unsigned short min_idx = -1;    // the position of the minimum value in the array
    int max = INT_MIN;              // the maximum value, it is initialize with the smallest integer
    int min = INT_MAX;              // the minimum value, it is initialize with the greatest integer

    for(i = 0; i<=max_index; i++){
        if (array[i] < min) {
            min = array[i];
            min_idx = i;
        }
        if (array[i] > max) {
            max = array[i];
            max_idx = i;
        }
        fprintf_s(out, "%d ", array[i]);
    }

    printf_s("The maximum value  %d, in position %d\n", max, max_idx);
    printf_s("The minimum value is %d, in position %d\n", min, min_idx);

}
