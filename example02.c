#include <stdio.h>
#include <stdlib.h>

#define side 3

/*
 * Past examination 18/02/2018 (version C)
 * today date: 04/04/2021
 * my signature: 49dbffdad5428e0a32ba850a6e5e0d45a550c70ef110a748058feb78590ca4dd
*/

FILE* open_file(char[]);
void import_square(short[2], int[side][side], FILE*);
int compute_x(short[2], int[side][side]);
void print_all(short[2], int[side][side]);

int main() {

    char filename[] = "input.txt";
    FILE* file;
    int square[side][side];
    short k[2];
    int x;

    if(!(file = open_file(filename))){
        printf("\nError opening the file\n");
        exit(EXIT_FAILURE);
    }

    import_square(k, square, file);
    if(k[0] < 0){
        printf("\nThe square is not a magic one\n");
        return 0;
    }
    if (k[1] == -2){
        printf("\nThe -1 is not on the diagonal\n");
        exit(EXIT_FAILURE);
    }

    x = compute_x(k, square);
    if(x >= 0)
        print_all(k, square);
    else
        printf("\nImpossible to complete the square magic\n");

    return 0;
}

FILE* open_file(char filename[]){
    FILE* file;
    if (fopen_s(&file, filename, "r") == 0)
        return file;
    else
        return 0;
}

void import_square(short k[2], int square[side][side], FILE* file) {

    short tmp_x_sum = 0;            // sum of the row
    short tmp_y_sums[side] = {0};   // sums of the columns
    short i, j;

    k[0] = -1;                      // K
    k[1] = -1;                      // -1 position (the variable x)

    // Read from the file
    rewind(file);
    for (i = 0; i < side; i++) {
        j = 0;
        while (j < side) {
            // Read side*side values (ignoring everything that isn't an integer)
            if (fscanf_s(file, "%d", &square[i][j])) j++;
        }
    }

    // Compute k
    for (i = 0; i < side; i++) {
        // For each row
        for (j = 0; j < side; j++) {
            // For each column
            if (square[i][j] == -1) {
                // In the position of -1
                tmp_x_sum = -1;                             // set the row sum to -1
                tmp_y_sums[j] = -1;                         // set the column sum to -1
                k[1] = (short) (i == j ? i : -2);           // save the position
            } else {
                if (tmp_x_sum != -1)                        // If the row doesn't contain the -1
                    tmp_x_sum += (short) square[i][j];      // update the row sum
                if (tmp_y_sums[j] != -1)                    // If the column doesn't contain the -1
                    tmp_y_sums[j] += (short) square[i][j];  // update the column sum
            }
        }

        /*
         * Sum of the row validation
         * -3 means that the actual row sum is different from the previous one computed
         * k[0] is == -1 if we are at the first iteration
         * tmp_x_sum is < 0 if in that row there's the -1
        */
        if (tmp_x_sum >= 0) {
            // If the sum of the row is not negative
            if (k[0] == -1)
                k[0] = tmp_x_sum;   // if k is not define, do it now
            else
                k[0] = (short)(k[0] != tmp_x_sum ? -3 : k[0]);
        }
        tmp_x_sum = 0;
    }

    /*
    * Sum of the columns validation
    * -3 means that the actual column sum is different from the previous one computed
    * tmp_y_sum is < 0 if in that column there's the -1
    */
    for (j = 0; j < side; j++){
        // For each column sum
        if (tmp_y_sums[j] >= 0 && k[0] != tmp_y_sums[j]) {
            // If the column sum is not negative, but the column sum is different from k
            k[0] = -3;
        }
    }
}

int compute_x(short k[2], int square[side][side]){

    int x;                              // the -1 (the unknown)
    unsigned short j;
    int sum = 0;

    for(j=0; j<side; j++)               // For each column
        if (j != k[1])                  // different from the one with the -1
            sum += square[k[1]][j];     // update the sum

    x = k[0] - sum;

    sum = k[0]*2;                       // set the sum as 2 times K
    square[k[1]][k[1]] = x;             // substitute in the square the x

    // Diagonal check
    for(j = 0; j<side; j++){
        sum -= square[j][j];            // update the sum
        sum -= square[j][side - j - 1];
    }
    if (!sum)
        return x;
    else
        return -1;
}

void print_all(short k[2], int square[side][side]){

    unsigned short i, j;

    printf("\nMagic constant: %hi\n\nSquare:\n", k[0]);
    for (i = 0; i<side; i++){
        // For each row
        for (j = 0; j < side; j++){
            // For each column
            printf("%d\t", square[i][j]);
        }
        printf("\n");
    }

}
