/*
 *
 * Term project: theater
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


#define MAX_TICKETS 100

sem_t available;
sem_t sold;
sem_t mutex;
sem_t buy_choice;
sem_t refund_choice;

//struct to represent tickets
typedef struct {
char *movie;
int hour;
int minute
}ticket;

int tickets_avail = MAX_TICKETS;
int done = 0;
void * refund();
void * buy();
void get_tickets(ticket* ticket_list);

int main(){

sem_init(&available,0,MAX_TICKETS);
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



// something like this if we want user input.

/*
int choice = 0;

while(1) {
do{
printf("Welcome to AMC Theaters online ticketing\n\n");
printf("Please choose an option\n");
printf("1. Buy a ticket\n");
printf("2. Refund a ticket\n");
printf("3. Quit\n\n\n");

scanf("%d",&choice);
}while(choice>3 || choice <1);


switch(choice){

case 1:
sem_post(&buy_choice);
break;

case 2:
sem_post(&refund_choice);
break;

case 3:
exit(0);
}
sleep(1);
}


*/
//Wait for the threads to finish 
 pthread_join(refunder, NULL);
 pthread_join(buyer, NULL);

// delete semaphores
 sem_destroy(&available);
 sem_destroy(&sold);
 sem_destroy(&mutex);
 sem_destroy(&buy_choice);
 sem_destroy(&refund_choice);


return 0;
}

// producer
void *refund(){
 
 while(!done) 
{
nanosleep(1,NULL);
    //sem_wait(&refund_choice); if we want user input
    sem_wait(&sold);
    sem_wait(&mutex);
        
 printf("You returned a ticket\n");
 tickets_avail++;    
 sem_post(&mutex);
 sem_post(&available);
}
			
pthread_exit(NULL);

}

void *buy() {

while(tickets_avail > 0){

//sem_wait(&buy_choice); if we want user input
sem_wait(&available);
sem_wait(&mutex);

printf("You bought a ticket\t\t\t Total tickets available = %d\n",tickets_avail);
tickets_avail--;

sem_post(&mutex);
sem_post(&sold);
}
done = 1;
pthread_exit(NULL);

}

//if we want to read transactions from a file?
void get_tickets(ticket* ticket_list){


}
