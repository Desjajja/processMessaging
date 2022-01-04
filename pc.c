#include "buffer.h"
#include<stdlib.h> /* required for rand() */
#include<unistd.h> /* required for sleep() */
#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>

struct message
{
	buffer_item buffer[BUFFER_SIZE];
	sem_t empty;
	sem_t full;	
	pthread_mutex_t mutex;
};

void *producer(void *param);

void *consumer(void *param);



void remove_element(buffer_item *item);

int main(int argc, char const *argv[])
{
	/* thread identifiers */
	pthread_t tidP, tidC;
	/* struct holding all messages transporting between threads */
	struct message m;

	/* initialize semaphores and mutex */
	sem_init(&m.empty, 0, BUFFER_SIZE);
	sem_init(&m.full, 0, 0);
	pthread_mutex_init(&m.mutex, NULL);

	/* create threads */
	pthread_create(&tidP, NULL, &producer, &m);
	pthread_create(&tidC, NULL, &consumer, &m);
	
	/* wait the threads to terminate */
	pthread_join(tidP, NULL);
	pthread_join(tidC, NULL);

	return 0;
}



void *producer(void *param) {
	struct message *ptr_m;

	ptr_m = (struct message *) param;
	buffer_item *buffer = ptr_m->buffer;
	sem_t *p_empty = &ptr_m->empty;
	sem_t *p_full = &ptr_m->full;
	pthread_mutex_t *p_mutex = &ptr_m->mutex;
	buffer_item item;
	int idx;
	
	for (int i = 0; i < 12; i++) {
		/* sleep for a random period */
		sleep(rand() % 5);
		/* generate an item */
		item = rand() % 100;
		/* wait for an empty space */
		sem_wait(p_empty);
		/* lock race section */
		pthread_mutex_lock(p_mutex);
		/* insert an element */
		/* get the insertion index */
		sem_getvalue(p_empty, &idx);
		buffer[idx] = item;
		printf("Producer produced an item:%d\n", item);
		sem_post(p_full);
		pthread_mutex_unlock(p_mutex);
	}
}

	void *consumer(void *param) {
		struct message *ptr_m;
		ptr_m = (struct message *) param;
		buffer_item *buffer = ptr_m->buffer;
		sem_t *p_empty = &ptr_m->empty;
		sem_t *p_full = &ptr_m->full;
		pthread_mutex_t *p_mutex = &ptr_m->mutex;
		buffer_item item;
		int idx;

		for (int i = 0; i < 12; i++) {
		/* sleep for a random period */
		sleep(rand() % 5);
		/* generate an item */
		item = rand() % 100;
		/* wait for state of fullness */
		sem_wait(p_full);
		/* lock race section */
		pthread_mutex_lock(p_mutex);
		/* insert an element */
		/* get the insertion index */
		sem_getvalue(p_empty, &idx);
		item = buffer[idx];
		printf("Consumer consumed an item:%d\n", item);
		sem_post(p_empty);
		pthread_mutex_unlock(p_mutex);
	}

}