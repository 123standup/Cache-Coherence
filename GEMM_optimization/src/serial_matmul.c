#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "data.h"
#include "timer.h"

int main(int argc, char **argv)
{
    if(argc != 3){
        printf("ERROR: Please specify only 2 files.\n");
        exit(EXIT_FAILURE);
    }
        
    struct timespec start;
    start_timer(&start);

    data_struct *d_1 = get_data_struct(argv[1]);
    data_struct *d_2 = get_data_struct(argv[2]);
    
    stop_timer(&start);
    fprintf(stderr, " (reading input)\n");

    if(d_1->cols != d_2->rows){
        printf("ERROR: Matrix dimension mismatch.\n");
        exit(EXIT_FAILURE);
    }

    data_struct *d_3 = malloc(sizeof(data_struct));
    d_3->rows = d_1->rows;
    d_3->cols = d_2->cols;

    d_3->data_point = calloc(d_3->rows, sizeof(double*));
    int i;
    for(i=0; i < d_3->rows; ++i)
        d_3->data_point[i]=calloc(d_3->cols, sizeof(double));

    start_timer(&start);
    /* Implementation of serial Matrix Multiplication */
    int x, y, z;
    for(x = 0; x < d_3->rows; x++) {
        for(y = 0; y < d_3->cols; y++) {
		for(z = 0; z < d_1->cols; z++){
            		d_3->data_point[x][y] += d_1->data_point[x][z] * d_2->data_point[z][y];
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
