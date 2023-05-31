#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main() {

    double start_time, end_time;

    
    start_time = omp_get_wtime();

    int i, n, threadNum;
    double* array;
    double totalSum = 0.0, mean, totalSquaredDiff = 0.0, variance, stdDev;

    printf("Enter the number of Theads: ");
    scanf_s("%d", &threadNum);
    omp_set_num_threads(threadNum);
    printf("Enter the number of elements: ");
    scanf_s("%d", &n);
    array = (double*)malloc(sizeof(double) * n);


    printf("Enter the elements:\n");
    for (i = 0; i < n; i++) {
        scanf_s("%lf", &array[i]);
        
    }

#pragma omp parallel for reduction(+:totalSum)
    for (i = 0; i < n; i++) {
#pragma omp atomic
        totalSum += array[i];
    }

    mean = totalSum / n;

#pragma omp parallel for reduction(+:totalSquaredDiff)
    for (i = 0; i < n; i++) {
        totalSquaredDiff += (array[i] - mean) * (array[i] - mean);
    }

    variance = totalSquaredDiff / n;
    stdDev = sqrt(variance);

    printf("Mean = %lf, Variance = %lf, Standard deviation = %lf\n", mean, variance, stdDev);
    end_time = omp_get_wtime();
    double total_time=end_time-start_time;
    printf("%f", total_time);
    free(array);

    return 0;
}
