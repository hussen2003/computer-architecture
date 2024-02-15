#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void print2darray(int *BHR, int n)
{
    for(int i = 0; i < n; i++)
    {
        printf("%d ", BHR[i]);
    }
}
int main()
{
    int M = 10;
    int N = 2;

    int table_size = pow(2, M);

    int *M_TABLE = (int *)malloc(table_size * sizeof(int));
    for (int i = 0; i < table_size; i++)
    {
        M_TABLE[i] = 2; // Initialize each entry to 2
    }

    int total_predictions = 0;
    int mispredictions = 0;
    int GHB = 0;
    
    int PC = 0x000004;
    int OUTCOME;
    char OP;
    int M_INDEX, N_EXT, Index,PREDICTION;
    FILE *trace_file = fopen("test.txt", "r");
    
    while (fscanf(trace_file,"%x %c", &PC, &OP) == 2)
    {
        

        OUTCOME = (OP == 't') ? 1 : 0;
        PC = PC/4;
        M_INDEX = PC % (int)(pow(2,M));
        N_EXT = GHB << (M-N);
        Index = M_INDEX ^ N_EXT;
        
        PREDICTION = M_TABLE[Index];
        printf("index: %x GHB: %d Prediction: %d Outcome: %d\n", Index, GHB, PREDICTION, OUTCOME);
        // Update GHB
        if (OUTCOME)
            GHB = (GHB>>1) + pow(2, N-1);
        else
            GHB = GHB>>1;

        // Compare prediction with outcome, and increment stats
        if ((PREDICTION == 0 || PREDICTION == 1) && OUTCOME == 1) 
        {
            mispredictions++;
        }
        else if((PREDICTION == 2 || PREDICTION == 3) && OUTCOME == 0)
        {
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
    printf("%d/%d ", mispredictions, total_predictions);
        
    /*
    PC = PC/4;
    M_INDEX = PC % (int)(pow(2,M));
    N_EXT = GHB << (M-N);
    Index = M_INDEX ^ N_EXT;

    GHB = (GHB >> 1) + pow(2, N-1);
    printf("%d\n", GHB);
    printf("%d\n", Index);
    */
    return 0;


}