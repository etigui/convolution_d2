/*****************************
Auteur:     Etienne Guignard
Date:       25.08.2016
Name:      	main.h
Projet:     Convolution 2D
Version:    1.0
*******************************/

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "ppm.h"
#include "kernel.h"
#include "queue.h"

#define MAX_THREADS 63143 //Max thread on my PC => cat /proc/sys/kernel/threads-max

void boss();
void worker();
int get_work(struct queue_work_struct *work, int indice);
void convolution(struct queue_work_struct *work);

#endif /* MAIN_H_ */
