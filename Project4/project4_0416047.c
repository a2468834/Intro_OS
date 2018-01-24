/* Project4: The Sleeping Gym Leader */
/* 0416047 Andy Wang */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Define some printf color
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Function prototypes
void *ash();
void *misty();
void *brock();
void *leader();

// Define the semaphores.

// waitingRoom limits the # of customers allowed in the waiting room at one time.
sem_t waitingRoom;

// battleField ensures mutually exclusive battling with the gym leader.
sem_t battleField;

// leaderBed is used to allow the gym leader to sleep until a trainer arrives.
sem_t leaderBed;

// seatBelt is used to make the trainer wait until the gym leader finished the battle.
sem_t seatBelt;

// battleTime is used to let the gym leader know how long the battle is.
int battleTime = 0;

// When all trainers get their badges, allDone will be set to 1 
int allDone = 0;

int main()
{
    pthread_t gymLeader;
    pthread_t trainerA;
	pthread_t trainerM;
	pthread_t trainerB;

    printf("\nProject4_0416047.c\n\n");
	
    // Initialize the semaphores with initial values.
    sem_init(&waitingRoom, 0, 2);
    sem_init(&battleField, 0, 1);
    sem_init(&leaderBed, 0, 0);
    sem_init(&seatBelt, 0, 0);

    // Create the barber.
    pthread_create(&gymLeader, NULL, leader, NULL);

    // Create the customers.
	pthread_create(&trainerA, NULL, ash, NULL);
	pthread_create(&trainerM, NULL, misty, NULL);
	pthread_create(&trainerB, NULL, brock, NULL);

    // Join each of the threads to wait for them to finish.
    pthread_join(trainerA, NULL);
	pthread_join(trainerM, NULL);
	pthread_join(trainerB, NULL);
	
    // When all trainers get their badges, kill the thread gymLeader.
    allDone = 1;
	// Wake the gym leader up, so he will exit.
	sem_post(&leaderBed);
    pthread_join(gymLeader, NULL);
	
	return 0;
}

void *leader()
{
    while (!allDone)
	{
		// Sleep until someone arrives and wakes you.
		sem_wait(&leaderBed);
		
		// Skip this 'if' statement when kill the thread gymLeader.
		if (!allDone)
		{
			sleep(battleTime);
			
			// Release the trainer when battle is done.
			sem_post(&seatBelt);
		}
		
		else printf(ANSI_COLOR_GREEN "The gym leader is going home today.""\n" ANSI_COLOR_RESET);
	}
}

void *ash()
{
	for(int i=0; i<3; i++)
	{
		printf("Trainer Ash is training.\n");
		sleep(2);
		printf("Trainer Ash arrives at gym.\n");
		
		// Wait for a position in the waiting room.
		while(sem_trywait(&waitingRoom) != 0)
		{
			printf("Trainer Ash goes back to train.\n");
			sleep(2);
		}
		printf("Trainer Ash enters waiting room.\n");

		// Wait for the battleField to become free.
		sem_wait(&battleField);
		
		// Wake up the gym leader.
		sem_post(&leaderBed);
		printf("The gym leader is waken up by Ash.\n");

		// Wait for the gym leader to finish battling.
		printf("Trainer Ash is battling with the gym leader now.\n");
		battleTime = 2;
		sem_wait(&seatBelt);
		
		
		if(i==2)printf(ANSI_COLOR_BLUE "\n""Trainer Ash gets the badge!""\n\n" ANSI_COLOR_RESET);
		else printf("Trainer Ash fails temporarily.\n");
		
		// Free the battleField waitingRoom to other waiting trainers.
		sem_post(&battleField);
		sem_post(&waitingRoom);
	}
}

void *misty()
{    
	for(int i=0; i<2; i++)
	{
		printf("Trainer Misty is training.\n");
		sleep(3);
		printf("Trainer Misty arrives at gym.\n");
		
		// Wait for a position in the waiting room.
		while(sem_trywait(&waitingRoom) != 0)
		{
			printf("Trainer Misty goes back to train.\n");
			sleep(3);
		}
		printf("Trainer Misty enters waiting room.\n");
		
		// Wait for the battleField to become free.
		sem_wait(&battleField);
		
		// Wake up the gym leader.
		sem_post(&leaderBed);
		printf("The gym leader is waken up by Misty.\n");
		
		// Wait for the gym leader to finish battling.
		printf("Trainer Misty is battling with the gym leader now.\n");
		battleTime = 3;
		sem_wait(&seatBelt);
		
		if(i==1)printf(ANSI_COLOR_BLUE "\n""Trainer Misty gets the badge!""\n\n" ANSI_COLOR_RESET);
		else printf("Trainer Misty fails temporarily.\n");

		// Free the battleField waitingRoom to other waiting trainers.
		sem_post(&battleField);
		sem_post(&waitingRoom);
	}
}

void *brock()
{
	printf("Trainer Brock is training.\n");
	sleep(5);
	printf("Trainer Brock arrives at gym.\n");
	
	// Wait for a position in the waiting room.
	while(sem_trywait(&waitingRoom) != 0)
	{
		printf("Trainer Brock goes back to train.\n");
		sleep(5);
	}
	printf("Trainer Brock enters waiting room.\n");
	
	// Wait for the battleField to become free.
	sem_wait(&battleField);
	
	// Wake up the gym leader.
	sem_post(&leaderBed);
	printf("The gym leader is waken up by Brock.\n");
	
	// Wait for the gym leader to finish battling.
	printf("Trainer Brock is battling with the gym leader now.\n");
	battleTime = 5;
	sem_wait(&seatBelt);
	printf(ANSI_COLOR_BLUE "\n""Trainer Brock gets the badge!""\n\n" ANSI_COLOR_RESET);
	
	// Free the battleField and waitingRoom to other waiting trainers.
	sem_post(&battleField);
	sem_post(&waitingRoom);
}
