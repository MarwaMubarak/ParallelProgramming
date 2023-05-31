#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size, remainder, i, j, k;
    int n, localN;
    double start_time, end_time;
    double* array, * localArray, localSum, totalSum, mean, localSquaredDiff, totalSquaredDiff, variance, stdDev;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    start_time = MPI_Wtime();


    if (rank == 0) {//master
        printf("Enter the number of elements: ");
        scanf("%d", &n);
        array = (double*)malloc(sizeof(double) * n);
        totalSum = 0;
        printf("Enter the elements:\n");
        for (i = 0; i < n; i++) {
            scanf("%lf", &array[i]);
            totalSum += array[i];
        }

        mean = totalSum / (double)n;
        totalSquaredDiff = 0.0;

        localN = (size>1? n / (size-1) : 0);
        remainder =(size>1? n % (size-1):n);
        for (i = localN * (size-1); i < n; i++) {
            totalSquaredDiff += (array[i] - mean) * (array[i] - mean);
        }



        int idx = 0;
        for (i = 1; i < size; i++) {
            fflush(stdout);
            localArray = (double*)malloc(sizeof(double) * localN);
            for (j = 0; j < localN; j++) {
                localArray[j] = array[idx++];
            }
            MPI_Send(&mean, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(&localN, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(localArray, localN, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }

        for (i = 1; i < size; i++) {
            MPI_Recv(&localSquaredDiff, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            totalSquaredDiff += localSquaredDiff;
        }
        variance = totalSquaredDiff / (double)n;
        stdDev = sqrt(variance);
        printf("Mean = %lf, Variance = %lf, Standard deviation = %lf\n", mean, variance, stdDev);
        fflush(stdout);
        end_time = MPI_Wtime();
        double total_time = end_time - start_time;
        printf("%f\n", total_time);
    }
    else {//slaves

        MPI_Recv(&mean, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&localN, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        localArray = (double*)malloc(sizeof(double) * localN);
        MPI_Recv(localArray, localN, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        localSquaredDiff = 0.0;
        for (i = 0; i < localN; i++) {
            localSquaredDiff += (localArray[i] - mean) * (localArray[i] - mean);
        }
        MPI_Send(&localSquaredDiff, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

    }

    MPI_Finalize();

    return 0;
}