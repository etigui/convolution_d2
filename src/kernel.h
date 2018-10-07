/*****************************
Auteur:     Etienne Guignard
Date:       25.08.2016
Name:       kernel.h
Projet:     Convolution 2D
Version:    1.0
*******************************/

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct kernel_t{
    unsigned int size;		//Kernel size
    double *data;  			//Content of the filter
}kernel_t;

kernel_t* getKernel(char *filename);

#endif /* KERNEL_H_ */
