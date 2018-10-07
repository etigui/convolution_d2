/*****************************
Auteur:     Etienne Guignard
Date:       25.08.2016
Name:       main.c
Projet:     Convolution 2D
Version:    1.0
*******************************/

#include "main.h"

//Global queue
extern struct queue_struct thread_queue;

/**
 * Program's entry point.
 * @param argc: number of arguments.
 * @param argv: array of strings storing the arguments.
 * @return program's exit code.
 */
int main(int argc, char **argv){

	if(argc != 5){
		printf("You must specify 4 arguments : <image source path> <number of threads> <filter path> <image destination path>\n");
		printf("filter path by name:\n");
		printf("\tkernel\\identity.k (3x3)\n");
		printf("\tkernel\\blur.k     (3x3)\n");
		printf("\tkernel\\sharpen.k  (3x3)\n");
		printf("\tkernel\\edge.k     (3x3)\n");
		printf("\tkernel\\gauss.k    (5x5)\n");
		printf("\tkernel\\unsharp.k  (5x5)\n");
		return EXIT_FAILURE;
	}

	//Read command line arguments
	char* src_img = argv[1];
	unsigned int nb_thread = atoi(argv[2]);

	if(nb_thread >= MAX_THREADS){
		printf("Error: too many threads\n");
		return EXIT_FAILURE;
	}
	char* filter = argv[3];
	char* dest_img = argv[4];

	//Start clock
	struct timespec run, finish;
	clock_gettime(CLOCK_MONOTONIC, &run);

	//Import kernel from file as 1D array
	kernel_t* kernel = getKernel(filter);
	if(kernel == NULL){
		printf("Error: getKernel error\n");
		return EXIT_FAILURE;
	}
	thread_queue.kernel = kernel;

	//Set kernel size from file
	thread_queue.kernel_size = thread_queue.kernel->size;

	//Import image source from file as 1D array
	img_t*  base_ppm_img = load_ppm(src_img);
	if(base_ppm_img == NULL){
		printf("Error: load_ppm base error\n");
		return EXIT_FAILURE;
	}
	thread_queue.base_ppm_img = base_ppm_img;

	//Generate new image content to put the convolution result (same size as the base one)
	img_t* new_ppm_img = alloc_img(thread_queue.base_ppm_img->width, thread_queue.base_ppm_img->height);
	if(new_ppm_img == NULL){
		printf("Error: alloc_img new error\n");
		return EXIT_FAILURE;
	}
	thread_queue.new_ppm_img = new_ppm_img;

	//Set "max" thread
	thread_queue.nb_thread = nb_thread;

	//Total pixel in the image
	thread_queue.img_size = thread_queue.base_ppm_img->width * thread_queue.base_ppm_img->height;

	//Division of the pixel
	thread_queue.nb_pixel = ceil(thread_queue.img_size / (thread_queue.nb_thread + MAX_QUEUE));

	//Init the queue
	queue_init(&thread_queue, MAX_QUEUE);

	//Create threads
	pthread_t threads[nb_thread];
	int i;
	for (i = 0; i < nb_thread; i++) {
		if(pthread_create(&threads[i], NULL,(void *(*)())worker, NULL) != 0){
			printf("Error: thread create error\n");
			return EXIT_FAILURE;
		}
	}

	//Boss thread (main)
	boss();

	//Terminated release all work thread and terminate
	queue_terminate_workers(&thread_queue);

	//Wait the worker thread to terminate
	for (i = 0; i < nb_thread; i++) {
		if(pthread_join(threads[i], NULL) != 0){
			printf("Error: thread join error\n");
			return EXIT_FAILURE;
		}
	}

	//Generate new ppm file
	write_ppm(dest_img, thread_queue.new_ppm_img);

	//Free all memory
	//Kernel
	free(kernel->data);
	free(kernel);

	//New image
	free_img(new_ppm_img);

	//Base image
	free_img(base_ppm_img);

	//Stop clock
	clock_gettime(CLOCK_MONOTONIC, &finish);
	double elapsed_ms = 1000 * (finish.tv_sec - run.tv_sec);
	elapsed_ms += (finish.tv_nsec - run.tv_nsec) / 1000000.0;
	printf("Image convoluted in %lfms\n", elapsed_ms);

	return EXIT_SUCCESS;
}

/**
 * Get work
 * @param work: work to process
 * @param indice: where the the convolution start
 * @return if terminated.
 */
int get_work(struct queue_work_struct *work, int indice){

	//Where the convolution start
	work->indice = indice * thread_queue.nb_pixel;

	//If the image is done -> exit
	if(indice > (thread_queue.nb_thread * MAX_QUEUE)){
		return 0;
	}else{
		return 1;
	}
}

/**
 * Boss thread (main)
 * @return void
 */
void boss(){

	struct queue_work_struct *work;
	int indice, ret;
	indice = 0;

	//Get work and break when complete
	while(true){

		//Allocate a work structure
		work = (struct queue_work_struct *) malloc (sizeof(struct queue_work_struct));
		if(work == NULL){
			break;
		}

		//Get work to add to the queue
		ret = get_work(work, indice);

		//If the operation is terminated then break -> end prog
		if(ret == 0){
			free(work);
			break;
		}

		//Queue up the work
		queue_add(&thread_queue, work);
		indice++;
	}
}

/**
 * Worker thread (process convolution)
 * @return void
 */
void worker(){

	struct queue_work_struct *work;

	//Infinite loop processing the work received from the work queue
	while(true){

		//Get the next work request
		work = queue_remove(&thread_queue);

		if(work == NULL){
			printf("Error: queue_remove error\n");
			exit(EXIT_FAILURE);
		}

		//Do the convolution with given indice (works)
		convolution(work);

		//Release memory for work request
		free(work);
	}
}

/**
 * Convolution
 * @param work: work to process.
 * @return void
 */
void convolution(struct queue_work_struct *work){

	//Get indices from worker
	//It correspond at the pixel position where we have to start the convolution
    int indice = work->indice;

    //Iterate through image
    //Which k is determinate with the number of thread and the max queue
    for (int k = 0; k < thread_queue.nb_pixel; k++){
        if( (k+indice) < thread_queue.base_ppm_img->width * (thread_queue.kernel_size/2) ||
            (k+indice) > thread_queue.img_size - thread_queue.base_ppm_img->width * (thread_queue.kernel_size/2) ||
            (k+indice) % thread_queue.base_ppm_img->width == 0 ||
            ((k+indice)-(k+indice)/thread_queue.base_ppm_img->width) % (thread_queue.base_ppm_img->width-1) == 0)
            continue;


        int n = thread_queue.kernel_size/2;
        double r = 0 ,g = 0 ,b = 0;

        //Iterate over kernel
        //Process one pixel from kernel size
        for(int i = -n ; i <= n ; i++){
            for(int j = -n ; j <= n ; j++){

            	//Calculate new color for each pixel r,g,b
            	r += (thread_queue.kernel->data[(i+n)*thread_queue.kernel_size + j + n] * thread_queue.base_ppm_img->data[indice+ k + i*thread_queue.base_ppm_img->width + j].r);
            	g += (thread_queue.kernel->data[(i+n)*thread_queue.kernel_size + j + n] * thread_queue.base_ppm_img->data[indice+ k + i*thread_queue.base_ppm_img->width + j].g);
            	b += (thread_queue.kernel->data[(i+n)*thread_queue.kernel_size + j + n] * thread_queue.base_ppm_img->data[indice+ k + i*thread_queue.base_ppm_img->width + j].b);
            }
        }

        //Bound value to max 255 for red, green and blue
        if(r > 255){r = 255;}
        thread_queue.new_ppm_img->data[indice+k].r = r;
        if(g > 255){g = 255;}
		thread_queue.new_ppm_img->data[indice+k].g = g;
		if(b > 255){b = 255;}
		thread_queue.new_ppm_img->data[indice+k].b = b;
    }
}