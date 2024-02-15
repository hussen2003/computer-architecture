#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 64

int hits_total = 0;
int miss_total = 0;
int memReads = 0;
int memWrites = 0;

typedef struct data // data struct representing instructiouns
{
    long long int tag_address;
    int dirty;
} data;

void print2DArray(data **arr, int rows, int columns);                              // function to print and visualize cache
void LRU(data **cache, data input, int set_num, int num_sets, int Assoc, int WB);  // funtion for LRU replacement policy
void FIFO(data **cache, data input, int set_num, int num_sets, int Assoc, int WB); ////funtion for FIFO replacement policy

int main(int argc, char *argv[])
{
    // Parsing part
    int cache_size = atoi(argv[1]);
    int Assoc = atoi(argv[2]);
    int REPL = atoi(argv[3]);
    int WB = atoi(argv[4]);
    char *Trace_File = argv[5];

    FILE *traceFile = fopen(Trace_File, "r");
    if (traceFile == NULL)
    {
        printf("Failed to open trace file.\n");
        return 1;
    }

    int num_sets = cache_size / (BLOCK_SIZE * Assoc);

    // allocate memory for cache and populating with 0
    data **cache = (data **)malloc(num_sets * sizeof(data *));
    for (int i = 0; i < num_sets; i++)
    {
        cache[i] = (data *)malloc(Assoc * sizeof(data));
    }
    for (int i = 0; i < num_sets; i++)
    {
        for (int j = 0; j < Assoc; j++)
        {
            cache[i][j].tag_address = 0;
            cache[i][j].dirty = 0;
        }
    }

    while (!feof(traceFile)) // reading file
    {
        char op;
        long long int address;

        // parse line from the trace file
        fscanf(traceFile, "%c %llx\n", &op, &address);

        int set_num = abs((address / BLOCK_SIZE) % num_sets);
        long long int tag_address = address / BLOCK_SIZE;

        int dirty = 0;
        if (op == 'W')
            dirty = 1;

        data input;

        input.tag_address = tag_address;
        input.dirty = dirty;

        if (REPL == 0)
        {
            LRU(cache, input, set_num, num_sets, Assoc, WB);
        }
        else
        {
            FIFO(cache, input, set_num, num_sets, Assoc, WB);
        }
    }

    double missRatio = (double)miss_total / (hits_total + miss_total);
    printf("%.6lf\n%d\n%d", missRatio, memWrites, memReads); // printing output

    fclose(traceFile);              // close file
    for (int i = 0; i < Assoc; i++) // free memory
    {
        free(cache[i]);
    }
    free(cache);
}

void LRU(data **cache, data input, int set_num, int num_sets, int Assoc, int WB)
{
    int access_hit = 0;
    int index_hit = -1;

    for (int i = 0; i < Assoc; i++)
    {

        if (input.tag_address == cache[set_num][i].tag_address)
        {
            access_hit = 1;
            index_hit = i;
            break;
        }
    }
    if (access_hit) // cache hit, put it on top of the stack
    {

        if (input.dirty == 1 && WB == 0)     //for write-through
        {
            memWrites++;
        }

        hits_total++;
        data temp = cache[set_num][index_hit];  //Store the value at the given index
        if (temp.dirty == 0)
        {
            temp.dirty = input.dirty;        //update dirty bit
        }

        for (int i = index_hit; i > 0; i--)
        {
            cache[set_num][i] = cache[set_num][i - 1]; // Shift elements to the right
        }
        cache[set_num][0] = temp;
    }
    else //cache not found: miss
    {
        miss_total++;
        memReads++;

        if (WB == 1 && cache[set_num][Assoc - 1].dirty == 1) // handles memory writes if write back policy is used
        {
            memWrites++;
        }

        for (int i = Assoc - 2; i >= 0; i--)
        {
            cache[set_num][i + 1] = cache[set_num][i];
        }
        cache[set_num][0] = input;
    }
}

void FIFO(data **cache, data input, int set_num, int num_sets, int Assoc, int WB)
{
    int access_hit = 0;
    int index_hit = -1;

    for (int i = 0; i < Assoc; i++)
    {

        if (input.tag_address == cache[set_num][i].tag_address)
        {
            access_hit = 1;
            index_hit = i;
            break;
        }
    }
    if (access_hit)  //cache hit FIFO nothing changes 
    {
        hits_total++;

        if (input.dirty == 1 && WB == 0)   //for write-through
        {
            memWrites++;
        }

        if (input.dirty == 1 && cache[set_num][index_hit].dirty == 0)
        {
            cache[set_num][index_hit].dirty == 1; // update dirty bit when hit
        }
    }
    else    //cache miss
    {
        miss_total++;
        memReads++;
        if (WB == 1 && cache[set_num][Assoc - 1].dirty == 1) // handles memory writes if write back policy is used
        {
            memWrites++;
        }
        for (int i = Assoc - 2; i >= 0; i--)                //makes sure the first-in element is the first-out
        {
            cache[set_num][i + 1] = cache[set_num][i];
        }
        cache[set_num][0] = input;
    }
}

void print2DArray(data **arr, int rows, int columns)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%llx(%d)  ", arr[i][j].tag_address, arr[i][j].dirty);
        }
        printf("\n");
    }
}

