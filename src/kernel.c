/*****************************
Auteur:     Etienne Guignard
Date:       25.08.2016
Name:       kernel.c
Projet:     Convolution 2D
Version:    1.0
*******************************/

#include "kernel.h"

/**
 * Get kernel value from file
 * @param filename: file name
 * @return kernel size and data
 */
kernel_t* getKernel(char *filename){
    kernel_t* kernel;
	unsigned int size;
	int matches;

	//Read file
    FILE *file = fopen(filename, "r");
    if(!file){
        printf("Error: cannot open the file.\n");
    }

    //Get size the kernel file
	matches = fscanf(file, "%u", &size);
	if (matches != 1){
        printf("Error: loading kernel\n");
	}

	//Alloc kernel
    kernel = (kernel_t*) malloc(sizeof(kernel_t));
    kernel->size = size;
    kernel->data = calloc(1, (size * size) * sizeof(double));

    //Get number in the kernel file
    int i;
    for(i = 0;i < size * size;i++){
		double nb = 0.0;
		matches = fscanf(file, "%lf", &nb);
		if (matches != 1){
			 printf("Error: loading kernel\n");
		}
		kernel->data[i] = nb;
    }

	fclose(file);
    return kernel;
}
