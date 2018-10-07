/*****************************
Auteur:     Etienne Guignard
Date:       25.08.2016
Name:       queue.h
Projet:     Convolution 2D
Version:    1.0
*******************************/

#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <errno.h>
#include <malloc.h>
#include "kernel.h"
#include "ppm.h"

#define MAX_QUEUE 8

//Queue_work_struct contain all the necessary information for the worker to process the request
//I decided to put global element such as mentioned at the next struct to ensure no extra memory allocation
typedef struct queue_work_struct {
  struct queue_work_struct *next;
  int indice;
}queue_work_struct;

//Queue_struct contain element to manage the queue
//and global element for the thread like image, kernel...
typedef struct queue_struct {
  //int valid;
  pthread_mutex_t queue_lock;		//Mutex lock the queue
  pthread_cond_t boss_cond;			//Wait/signal boss (not)give work (getWork->add_queue)
  pthread_cond_t worker_cond;		//Wait/signal worker to (not) process work (remove_queue)
  struct queue_work_struct *head;	//Work from head
  struct queue_work_struct *tail;	//Work from tail
  int queue_exit;					//if 1->exit
  int boss_waiting;					//Used to check if boss waiting (add_work)
  int worker_waiting;				//Used to check if worker waiting (remove_work)
  int queue_len;					//Actual queue lenght
  int max_queue_len;				//Max queue lenght
  int kernel_size;					//Kernel size
  img_t *base_ppm_img;				//Base image from file
  img_t *new_ppm_img;				//New image to write into
  kernel_t* kernel;					//Kernel struct
  int nb_thread;					//Max thread defined by user
  int img_size;						//Max image size -> width*heigt
  int nb_pixel;						//Max pixel to process by worker
}queue_struct;

void queue_init(struct queue_struct *queue, int max_len);
void queue_add(struct queue_struct *queue, struct queue_work_struct *elem);
struct queue_work_struct * queue_remove(struct queue_struct *queue);
void queue_terminate_workers(struct queue_struct *queue);

#endif /* QUEUE_H_ */
