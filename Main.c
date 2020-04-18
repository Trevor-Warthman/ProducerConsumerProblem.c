
//Trevor Warthman
//CSE 2431
//Lab 2
//To run this program, make sure all files are attatched and compile with gcc <files> -lpthread -lrt 
//run with three command line arguments

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

typedef int buffer_item;
#define BUFFER_SIZE 5

int buffer_elements;

//Declare thread functions

void *producer(void *param); 
void *consumer(void *param);

/* Initialize the buffer as an array of buffer_items */
buffer_item buffer[BUFFER_SIZE];

//Set up Pthreads
	pthread_t thread_id;       //get the thread id
	pthread_attr_t attr; 

/*Initialize Semaphores */
	sem_t emptySem;
	sem_t fullSem;

/*Initialize Mutex */
	pthread_mutex_t mutex;


//INSERT AND REMOVE FUNCTIONS

//Insert Function
//Adds item to the buffer if empty
int insert_item(buffer_item item) {

	/* insert an item into buffer */ 
	/* return 0 if successful, otherwise
 	return -1 indicating an error condition */
	if(buffer_elements < BUFFER_SIZE){
		//Buffer was empty
		//Add item to the buffer
		buffer[buffer_elements] = item;
		//Increment number of elements
		buffer_elements = buffer_elements + 1;
		printf("producer produced %d\n", item);
		return 0;
	}
	else{
		//Buffer was full
		return -1;
	}
}

//Remove Function
//Replaces item with the removed element from the buffer if buffer has an element
int remove_item(buffer_item *item) {
	printf("removing item");
	/* remove an object from buffer and placing it in item*/ 
	/* return 0 if successful, otherwise
 	return -1 indicating an error condition */
 	if(buffer_elements > 0){
 		//Buffer had an element
 		//Remove item from buffer
 		*item = buffer[buffer_elements - 1];
 		int random = buffer[buffer_elements - 1];
      	buffer_elements = buffer_elements - 1;
      	printf("consumer consumed %d\n", random);
      	return 0;
 	}
 	else{
 		//Buffer was empty already
 		return -1;
 	}
}


//PRODUCER AND CONSUMER THREAD FUNCTIONS

//Producer Code
void *producer(void *param) {

	buffer_item rando;
	//Loop Infinitly
	while (1) {
		/* sleep for a random period of time */
		int sleepTime = (rand() / 1000000000);
		//divide by a lot to shorten the time so that it can finish before the program terminates
		sleep(sleepTime);

		/* generate a random number */
		rando = rand();

		//Aquire locks
		pthread_mutex_lock(&mutex);
		sem_wait(&emptySem);

		//Run insert item with the random number and generate a error if need be
		if (insert_item(rando) < 0)
		printf("Producer Error: Buffer was full\n"); // report error condition

		//Release and signal that it is now full
		sem_post(&fullSem);
      	pthread_mutex_unlock(&mutex);
	}
}

//Consumer Code
void *consumer(void *param) {
	buffer_item rando;
	//Loop Infinitly
	while (1) {
		/* sleep for a random period of time */
		int sleepTime = (rand() / 10000000000);
		//divide by a lot to shorten the time so that it can finish before the program terminates
		sleep(sleepTime);

		//Aquire locks
		sem_wait(&fullSem);
		pthread_mutex_lock(&mutex);

		//Remove item and generate error contition if need be, put it in rand
		if (remove_item(&rando) < 0)
		printf("Consumer Error: Buffer was already empty\n"); // report error condition

		//unlock and signal
		sem_post(&emptySem);
		pthread_mutex_unlock(&mutex);
	}
}




//Main Function
int main(int argc, char*argv[]) {
/* 1. Get command line arguments argv[1], argv[2], argv[3] */

	int c1 = atoi(argv[0]);
	int c2 = atoi(argv[1]);
	int c3 = atoi(argv[2]);

/* 2. Initialize buffer, mutex, semaphores, and other global vars */

	//Init pthread
	pthread_attr_init(&attr);

	//Create Mutex
	pthread_mutex_init(&mutex, NULL);

	/* create the semaphores and initialize them to the buffer size and 0 */
	sem_init(&emptySem, 0, BUFFER_SIZE);
	sem_init(&fullSem, 0, 0);

	//Init a counter of number of elements in the buffer of each thread
	buffer_elements = 0;

/* 3. Create producer thread(s) */
	int producers = c2;
	while(producers > 0){
		//Create a producer thread
		pthread_create(&thread_id,&attr,producer,NULL);
		//Decrement
		producers = producers - 1;
	}

/* 4. Create consumer thread(s) */
	int consumers = c3;
	while(consumers > 0){
		//Create a consumer thread
		pthread_create(&thread_id,&attr,consumer,NULL);
		//Decrement
		consumers = consumers - 1;
	}

/* 5. Sleep */
	sleep(c1);
/* 6. Release resources, e.g. destroy mutex and semaphores */
	pthread_mutex_destroy(&mutex);
	sem_destroy(&emptySem);
	sem_destroy(&fullSem);
/* 7. Exit */
	printf("Goodbye!\n");
	exit(0);
}
