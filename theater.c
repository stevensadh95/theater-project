/*
*
* Term project: Theater Ticket sales.
*	Michael Fuentes, Paul Hafer, Steven sadhwani
*
*
*/

#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_TICKETS 5000

sem_t available;
sem_t sold;
sem_t mutex;
sem_t buy_choice;
sem_t refund_choice;

//struct to represent tickets
typedef struct {
	char *movie;
	int hour;
	int minute;
} ticket;

//int tickets_avail = MAX_TICKETS;
int tickets_avail = 0;
int done = 0;
int myTicketNum = 0;
void * refund();
void * buy();

//*****************************************************************************
//
//	Main: Runs the program.
//
//*****************************************************************************
int main(int argc, char *argv[]){
	//Allows user input to specify amount of tickets available.
	if (argc == 1) {
		myTicketNum = MAX_TICKETS;
	}
	else myTicketNum = atoi(argv[1]);
	tickets_avail = myTicketNum;

	printf("\nTheater Box Office is now open!\t\t\t Tickets available: %d\n\n\n", myTicketNum);
	sem_init(&available,0,myTicketNum);
	sem_init(&sold, 0,0);
	sem_init(&mutex, 0,1);
	sem_init(&buy_choice, 0,0);
	sem_init(&refund_choice,0,0);

	pthread_t refunder;
	pthread_t buyer;

	pthread_attr_t attr[1];

	// Required to schedule thread independently.
	pthread_attr_init(&attr[0]);
	pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);


	if(pthread_create(&refunder, NULL, refund, attr)){
		printf("Error creating thread\n");
	}
	if (pthread_create(&buyer, NULL, buy, attr)){
		printf("Error creating thread\n");
	}

//Wait for the threads to finish
pthread_join(refunder, NULL);
pthread_join(buyer, NULL);

printf("\nTickets Sold Out!\n\n");

// delete semaphores
sem_destroy(&available);
sem_destroy(&sold);
sem_destroy(&mutex);
sem_destroy(&buy_choice);
sem_destroy(&refund_choice);


return 0;
}

//*****************************************************************************
//
//	Refund: Buys tickets and raises the total number of tickets available.
//					Random ticket return from 1-9.
//
//*****************************************************************************
void *refund(){
	while(tickets_avail)
	{
		nanosleep((const struct timespec *)1, NULL);
		int ticketsReturn = (rand()%4) + 1;

		sem_wait(&sold);
		sem_wait(&mutex);
		if(tickets_avail != 0)
		{
			tickets_avail += ticketsReturn;
			if (ticketsReturn > 0) {
				printf("%d tickets returned \t\t\t Total tickets available = %d\n", ticketsReturn, tickets_avail);
			}
		}
		sem_post(&mutex);
		sem_post(&available);
	}
	pthread_exit(NULL);
}

//*****************************************************************************
//
//	Refund: Sells tickets and lowers the total number of tickets available.
//					Random ticket sale from 1-5.
//
//*****************************************************************************
void *buy() {

	while(tickets_avail > 0){
		nanosleep((const struct timespec *)1, NULL);
		int ticketsSold = (rand()%8) + 1;

		//If not enough tickets available to sell, skip transaction.
		if(tickets_avail < ticketsSold)
		{
			printf("Sorry, there are only %d tickets left for sale. We cannot sell you %d tickets.\n", tickets_avail, ticketsSold);
			continue;
		}

		sem_wait(&available);
		sem_wait(&mutex);
		tickets_avail-= ticketsSold;
		if (ticketsSold > 0) {
			printf("%d tickets sold\t\t\t\t Total tickets available = %d\n",ticketsSold, tickets_avail);
		}
		sem_post(&mutex);
		sem_post(&sold);
	}
	done = 1;
	pthread_exit(NULL);
}
