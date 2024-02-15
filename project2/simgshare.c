#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int M, N;
    // Parsing part
    M = atoi(argv[1]);
    N = atoi(argv[2]);
    const char *TRACE_FILE = argv[3];

    int size_of_table = pow(2, M);
    int *M_TABLE = (int *)malloc(size_of_table * sizeof(int));

    for (int i = 0; i < size_of_table; i++)
    {
        M_TABLE[i] = 2; // Initialize each entry to 2
    }

    int total_predictions = 0;
    int mispredictions = 0;
    int GHB = 0;

    FILE *trace_file = fopen(TRACE_FILE, "r");
    if (trace_file == NULL)
    {
        printf("Error opening file: %s\n", TRACE_FILE);
        free(M_TABLE);
        return 1;
    }
    int PC, OUTCOME, M_INDEX, N_EXT, Index, PREDICTION;
    char OP;
    while (fscanf(trace_file, "%x %c", &PC, &OP) == 2)
    {
        OUTCOME = (OP == 't') ? 1 : 0;
        PC = PC / 4;                       //remove 2 lower bits
        M_INDEX = PC % (int)(pow(2, M));   //extract M-number of bits
        N_EXT = GHB << (M - N);            //append the GHB to calculate index
        Index = M_INDEX ^ N_EXT;           //XOR to get index

        PREDICTION = M_TABLE[Index];

        // Update GHB
        if (OUTCOME)
            GHB = (GHB >> 1) + pow(2, N - 1);
        else
            GHB = GHB >> 1;

        // Compare prediction with outcome, and increment stats
        if ((PREDICTION == 0 || PREDICTION == 1) && OUTCOME == 1)
        {
            mispredictions++;
        }
        else if ((PREDICTION == 2 || PREDICTION == 3) && OUTCOME == 0)
        {
            mispredictions++;
        }
        total_predictions++;

        // Update the M_TABLE entry and ensure it stays within the valid range [0, 3]
        PREDICTION += (OUTCOME ? 1 : -1);   //increment or decrement prediction based on 
        if (PREDICTION > 3)
        {
            PREDICTION = 3;
        }
        else if (PREDICTION < 0)
        {
            PREDICTION = 0;
        }
        M_TABLE[Index] = PREDICTION;
    }
    double misprediction_rate = (double)mispredictions / total_predictions * 100;
    printf("%d %d %.2f%%\n", M, N, misprediction_rate);  //output

    fclose(trace_file);
    free(M_TABLE);
    return 0;
}
