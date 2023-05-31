#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>
#include <string.h>

const int  MAX_MESSAGE_SIZE = 10000;

int main(int argc, char* argv[])
{
    int rank, size;
    int i=0, j=0;
    char originalMessage[MAX_MESSAGE_SIZE];
    int messageLength=0;

    char *localMessage;
    char *remainingMessage;

    int remainingSize=0;
    int partitionSize=0;

    char *encryptedMessage;

    int shift=0;


    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    localMessage =(char *) malloc(sizeof (char));

    if(rank==0){

        messageLength=0;
        FILE* fp;
        char c;
        char filename[150];
        printf("Enter input filename: ");
        scanf("%s", &filename);
        fp = fopen(filename, "r");
        if (fp == NULL) {
            printf("Error opening file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        while ((c = fgetc(fp)) != EOF) {
            originalMessage[messageLength]=c;
            messageLength++;
        }
        if(originalMessage[messageLength-1]=='\n')
            originalMessage[messageLength-1]='\0',messageLength--;
        else
            originalMessage[messageLength]='\0';
        fclose(fp);

        printf("KEY/SHIFT: ");
        scanf("%d", &shift);

        remainingSize=messageLength%size;
        partitionSize=messageLength/size;
        encryptedMessage=(char *) malloc((messageLength)* sizeof(char ));
        remainingMessage=(char *) malloc((remainingSize)* sizeof(char ));
        for(i=messageLength-remainingSize,j=0;i<messageLength;j++,i++){
            remainingMessage[j]=originalMessage[i];
        }
        remainingMessage[j]='\0';


    }
    MPI_Bcast(&shift, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&partitionSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&remainingSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&remainingMessage, remainingSize, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Scatter(originalMessage, partitionSize, MPI_CHAR, localMessage,partitionSize, MPI_CHAR, 0, MPI_COMM_WORLD);

    // do encryption

    char *encMess=(char *) malloc(sizeof(char )*partitionSize);
    for(i=0;i<partitionSize;i++){
        if(isalpha(localMessage[i])){
            char c= ((toupper(localMessage[i])-'A'+shift)%26)+'A';
            encMess[i]=c;
        } else{
            encMess[i]= toupper(localMessage[i]);
        }
    }
    encMess[i]='\0';



    MPI_Gather(encMess, partitionSize, MPI_CHAR, encryptedMessage, partitionSize, MPI_CHAR, 0, MPI_COMM_WORLD);

    if(rank==0){


        char *finalMessage=(char *) malloc(sizeof(char )*messageLength);
        char *encRem=(char *) malloc(sizeof(char )*remainingSize);
        for(i=0;i<remainingSize;i++){
            if(isalpha(remainingMessage[i])){
                char c= ((toupper(remainingMessage[i])-'A'+shift)%26)+'A';
                encRem[i]=c;
            }else{
                encRem[i]=remainingMessage[i];
            }
        }
        encRem[i]='\0';

        for(i=0;i<partitionSize*size;i++){
            finalMessage[i]=encryptedMessage[i];
        }
        j=0;
        while (i<messageLength){
            finalMessage[i++]=(encRem[j++]);
        }
        finalMessage[i]='\0';

        FILE* fp;
        char filename[100];
        fp = fopen("output.txt", "w");
        if (fp == NULL)
        {
            printf("Error opening file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fprintf(fp, "%s", finalMessage);
        fclose(fp);

    }




    MPI_Finalize();
    return 0;
}


