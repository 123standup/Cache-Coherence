#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "data.h"
#include <omp.h>
#include "timer.h"

#define chunk_size 16

void transposeMatrix(data_struct *matrix) {
    unsigned int rows = matrix->rows;
    unsigned int cols = matrix->cols;
    double **data = matrix->data_point;

    double **transposed_data = (double **)malloc(cols * sizeof(double *));
    for (unsigned int i = 0; i < cols; i++) {
        transposed_data[i] = (double *)malloc(rows * sizeof(double));
    }

    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            transposed_data[j][i] = data[i][j];
        }
    }

    matrix->rows = cols;
    matrix->cols = rows;
    for (unsigned int i = 0; i < rows; i++) {
        free(data[i]);
    }
    free(data);
    matrix->data_point = transposed_data;
}

int main(int argc, char **argv)
{
    if(argc != 4){
        printf("Usage: <num_thread> <vec_a> <vec_b>.\n");
        exit(EXIT_FAILURE);
    }

    // int numThreads = omp_get_max_threads();
    // convinient for use in partitioning
    int numThreads = atoi(argv[1]);
    omp_set_num_threads(numThreads);
        
    struct timespec start;
    start_timer(&start);

    data_struct *d_1 = get_data_struct(argv[2]);
    data_struct *d_2 = get_data_struct(argv[3]);
    transposeMatrix(d_2); // transpose

    stop_timer(&start);
    fprintf(stderr, " (reading input)\n");

    if(d_1->cols != d_2->cols){ // transposed matrix should have same cols
        printf("ERROR: Matrix dimension mismatch.\n");
        exit(EXIT_FAILURE);
    }

    data_struct *d_3 = malloc(sizeof(data_struct));
    d_3->rows = d_1->rows;
    d_3->cols = d_2->rows;

    d_3->data_point = calloc(d_3->rows, sizeof(double*));
    int i;
    for(i=0; i < d_3->rows; ++i)
        d_3->data_point[i]=calloc(d_3->cols, sizeof(double));

    start_timer(&start);
    /* Implementation of Matrix Multiplication  openMP */
    int thread_cal = d_3->rows / numThreads;
    if(thread_cal == 0){
        thread_cal = d_3->rows % numThreads;
    }

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

    int B = 4; // Block size
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int x, y, z;
        int yy, zz;
        for (yy = 0; yy < d_3->cols; yy += B) {
            for (zz = 0; zz < d_1->cols; zz += B) {
                for(x = 0 + tid * thread_cal; x < (tid + 1) * thread_cal; x ++) {
                    for(y = yy; y < min(yy + B, d_3->cols); y ++) {
                        for(z = zz; z < min(zz + B, d_1->cols); z ++) {
                            d_3->data_point[x][y] += d_1->data_point[x][z] * d_2->data_point[y][z];
                        }
                    }
                }
            }
        }
    }
    // calculating the remainders 
    if(numThreads * thread_cal < d_3->rows) {
    	for(int x = numThreads * thread_cal; x < d_3->rows; x++) {
            for(int y = 0; y < d_3->cols; y++) {
                for(int z = 0; z < d_1->cols; z++) {
                    d_3->data_point[x][y] += d_1->data_point[x][z] * d_2->data_point[y][z];
                }
            }
    	}
    }
    stop_timer(&start);
    fprintf(stderr, " (calculating answer)\n");
    
    start_timer(&start);
    /* Printing output */
	print_data(d_3);
    stop_timer(&start);
    fprintf(stderr, " (printing output)\n");
        free_data(d_1);
        free_data(d_2);
        free_data(d_3);
}
