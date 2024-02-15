#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    int M, N;

    M = 8;
    N = 5;
    const char *TRACE_FILE = "mcf_trace.txt";

    int size_of_table = pow(2, M);
    int *M_TABLE = (int *)malloc(size_of_table * sizeof(int));
    for (int i = 0; i < size_of_table; i++) {
        M_TABLE[i] = 2; // Initialize each entry to 2
    }

    int GHB = 0;
    int total_predictions = 0;
    int mispredictions = 0;

    FILE *trace_file = fopen(TRACE_FILE, "r");
    if (trace_file == NULL) {
        printf("Error opening file: %s\n", TRACE_FILE);
        free(M_TABLE);
        return 1;
    }

    int OUTCOME, M_INDEX, N_EXT, Index, PREDICTION;
    while (fscanf(trace_file, "%x %*c", &M_INDEX) == 1) {
        // Convert 't'/'n' to 1/0 for outcome
        OUTCOME = (fgetc(trace_file) == 't') ? 1 : 0;

        M_INDEX >>= 2; // PC = PC / 4
        M_INDEX &= (int)(pow(2, M) - 1);
        N_EXT = GHB << (M - N);
        Index = M_INDEX ^ N_EXT;
        PREDICTION = M_TABLE[Index];

        // Update GHB
        GHB = (GHB >> 1) | (OUTCOME << (N - 1));

        // Compare prediction with outcome, and increment stats
        if (PREDICTION != OUTCOME) {
            mispredictions++;
        }

        total_predictions++;

        // Update the M_TABLE entry and ensure it stays within the valid range [0, 3]
        PREDICTION += (OUTCOME ? 1 : -1);
        if (PREDICTION > 3) {
            PREDICTION = 3;
        } else if (PREDICTION < 0) {
            PREDICTION = 0;
        }
        M_TABLE[Index] = PREDICTION;
    }

    fclose(trace_file);
    free(M_TABLE);

    // Calculate and print the misprediction rate
    double misprediction_rate = (double)mispredictions / total_predictions * 100;
    printf("Misprediction Rate: %.2f%%\n", misprediction_rate);
    return 0;
}
