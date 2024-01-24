#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "data.h"
/* arguments supplied from the test script  */
data_struct *get_data_struct(char data[]) { 
    data_struct *d = malloc(sizeof(data_struct));
    d->rows = 0;
    d->cols = 0;
    FILE* myfile = fopen(data, "r");
    /* check the test script for the files */
    if(myfile == NULL) {
        printf("Error: The file you entered could not be found.\n");
        exit(EXIT_FAILURE);
    }
  
/* Count the number of rows and coloumns */
/* Matrix nxn */
    int ch = 0;
    do {
        ch = fgetc(myfile);
        
        
        if(d->rows == 0 && ch == '\t')
            d->cols++;
        
        
        if(ch == '\n')
            d->rows++;
            
    } while (ch != EOF);
    

/* Populate the data struct and allocate memory */   
    d->cols++;
    
    d->data_point = calloc(d->rows, sizeof(double*)); 
    int i;
    for(i=0; i < d->rows; ++i)
        d->data_point[i]=calloc(d->cols, sizeof(double));
       
    
    rewind(myfile);
    int x,y;
    
/* The the data and return the data structure */

    for(x = 0; x < d->rows; x++) {
        for(y = 0; y < d->cols; y++) {
            if (!fscanf(myfile, "%lf", &d->data_point[x][y])) 
            break;
        }
    }
    
    fclose(myfile);

    return d;
}

void print_data(data_struct *data_to_print){
	for(int i = 0; i < data_to_print->rows; i++){
		for(int j = 0; j< data_to_print->cols; j++){
			printf("%f ", data_to_print->data_point[i][j]);
		}
		printf("\n");
	}
}

/* Check how allocated data is free */
void free_data(data_struct *data_to_free) {

for(int i = 0; i < data_to_free->rows; i++) {
        free(data_to_free->data_point[i]);
    }
    free(data_to_free->data_point);
    free(data_to_free);

}

