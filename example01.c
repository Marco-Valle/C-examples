#include <stdio.h>
#include <stdlib.h>

/*
 * Past examination 18/02/2016 (version D)
 * today date: 12/03/2021
 * my signature: 49dbffdad5428e0a32ba850a6e5e0d45a550c70ef110a748058feb78590ca4dd
*/

FILE* mapFile;
FILE* findsFile;
FILE* outFile;
float** map;
float** finds;
unsigned N = 0; // The size of the map NxN

unsigned importMatrix(FILE*, float***);
unsigned recoverFinds(unsigned, unsigned, unsigned);
void printUsage();
void registerFind(unsigned, unsigned, unsigned, unsigned, float, float, float);

int main(int argc, char **argv) {

    // MAIN VARIABLES
    char * mapName;
    char * findName;
    const char outName[] = "out.txt";
    unsigned maxPressure;
    unsigned maxWeight;
    unsigned numberOfFinds;
    int fileErr; // the error number when we open the file (we need this later)

    if(argc==5){
        mapName = argv[1];
        findName = argv[2];
        maxPressure = (unsigned) argv[3];
        maxWeight = (unsigned) argv[4];
    }else{
        // Some default values
        mapName = "SEA_map.txt";
        findName = "finds.txt";
        maxPressure = 120;
        maxWeight = 40;
        printUsage();
    }

    // Open the files
    if((fileErr = fopen_s(&mapFile, mapName, "r")) != 0)
        return fileErr;
    if((fileErr = fopen_s(&findsFile, findName, "r")) != 0)
        return fileErr;
    if((fileErr = fopen_s(&outFile, outName, "w")) != 0)
        return fileErr;

    // Print the header in the output file
    fprintf_s(outFile, "find\tdive\tx\ty\tpressure\tweight\t\tactualWeight\n");

    // Import the map
    N = importMatrix(mapFile, &map);
    if(N>0)
        printf("A %dX%d map imported\n", N, N);
    else
        return 1;

    // Import the finds
    if((numberOfFinds = importMatrix(findsFile, &finds)) == 0)
        printf("No finds in the area\n");
    else
        printf("%d finds in the area\n", numberOfFinds);

    // Start the recovering operation
    if (numberOfFinds)
        printf("\n%d finds recovered\n", recoverFinds(maxPressure, maxWeight, numberOfFinds));

    // Close the files
    fclose(mapFile);
    fclose(findsFile);
    fclose(outFile);

    return 0;
}

void printUsage(){
    printf("\nUsage: ./program mapName findsName maxPressure maxWeight\n\n");
}

unsigned importMatrix(FILE* file, float*** matrix){

    /*
     * This function read char by char the file and import the float values
     * when it finds a white space it casts the number and add a column
     * when it find the line feed add a new row in the matrix
     * it returns the number of the rows
     */

    unsigned Y = 0;
    unsigned X = 0;
    unsigned tmpPosition = 0;
    unsigned stop = 0;
    unsigned i = 0;
    char c;
    char* tmp;

    // Clean the matrix and allocate some new space
    free(*matrix);
    *matrix = malloc(sizeof(float));
    (*matrix)[Y] = malloc(sizeof(float));
    // Allocate some space also for the tmp variable
    tmp = malloc(sizeof(char));

    rewind(file); // precaution
    while (!feof(file) || !stop){
        if((c = fgetc(file)) == NULL || c==-1)
            stop = 1; // save the last value and then stop
        if(c==32 || c==10 || c==-1){
            // Add to the matrix the float
            (*matrix)[Y][X] = (float) atof(tmp);
            // Clean tmp array
            tmpPosition = 0;
            free(tmp);
            tmp = malloc(sizeof(char));
            switch (c) {
                case 32: // space
                    // Resize the row of the matrix
                    X++;
                    (*matrix)[Y] = realloc((*matrix)[Y], (X+1)*sizeof(float));
                    break;
                case 10: // line feed
                    Y++;
                    X = 0;
                    (*matrix) = realloc((*matrix), (Y+1)*sizeof(float));
                    (*matrix)[Y] = malloc(sizeof(float));
                    i++;
                    break;
                default:
                    break;
            }
        }else{
            tmp[tmpPosition] = c;
            tmpPosition++;
            tmp = realloc(tmp, (tmpPosition+1)*sizeof(char));
        }
    }

    // The last one is not counted
    if (i > 0)
        i++;

    return i;
}

unsigned recoverFinds(unsigned maxPressure, unsigned maxWeight, unsigned numberOfFinds){

    unsigned x, y; // coordinates
    unsigned dives = 1;
    unsigned findsCaught = 0;
    float actualWeight = 0;

    for(unsigned i = 0; i<numberOfFinds; i++){
        printf("\n");
        // Check if the find's weight isn't over the maximum weight transportable
        if (finds[i][2] > maxWeight)
            printf("Error: too much weight\n\tDive:\t%d\n\tFind:\t%d\n\tWeight:\t%f\n",dives, i, finds[i][2]);
        else {
            x = (unsigned) finds[i][0];
            y = (unsigned) finds[i][1];

            // Check if the find is in the map
            if (x>=N || y>=N)
                printf("Error: out of the sea\n\tDive:\t%d\n\tFind:\t%d\n\tX:\t%d\n\tY:\t%d\n",dives, i, x, y);
            else{
                // Check the pressure
                if (map[x][y] > maxPressure)
                    printf("Error: too much pressure\n\tDive:\t\t%d\n\tFind:\t\t%d\n\tPressure:\t%.*f\n",dives, i, 2, map[x][y]);
                else {
                    actualWeight += finds[i][2];
                    // If we are over the max weight, empty the cargo and carry again the find
                    if (actualWeight > maxWeight) {
                        actualWeight = finds[i][2];
                        printf("\nNew dive\n");
                        dives++;
                    }
                    findsCaught++;
                    printf("Find caught\n\tDive:\t\t%d\n\tActual weight:\t%.*f\n\tFind:\t\t%d\n\tX:\t\t%d\n\tY:\t\t%d\n", dives, 2, actualWeight, i, x, y);
                    registerFind(i, dives, x, y, map[x][y], finds[i][2], actualWeight);
                }
            }
        }
    }
    return findsCaught;
}

void registerFind(unsigned find, unsigned dive, unsigned x, unsigned y, float pressure, float weight, float actualWeight){
    fprintf_s(outFile, "%d\t%d\t%d\t%d\t%.*f\t\t%.*f\t\t%.*f\n", find, dive, x, y, 2, pressure, 2, weight, 2, actualWeight);
}
