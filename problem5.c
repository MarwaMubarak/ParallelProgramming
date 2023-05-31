#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int isPrime(int m) {
    if(m==2)
        return 1;
    if (m % 2 == 0 || m==1)
        return 0;
    int i;
    for (i = 2; i * i <= m; i++) {
        if (m % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {


    int curRank; // current rank
    int n; // number of processes
    const int tag = 0;//tag
    int x;//the lower bound of the range
    int y;// the higher bound of the range
    int i, j;
    double start ,end;
    //start of MPI
    MPI_Init(&argc, &argv);


    MPI_Comm_rank(MPI_COMM_WORLD, &curRank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    if (curRank == 0) {//master

        printf("Enter lower bound of the range: \n");
        scanf("%d", &x);
        printf("Enter higher bound of the range: \n");
        scanf("%d", &y);
        start=MPI_Wtime(); //calculate the start time

        int range = (y - x+1) / (n - 1); // the range that every slave work on it
        int rem = (y - x+1) % (n - 1);// the last slave will do the remaining


        //send the data to the slaves
        for (i = 1; i < n; i++) {
            MPI_Send(&x, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&range, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            if(i==n-1){
                MPI_Send(&rem, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            }
        }

        int ans=0;

        //receive data
        for (i = 1; i < n ; i++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            ans+=count;

        }
        printf("The Prime Numbers Count = %d \n",ans);
        //calculate the end time
         end =MPI_Wtime();
        double time =(end-start);
        printf("Time :%f sec\n\n",time);
        fflush(stdout);

    } else {
        // receive the data from the master

        int r, l, range;
        MPI_Recv(&l, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&range, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        r = l+range * curRank-1;
        l= l+(range * (curRank-1));

        if(curRank==n-1) {
            int rem;
            MPI_Recv(&rem, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            r += rem;
        }

        int idx = 0;// check the numbers
        for (i = l; i <= r; i++) {
            if (isPrime(i)) {
                idx++;
            }
        }


        // send the answer to the master
        MPI_Send(&idx, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);


    }

    // end of MPI
    MPI_Finalize();


    return 0;
}




