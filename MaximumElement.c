
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int max(int x, int y) {
    if (x > y)
        return x;
    else
        return y;
}
int min(int x, int y) {
    if (x < y)
        return x;
    else
        return y;
}

int main(int argc, char* argv[])
{


    int curRank; // current rank
    int numOfProcesses; // number of processes
    const int tag = 0;//tag
    int partitionSize;
    int n;// number of the elements of the array
    int* a;//array
    int mx = -1000000000;

    //start of MPI
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &curRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);

    if (curRank == 0) {//master
        printf("Hello from master process.\n");
        fflush(stdout);
        printf("Number of slave processes is %d\n\n", numOfProcesses);
        fflush(stdout);
        printf("Please enter size of array...\n");
        fflush(stdout);
        scanf("%d", &n);// take the input n from the user

        while (n<=0){
            printf("Invalid Input!!\nPlease enter size of array...\n");
            fflush(stdout);
            scanf("%d", &n);// take the input n from the user
        }

        a = (int*)malloc(n* sizeof(int));// initial array size

        // take the array element
        printf("Please enter array elements ... \n");
        fflush(stdout);
        int i;
        int j;
        for ( i = 0; i < n; i++) {
            scanf("%d", &a[i]);
        }
        numOfProcesses = min(numOfProcesses,n);
        partitionSize = n / (numOfProcesses);//partition size
        int rem = n % (numOfProcesses); //remaining elements



        // the master itself
        mx = -1000000000;
        int idx = -1;

        for (i = 0; i < partitionSize; i++) {

            if (mx < a[i]) {
                idx = i;
                mx = max(mx, a[i]);
            }
        }

        printf("Hello from slave# %d number in my partition is %d and index is %d.\n", curRank+1, mx, idx);
        fflush(stdout);
        for (i = 1; i < numOfProcesses-1; i++) { // divide the array and send it yo the slaves
            MPI_Send(&partitionSize, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&a[i * partitionSize], partitionSize, MPI_INT, i, tag, MPI_COMM_WORLD);

        }
        // take the remaining elements with the last core
        if(i<numOfProcesses) {
            int num = partitionSize + rem;
            MPI_Send(&(num), 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&a[(i) * (partitionSize)], num, MPI_INT, i, tag, MPI_COMM_WORLD);

        }
        int mxTemp;
        int idxTemp;
        // receive the ans from the cores
        for (i = 1; i < numOfProcesses; i++) {
            MPI_Recv(&mxTemp, 1, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&idxTemp, 1, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                printf("Hello from slave# %d number in my partition is %d and index is %d.\n", i+1, mxTemp, idxTemp);
                fflush(stdout);
                
            if (mx < mxTemp) {
                mx = max(mx, mxTemp);
                idx = ((i) * partitionSize) + idxTemp;
            }

        }

        printf("Master process announce the final max which is %d and its index is %d.\n", mx, idx);
        fflush(stdout);

        printf("\nThanks for using our program\n");
        fflush(stdout);
    }
    else {//slaves

        mx = -1000000000;
        MPI_Recv(&partitionSize, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        a = (int*)malloc(partitionSize * sizeof(int));
        MPI_Recv(a, partitionSize, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int i;
        int idx = -1;
        for (i = 0; i < partitionSize; i++) {
            if (mx < a[i]) {
                mx = max(mx, a[i]);
                idx = i;
            }
        }
        MPI_Send(&mx, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);//send the mx to the master
        MPI_Send(&idx, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);//send the idx to the master


    }

    // end of MPI
    MPI_Finalize();


    return 0;
}



