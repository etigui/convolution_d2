/*****************************
Auteur:     Etienne Guignard
Date:       25.08.2016
Name:       queue.c
Projet:     Convolution 2D
Version:    1.0
*******************************/

#include "queue.h"

//Global queue
struct queue_struct thread_queue;

//Mutex to protect queue initialization
pthread_mutex_t queue_init_mtx = PTHREAD_MUTEX_INITIALIZER;

/**
 * Initialisation queue
 * @param queue: global queue with thread info
 * @param max_len: max data in queue
 * @return void
 */
void queue_init(struct queue_struct *queue, int max_len){

	//Lock to make sure two threads don't init the queue
	pthread_mutex_lock(&queue_init_mtx);

	//Init the queue
	pthread_mutex_init(&queue->queue_lock, NULL);
	pthread_cond_init(&queue->boss_cond, NULL);
	pthread_cond_init(&queue->worker_cond, NULL);
	queue->queue_exit = 0;
	queue->boss_waiting = 0;
	queue->worker_waiting = 0;
	queue->queue_len = 0;
	queue->max_queue_len = max_len;
	queue->head = NULL;
	queue->tail = NULL;
	pthread_mutex_unlock(&queue_init_mtx);
}

/**
 * Add element in queue
 * @param queue: global queue with thread info
 * @param work: work to add in queue
 * @return void
 */
void queue_add(struct queue_struct *queue, struct queue_work_struct *work){

	//Lock "queue_add function" -> not other thread can use it
	pthread_mutex_lock(&queue->queue_lock);

	//If the queue is full, wait until we can add more work
	while (queue->queue_len == queue->max_queue_len) {
		queue->boss_waiting++;
		pthread_cond_wait(&queue->boss_cond, &queue->queue_lock);
		queue->boss_waiting--;
	}

	//Add the work to the queue
	//If the queue is not full -> otherwise we will wait before
	work->next = NULL;
	if (queue->queue_len == 0 ) {
		queue->head = queue->tail = work;
	} else {
		queue->tail->next = work;
		queue->tail = work;
	}
	queue->queue_len++;

	//If at least one worker is waiting signal it (wake-up one)
	if (queue->worker_waiting > 0 ){
		pthread_cond_signal(&queue->worker_cond);
	}

	pthread_mutex_unlock(&queue->queue_lock);
}

/**
 * Remove work from queue
 * @param queue: global queue with thread info
 * @return work to worker
 */
struct queue_work_struct * queue_remove(struct queue_struct *queue){

	struct queue_work_struct *work;

	//Lock "queue_remove function", as that not an other thread can it
	pthread_mutex_lock(&queue->queue_lock);

	//Is there work on the queue
	while (queue->queue_len == 0) {

		//If terminate -> unlock mutex and terminate the calling thread
		if (queue->queue_exit) {
			pthread_mutex_unlock(&queue->queue_lock);
			pthread_exit(NULL);
		}

		//Wait for data
		queue->worker_waiting++;
		pthread_cond_wait(&queue->worker_cond, &queue->queue_lock);
		queue->worker_waiting--;
	}

	//Take the work out of queue
	work = queue->head;
	queue->head = work->next;
	queue->queue_len--;

	//If the queue is empty set the tail to null
	if (queue->queue_len == 0){
		queue->tail = NULL;
	}

	//If boss thread is waiting
	if (queue->boss_waiting > 0){
		pthread_cond_signal(&queue->boss_cond);
	}

	pthread_mutex_unlock(&queue->queue_lock);
	return work;
}

/**
 * When the boos stop -> exit thread
 * @param queue: global queue with thread info
 * @return void
 */
void queue_terminate_workers(struct queue_struct *queue)
{
	pthread_mutex_lock(&queue->queue_lock);

	//Set to 1 to terminate queue (thread)
	queue->queue_exit = 1;

	//Wake-up all waiting workers to terminate
	(void) pthread_cond_broadcast(&queue->worker_cond);
	(void) pthread_mutex_unlock(&queue->queue_lock);
}
