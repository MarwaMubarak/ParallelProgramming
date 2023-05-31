#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int min(int x,int y){
    if(x>y)
        return y;
    else
        return x;
}
int main(int argc, char *argv[]) {


    int curRank; // current rank
    int numOfProcesses; // number of processes
    const int tag = 0;//tag
    //bool flag = true;

    //start of MPI
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &curRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);

    if (curRank == 0) {//master
        printf("Welcome to vector Matrix multiplication program!\n");
        fflush(stdout);
        int option;
        printf("To read dimensions and values from file press 1\n"
               "To read dimensions and values from console press 2\n");
        fflush(stdout);
        scanf("%d",&option);
        while (option!=1 &&option!=2){
            printf("Invalid option!!\nTo read dimensions and values from file press 1\nTo read dimensions and values from console press 2\n");
            fflush(stdout);
            scanf("%d",&option);

        }
        int x1, y1;
        int *matrix1;
        int x2, y2;
        int *matrix2;
        int j;
        int i;
        int flag = 0;
        int temp;
        int rem;
        int partition;
        if(option==1){
            FILE *fp;
            fp = fopen("problem2.txt", "r");

            fscanf(fp,"%d", &x1);
            fscanf(fp,"%d", &y1);


            matrix1 = (int *) malloc((x1 * y1) * sizeof(int));// initialize matrix1 size

            for (i = 0; i < x1; i++) {
                for (j = 0; j < y1; j++) {
                    fscanf(fp,"%d", &matrix1[i * y1 + j]);
                }
            }
            fscanf(fp,"%d", &x2);
            fscanf(fp,"%d", &y2);
            matrix2 = (int *) malloc((x2 * y2) * sizeof(int));// initialize matrix2 size

            for (i = 0; i < x2; i++) {
                for (j = 0; j < y2; j++) {
                    fscanf(fp,"%d", &matrix2[i * y2 + j]);
                }
            }

            fclose(fp);


        }else {

            do {
                if (flag) {
                    printf("Invalid Multiplication!!\n");
                    fflush(stdout);
                }

                //matrix 1
                printf("Please enter dimensions of the first matrix: \n");
                fflush(stdout);


                scanf("%d", &x1);
                scanf("%d", &y1);
                while (x1 <= 0 || y1 <= 0) {
                    printf("Invalid Input !!\nPlease enter dimensions of the first matrix: \n");
                    fflush(stdout);
                    scanf("%d", &x1);
                    scanf("%d", &y1);
                }
                matrix1 = (int *) malloc((x1 * y1) * sizeof(int));// initialize matrix1 size
                printf("Please enter its elements:\n");
                fflush(stdout);

                for (i = 0; i < x1; i++) {
                    for (j = 0; j < y1; j++) {
                        scanf("%d", &matrix1[i * y1 + j]);
                    }
                }

                //matrix 2
                printf("Please enter dimensions of the second matrix: \n");
                fflush(stdout);

                scanf("%d", &x2);
                scanf("%d", &y2);
                while (x2 <= 0 || y2 <= 0) {
                    printf("Invalid Input !!\nPlease enter dimensions of the second matrix: \n");
                    fflush(stdout);
                    scanf("%d", &x2);
                    scanf("%d", &y2);
                }
                matrix2 = (int *) malloc((x2 * y2) * sizeof(int));// initialize matrix2 size

                printf("Please enter its elements:\n");
                fflush(stdout);

                for (i = 0; i < x2; i++) {
                    for (j = 0; j < y2; j++) {
                        scanf("%d", &matrix2[i * y2 + j]);
                    }
                }
                flag++;
            } while (y1 != x2);

        }



        partition = x1 /numOfProcesses ;// partition size
        rem = x1 % numOfProcesses;//remaining of the  partitions

        temp = x2 * y2;


        printf("\nResult Matrix is (%dx%d):\n", x1, y2);
        //master work itself
        int ii;
        for (ii = 0; ii < partition; ii++) {
            for (i = 0; i < y2; i++) {
                int x = 0;
                for (j = 0; j < x2; j++) {

                    x += matrix1[ii * y1 + j] * matrix2[j * y2 + i];
                }


                printf("%d ", x);
            }
            printf("\n");
            fflush(stdout);
        }

        //for each core send number of partitions
        int counter = partition;
        for (i = 1; i < numOfProcesses - 1; i++) {
            MPI_Send(&partition, 1, MPI_INT, i, tag, MPI_COMM_WORLD); //its number of parts
            for (j = 0; j < partition; j++) {//send to each core  its partition of the rows
                MPI_Send(&x2, 1, MPI_INT, i, tag, MPI_COMM_WORLD);// row size
                MPI_Send(&y2, 1, MPI_INT, i, tag, MPI_COMM_WORLD);// col size
                MPI_Send(&matrix1[counter * x2], x2, MPI_INT, i, tag, MPI_COMM_WORLD);//row of matrix1
                MPI_Send(matrix2, temp, MPI_INT, i, tag, MPI_COMM_WORLD);//matrix2
                counter++;

            }

        }

        // the last core with the remaining partition
        int num = partition + rem;
        if(i<numOfProcesses) {
            MPI_Send(&num, 1, MPI_INT, i, tag, MPI_COMM_WORLD);//its number of parts
            for (j = 0; j < num; j++) {//send to each core  its partition of the rows

                MPI_Send(&x2, 1, MPI_INT, i, tag, MPI_COMM_WORLD);// row size
                MPI_Send(&y2, 1, MPI_INT, i, tag, MPI_COMM_WORLD);// col size
                MPI_Send(&matrix1[counter * x2], x2, MPI_INT, i, tag, MPI_COMM_WORLD);//row of matrix1
                MPI_Send(matrix2, temp, MPI_INT, i, tag, MPI_COMM_WORLD);//matrix2
                counter++;
            }


            // received data
            counter = partition;
            int *ans = (int *) malloc(sizeof(int) * y2);
            for (i = 1; i < numOfProcesses - 1; i++) {
                for (j = 0; j < partition; j++) {

                    MPI_Recv(ans, y2, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    int e;
                    for (e = 0; e < y2; e++) {
                        printf("%d ", ans[e]);
                    }
                    printf("\n");
                    fflush(stdout);
                }

            }
            for (j = 0; j < num; j++) {
                MPI_Recv(ans, y2, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int e;
                for (e = 0; e < y2; e++) {
                    printf("%d ", ans[e]);
                }
                printf("\n");
                fflush(stdout);
            }
        }

    } else {//slaves

        int r, c, counter;
        MPI_Recv(&counter, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int e;
        for (e = 0; e < counter; e++) {
            MPI_Recv(&r, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&c, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int *row = (int *) malloc((r) * sizeof(int));
            int *mat = (int *) malloc((r * c) * sizeof(int));
            MPI_Recv(row, r, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int i, j;

            MPI_Recv(mat, (r * c), MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            //// calculate the anser and send it
            int *ans = (int *) malloc(sizeof(int) * c);
            int x;
            for (i = 0; i < c; i++) {
                x = 0;
                for (j = 0; j < r; j++) {

                    x += row[j] * mat[j * c + i];
                }

                ans[i] = x;
            }


            MPI_Send(ans, c, MPI_INT, 0, tag, MPI_COMM_WORLD);

        }


    }

    // end of MPI
    MPI_Finalize();


    return 0;
}




