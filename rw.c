/* 1st readers-writers problem
No readers be kept waiting unless the writer thread
OBTAINED permission to use the shared object. */

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include"rw.h"

struct message {
	obj_item shaObj; /* a char variable */
	pthread_mutex_t rw_mutex;/* reader-writer mutex */
	pthread_mutex_t mutex;/* mutex within readers in execution queue */
	int read_count;/* indicating nums of readers, at border conditions */
};

void *reader(void *param);
void *writer(void *param);

int main(int argc, char const *argv[])
{
	
	pthread_t tidW, tidR1, tidR2; /* one writer, two readers for instance */
	struct message m;
	m.shaObj = 'a';
	/* Initialize mutexs and other messages*/
	pthread_mutex_init(&m.rw_mutex,NULL);
	pthread_mutex_init(&m.mutex,NULL);
	m.read_count = 0;
	/* Initialize and create threads */
	pthread_create(&tidW, NULL, writer, (void *)&m);
	pthread_create(&tidR1, NULL, reader, (void *)&m);
	pthread_create(&tidR2, NULL, reader, (void *)&m);
	/* wait for threads to terminate */
	pthread_join(tidW,NULL);
	pthread_join(tidR1,NULL);
	pthread_join(tidR2,NULL);
	return 0;
}

void *writer(void *param) {
	struct message *m = (struct message *)param;
	/* get wr_mutex */
	for (int i = 0; i < 25; i++)
	{
		pthread_mutex_lock(&m->rw_mutex);
		// printf("The writer has captured rw_mutex\n");
		// printf("read_count from writer = %d\n", m->read_count);
		m->shaObj += 1; 
		printf("Writer has updated the charactor!(%c)\n", m->shaObj);
		pthread_mutex_unlock(&m->rw_mutex);
		// printf("rw_mutex has been released from the writer\n");

		sleep(rand() % 5);
	}
}

void *reader(void *param) {
	struct message *m = (struct message *)param;
	/* add 1 to waiting list */
	pthread_mutex_lock(&m->mutex);
	m->read_count += 1;
	// printf("read_count from reader = %d\n", m->read_count);
	for(int i = 0; i < 26; i++) {
		/* check rw_mutex */
		if (m->read_count == 1) 
			pthread_mutex_lock(&m->rw_mutex);
		pthread_mutex_unlock(&m->mutex);
		/* exit mutex */

		printf("A reader from %ld reads the charactor %c.\n", pthread_self(), m->shaObj);
		
		pthread_mutex_lock(&m->mutex);
		if (--m->read_count == 0) {
			pthread_mutex_unlock(&m->rw_mutex);
			// printf("rw_mutex has been released from readers\n");
		}
		pthread_mutex_unlock(&m->mutex);
		/* mutex lock needed when altering count list */
		
		sleep(rand() % 5);
	}
}