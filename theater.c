/******************************************************************************
 * Final Project: Theater Ticket Sales
 * Operating Systems - COP 4600-001
 * 
 * Authors: Michael Fuentes, Paul Hafer, and Steven Sadhwani
 * Date: 11-13-2017
 *****************************************************************************/

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

// Declare semaphores
sem_t available;
sem_t sold;
sem_t mutex;

// Struct to represent tickets
typedef struct {
	char *movie;
} ticket;

// Declare global variables
int tickets_avail = 0;
int myTicketNum = 0;

// Declare producer / consumer functions
void * refund();
void * buy();

//*****************************************************************************
//
//	Main: Runs the program.
//
//*****************************************************************************
int main(int argc, char *argv[]) {
	// Allows user input to specify amount of tickets available.
	if (argc == 1) {
		myTicketNum = MAX_TICKETS; // Without command line arguments, set to MAX
	}
	else myTicketNum = atoi(argv[1]);	// With command line arguments
	tickets_avail = myTicketNum;

	printf("\nTheater Box Office is now open!\t\t\t Tickets available: %d\n\n\n", myTicketNum);
	
	// Initialize semaphores
	sem_init(&available, 0, myTicketNum);
	sem_init(&sold, 0, 0);
	sem_init(&mutex, 0, 1);

	pthread_t refunder;
	pthread_t buyer;

	pthread_attr_t attr[1];

	// Required to schedule thread independently.
	pthread_attr_init(&attr[0]);
	pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);

	// Handle pthread initialization errors
	if(pthread_create(&refunder, NULL, refund, attr)) {
		printf("Error creating thread\n");
	}
	if (pthread_create(&buyer, NULL, buy, attr)) {
		printf("Error creating thread\n");
	}

	// Wait for both threads to finish
	pthread_join(refunder, NULL);
	pthread_join(buyer, NULL);

	printf("\nTickets Sold Out!\n\n");

	// Delete all semaphores
	sem_destroy(&available);
	sem_destroy(&sold);
	sem_destroy(&mutex);
	sem_destroy(&buy_choice);
	sem_destroy(&refund_choice);

	return 0;	// End of program
}

//*****************************************************************************
//
//	Refund: Buys tickets back and increases the total number of tickets
//	available.
//					Random ticket return range: 1 - 5.
//
//*****************************************************************************
void *refund() {
	while(tickets_avail) {	// While there are still tickets available
		nanosleep((const struct timespec *)1, NULL);	// Sleep briefly
		int ticketsReturn = (rand() % 4) + 1;		// Random number of tickets

		// Synchronize with semaphores
		sem_wait(&sold);
		sem_wait(&mutex);
		if(tickets_avail != 0)
		{
			tickets_avail += ticketsReturn;
			if (ticketsReturn > 0) {
				printf("%d tickets returned \t\t\t Total tickets available = %d\n", ticketsReturn, tickets_avail);
			}
		}
		// Release the semaphores
		sem_post(&mutex);
		sem_post(&available);
	}
	pthread_exit(NULL);	// Exit the thread
}

//*****************************************************************************
//
//	Buy: Sells tickets and decreases the total number of tickets available.
//					Random ticket sale range: 1 - 9.
//
//*****************************************************************************
void *buy() {

	while(tickets_avail > 0) {		// While there are still tickets available
		nanosleep((const struct timespec *)1, NULL);	// Sleep briefly
		int ticketsSold = (rand() % 8) + 1;		// Random number of tickets

		// If there are not enough tickets available to sell, skip transaction.
		if(tickets_avail < ticketsSold)
		{
			printf("Sorry, there are only %d tickets left for sale. We cannot sell you %d tickets.\n", tickets_avail, ticketsSold);
			continue;
		}
		// Synchronize with semaphores
		sem_wait(&available);
		sem_wait(&mutex);
		tickets_avail-= ticketsSold;	// Decrement the tickets available
		if (ticketsSold > 0) {
			printf("%d tickets sold\t\t\t\t Total tickets available = %d\n",ticketsSold, tickets_avail);
		}
		// Release the semaphores
		sem_post(&mutex);
		sem_post(&sold);
	}	
	pthread_exit(NULL);	// Exit the thread
}