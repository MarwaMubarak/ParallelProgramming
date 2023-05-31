#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int isPrime(int m) {
    if (m == 2)
        return 1;
    if (m % 2 == 0 || m == 1)
        return 0;
    int i;
    for (i = 2; i * i <= m; i++) {
        if (m % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {


    int curRank; // current rank
    int n; // number of processes
    const int tag = 0;//tag
    int x;//the lower bound of the range
    int y;// the higher bound of the range
    int i, j;
    int ans = 0;// the count of the prime numbers
    int range; // the range
    int rem;
    double start,end;

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

        range = (y - x+1) / (n); // the range that every slave work on it
        rem = (y - x+1) % (n);// the last slave will do the remaining
    }

    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);//send the start to all slaves 
    MPI_Bcast(&range, 1, MPI_INT, 0, MPI_COMM_WORLD);//send the range to all slaves 
    MPI_Bcast(&rem, 1, MPI_INT, 0, MPI_COMM_WORLD); // send the remaining to the last slave



    int l = x + (curRank * range); //start of the range to this slave
    int r = l + range-1;//end of the range to this slave


    if (curRank == n - 1)//the last slave add the reminder
        r +=rem ;

    // count the prime numbers 
    int count = 0;
    for (i = l; i <= r; i++) {
        if (isPrime(i)) {
            count++;
        }
    }


    MPI_Reduce(&count, &ans, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); //send the results to the master and sum them

    if (curRank == 0) {//maser
        printf("The Prime Numbers Count = %d \n", ans);
        fflush(stdout);

        //calculate the end time
        end =MPI_Wtime();
        double time =(end-start);
        printf("Time :%f sec\n\n",time);
        fflush(stdout);

    }



    // end of MPI
    MPI_Finalize();


    return 0;
}




