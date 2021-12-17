#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 25

/*
 * Past examination 18/02/2016 (version D)
 * today date: 12/03/2021
 * my signature: 49dbffdad5428e0a32ba850a6e5e0d45a550c70ef110a748058feb78590ca4dd
*/

typedef struct {
    unsigned row;
    unsigned column;
    float weight;
} Find;

unsigned importMatrix(FILE*, float***);
unsigned importFinds(FILE*, Find**);
unsigned recoverFinds(Find*, float**, float, float, unsigned, unsigned);
void printUsage();
void my_open(FILE**, char*);
void *my_malloc(size_t);
void free_all(Find*, float**, unsigned);

int main(int argc, char **argv) {

    Find *finds;
    float **map, maxPressure, maxWeight;
    unsigned numberOfFinds, N;
    FILE *mapFile = NULL, *findsFile = NULL;
    char mapName[MAX_LEN], findName[MAX_LEN];

    if(argc >= 5){
        strcpy_s(mapName, _countof(mapName) * sizeof(char), argv[1]);
        strcpy_s(findName, _countof(findName) * sizeof(char), argv[2]);
        sscanf_s(argv[3], "%f", &maxPressure);
        sscanf_s(argv[4], "%f", &maxWeight);
    }else{
        printUsage();
        exit(EXIT_SUCCESS);
    }

    // Open the files
    my_open(&mapFile, mapName);
    my_open(&findsFile, findName);

    // Import the map
    if((N = importMatrix(mapFile, &map))) {
        printf("A %uX%u map imported\n", N, N);
    } else {
        printf("Can't import the map. Abort.\n");
        exit(EXIT_FAILURE);
    }

    // Import the finds
    if((numberOfFinds = importFinds(findsFile, &finds))) {
        printf("%u finds in the area\n", numberOfFinds);
    } else {
        printf("No finds in the area\n");
    }

    // Start the recovering operation
    if (numberOfFinds) {
        printf("\n%u finds recovered\n", recoverFinds(finds, map, maxPressure, maxWeight, numberOfFinds, N));
    }

    free_all(finds, map, N);

    exit(EXIT_SUCCESS);
}

void printUsage(){
    printf("\nUsage: ./program mapName findsName maxPressure maxWeight\n\n");
}

unsigned importMatrix(FILE* file, float ***matrix){

    unsigned N = 1;
    float **ptr;
    int c;

    // Get the size
    rewind(file);
    while ((c = fgetc(file)) != 10 || c == EOF){
        if (c == 32){
            N++;
        }
    }
    rewind(file);

    // Allocate the memory
    ptr = (float **) my_malloc(sizeof(float*) * N);
    *matrix = ptr;
    for (int i = 0; i < N; ++i) {
        ptr[i] = my_malloc(sizeof(float) * N);
    }

    // Import the values
    for (int i = 0; i < N; ++i) {
        if (!fscanf_s(file, "%f%*c", &ptr[i / N][i % N])){
            printf("\nMap file with a wrong format. Abort.");
            exit(EXIT_FAILURE);
        }
    }

    return N;
}

unsigned importFinds(FILE *file, Find **finds){

    unsigned N = 1;
    int c;
    Find *ptr;

    // Get the size
    rewind(file);
    while ((c = fgetc(file)) != EOF){
        if (c == 10) N++;
    }
    rewind(file);

    // Allocate the memory
    ptr = (Find *) my_malloc(sizeof(Find) * N);
    *finds = ptr;

    // Import the values
    for (int i = 0; i < N; ++i) {
        if (fscanf_s(file, "%u%*c%u%*c%f%*c", &ptr[i].row, &ptr[i].column, &ptr[i].weight) != 3){
            printf("\nFinds file with a wrong format. Abort.");
            exit(EXIT_FAILURE);
        }
    }

    return N;
}

unsigned recoverFinds(Find *finds,
                      float **map,
                      float maxPressure,
                      float maxWeight,
                      unsigned numberOfFinds,
                      unsigned N){

    unsigned dives = 1;
    unsigned findsCaught = 0;
    float actualWeight = 0;

    for(unsigned i = 0; i < numberOfFinds; i++){
        printf("\n");

        // Check if the find's weight isn't over the maximum weight transportable
        if (finds[i].weight > maxWeight) {
            printf("Error: too much weight\n\tDive:\t%u\n\tFind:\t%u\n\tWeight:\t%.2f\n", dives, i, finds[i].weight);
            goto end;
        }

        // Check if the find is in the map
        if (finds[i].column >= N || finds[i].row >= N){
            printf("Error: out of the sea\n\tDive:\t%u\n\tFind:\t%u\n\tX:\t%u\n\tY:\t%u\n",dives, i, finds[i].column, finds[i].row);
            goto end;
        }

        // Check the pressure
        if (map[ finds[i].row ][ finds[i].column ] > maxPressure){
            printf("Error: too much pressure\n\tDive:\t\t%u\n\tFind:\t\t%u\n\tPressure:\t%.2f\n", dives, i, map[ finds[i].row ][ finds[i].column ]);
            goto end;
        }

        actualWeight += finds[i].weight;
        // If we are over the max weight, empty the cargo and carry again the find
        if (actualWeight > maxWeight) {
            actualWeight = finds[i].weight;
            printf("\nNew dive\n");
            dives++;
        }

        findsCaught++;
        printf("Find caught\n\tDive:\t\t%d\n\tActual weight:\t%.2f\n\tFind:\t\t%d\n\tX:\t\t%d\n\tY:\t\t%d\n",
               dives, actualWeight, i, finds[i].column, finds[i].row);

        end:;
    }
    return findsCaught;
}

void my_open(FILE **fp, char *filename){
    if(fopen_s(fp, filename, "r")) {
        exit(EXIT_FAILURE);
    }
}

void *my_malloc(size_t size){
    void *ptr = malloc(size);
    if (ptr == NULL){
        printf("\nMemory allocation error. Abort.");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void free_all(Find *finds, float **map, unsigned N){
    float **ptr = map;
    for (int i = 0; i < N; ++i, ptr++) {
        free(*ptr);
    }
    free(map);
    free(finds);
}
