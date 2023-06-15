#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void p0_send_input(int input1, int input2, int n) 
{
    int i;

    for (i = 0; i < n/2; i++)
    {
        //printf("Yoo");
        MPI_Send(&input1, 1, MPI_INT, i * 2 + 1, 1, MPI_COMM_WORLD);
        //printf("Yoo");
        MPI_Send(&input2, 1, MPI_INT, i * 2 + 2, 2, MPI_COMM_WORLD);
    }
}

void p0_receive_output(int* output_array , int n)
{
    int output_received = 0, output, source;
    MPI_Status status;

    while (output_received < n-1)
    {
        //printf("T");
        MPI_Recv(&output, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        source = status.MPI_SOURCE;
        output_array[source] = output;
        output_received++;
    }
}

int p1_receive_input() 
{
    int input1;
    MPI_Status status;
    //printf("Ti1");
    MPI_Recv(&input1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    //printf("Ti11");
    return input1;
}

int p1_compute_output(int input) 
{
    int i, output = 0;
    for (i=0; i<=input; i++) 
        if (i % 2 == 0)
            output++;
    return output;
}

void p1_send_output(int output1)
{
    MPI_Send(&output1, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
}

int p2_receive_input() 
{
    MPI_Status status;
    int input2;
    //printf("Ti2");
    MPI_Recv(&input2, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
    //printf("Ti22");
    return input2;
}


int p2_compute_output(int input) 
{
    int i, output = 0;
    for (i=0; i<=input; i++) 
        if (i % 2 != 0)
            output++;
    return output;
}

void p2_send_output(int output2)
{
    MPI_Send(&output2, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
}


int main(int argc, char *argv[]) 
{
    int id, p, input1, input2, output1, output2;
    double wtime;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    input1 = input2= atoi(argv[1]);
    int* output = (int *) malloc(p * sizeof(int));

    if (p < 3) {
        printf("Not enough processes available\n");
        MPI_Finalize();
        exit(0);
    }

    if (id == 0) {
        //printf("0");
        wtime = MPI_Wtime();
        p0_send_input(input1, input2, p);
        p0_receive_output(output,p);
        wtime = MPI_Wtime() - wtime;
        printf("Process 0 - time: %g\n", wtime);
        for(int i=1;i<p;i++)
            printf("Output  %d: %d\n", i, output[i]);
    }

    else if (id%2 == 1) {
        //printf("O");
        wtime = MPI_Wtime();
        input1 = p1_receive_input();
        output1 = p1_compute_output(input1);
        p1_send_output(output1);
        wtime = MPI_Wtime() - wtime;
        printf("Process %d: Time: %g\n", id, wtime);
    }
    
    else if (id%2 == 0) {
        //printf("E");
        wtime = MPI_Wtime();
        input2 = p2_receive_input();
        output2 = p2_compute_output(input2);
        p2_send_output(output2);
        wtime = MPI_Wtime() - wtime;
        printf("Process %d: Time: %g\n", id, wtime);
    }
    MPI_Finalize();
}
