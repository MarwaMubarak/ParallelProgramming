#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    static long numOfStep = 1000000;
    int rank;
    int size;
    int start;
    int end;
    double step;
    double sum = 0.0;
    double pi = 0.0;
    double localSum = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    step = 1.0 / (double) numOfStep;
    int range = numOfStep / size;
    start = rank * range;
    end = (rank + 1) * range;

    // Calculate local sum for each process
    int i;
    for (i = start; i < end; i++) {
       double x = ((double) (i + 0.5)) * step;
        localSum += 4.0 / (1.0 + x * x);
    }

    // broadcast the step value from the master to all slaves
    MPI_Bcast(&step, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // using Reduce sum all local sums 
    MPI_Reduce(&localSum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  

    // output the final value of PI on the master process
    if (rank == 0) {
        pi = step * sum;
        printf("Pi= %.20f\n", pi);
    }

    MPI_Finalize();
    return 0;
}
