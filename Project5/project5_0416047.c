/* Project5: The Banker's Algorithm
   0416047 Andy Wang */

#include <time.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

// Function prototypes
int requestResources(int customerNum, int request[]);
int releaseResources(int customerNum, int release[]);
bool checkSafe();
void printState();
void* customer(void * input);

// The available amount of each resources.
int available[NUMBER_OF_RESOURCES];

// The maximum demand of each customer.
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// The amount currently allocated to each customer.
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// The remaining need of each customer.
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* The finished count of system 'finishCount' increasing
   by one  while a costomer acquire all needs. */
int finishState[NUMBER_OF_CUSTOMERS];
int finishCount = 0;

pthread_mutex_t mutexAvailable;

int main(int argc, char const *argv[])
{
    // start the program
    printf("\nProject5_0416047.c\n\n\n\n");

	int resourcesNum[NUMBER_OF_RESOURCES];
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	pthread_t thread5;

	pthread_mutex_init(&mutexAvailable, NULL);

	int temp = 0;
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
	{
		temp = atoi(argv[i+1]);
		resourcesNum[i] = temp;
	}
	
	/* Initialize "finishState" with all false 
	   If finishState[i] turns into true, it means thread i is done */
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
		finishState[i] = false;

	// Initialize "available" with input values
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		available[i] = resourcesNum[i];

	// Initialize "maximum" with random number according to input values
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
	{
		// Because the program runs fast, time(NULL) will get the same value in each for loop
		srand(time(NULL)+i);
		for (int j=0; j<NUMBER_OF_RESOURCES; j++)
			maximum[i][j] = ( rand() % (resourcesNum[j]+1) );
	}

	// Initialize "allocation" with zero
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
		for (int j=0; j<NUMBER_OF_RESOURCES; j++)
			allocation[i][j] = 0;

	// Initialize "need" by "maximum" minus "allocation"
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
		for (int j=0; j<NUMBER_OF_RESOURCES; j++)
			need[i][j] = maximum[i][j] - allocation[i][j];

	//printState(); 
	
	// Create the customers
	pthread_create(&thread1, NULL, customer, (void *)0);
	pthread_create(&thread2, NULL, customer, (void *)1);
	pthread_create(&thread3, NULL, customer, (void *)2);
	pthread_create(&thread4, NULL, customer, (void *)3);
	pthread_create(&thread5, NULL, customer, (void *)4);
	/*
	do
	{
		for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
			if(finishState[i] == true)finishCount++;

	}while(finishCount != 5);
	*/
	// Join each of the threads to wait for them to finish.
    pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	pthread_join(thread4, NULL);
	pthread_join(thread5, NULL);

	//printf("\nAll threads finished without deadlock.\n");

	return 0;
}

/*
   The meaning of return code
    3 : succeeds & all customers finish (without implementation)
    2 : succeeds & current customer finishes
    1 : succeeds
    0 : no request
   -1 : fails since request exceeds need
   -2 : fails since request exceeds available
   -3 : fails since the current state is not safe
*/
int requestResources(int customerNum, int request[])
{
	/* Check whether this time is no request */
	bool allZero = true;
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		if(request[i] != 0)allZero = false;
	if(allZero == true)return 0;

	/* Check whether all request <= need */
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		if(request[i] > need[customerNum][i])return (-1);

	/* Check whether all request <= available */
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		if(request[i] > available[i])return (-2);
	
	//pthread_mutex_lock( &mutexAvailable );

	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
	{
		available[i] = available[i] - request[i];
		allocation[customerNum][i] = allocation[customerNum][i] + request[i];
		need[customerNum][i] = need[customerNum][i] - request[i];
	}

	/* If system is unsafe, restore the resources and return -3
	   If system is safe, return 1 */
	if( checkSafe() == false )
	{
		for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		{
			available[i] = available[i] + request[i];
			allocation[customerNum][i] = allocation[customerNum][i] - request[i];
			need[customerNum][i] = need[customerNum][i] + request[i];
		}
		//pthread_mutex_unlock( &mutexAvailable );
		return (-3);
	}
	else
	{
		//pthread_mutex_unlock( &mutexAvailable );
		return 1;
	}
}

/*
   The meaning of return code
    1 : succeeds
    0 : no release
   -1 : fails since release exceeds allocation
*/
int releaseResources(int customerNum, int release[])
{
	/* Check whether this time is no request */
	bool allZero = true;
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		if(release[i] != 0)allZero = false;
	if(allZero == true)return 0;

	/* Check whether all release <= allocation */
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		if(release[i] > allocation[customerNum][i])return (-1);

	//pthread_mutex_lock( &mutexAvailable );
	
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
	{
		available[i] = available[i] + release[i];
		allocation[customerNum][i] = allocation[customerNum][i] - release[i];
		need[customerNum][i] = need[customerNum][i] + release[i];
	}
	return 1;
	//pthread_mutex_unlock( &mutexAvailable );
}

void* customer(void * input)
{
	int customerNum = (int *) input;
	int myMaximum[NUMBER_OF_RESOURCES];
	int myRequest[NUMBER_OF_RESOURCES];
	int myRelease[NUMBER_OF_RESOURCES];
	int requestState = 0;
	int releaseState = 0;
	srand(time(NULL));

	for(int i=0; i<NUMBER_OF_RESOURCES; i++)myMaximum[i] = maximum[customerNum][i];
	
	do{
		// Randomly generate the array of request
		for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		{
			//srand(time(NULL)+i);
			myRequest[i] = ( rand() % (myMaximum[i]+1) );
		}

		// beginning of mutex lock (request procedure)
		pthread_mutex_lock( &mutexAvailable );

		requestState = requestResources(customerNum, myRequest);

		if(requestState == 1)
		{
			// Check whether this customer is fully allocated
			bool fullAllocation = true;

			for(int i=0; i<NUMBER_OF_RESOURCES; i++)
				if(allocation[customerNum][i] != maximum[customerNum][i])fullAllocation = false;

			// It means that this customer is finished
			if (fullAllocation == true)
			{
				releaseResources(customerNum, maximum[customerNum]);
				requestState = 2;
				for(int i=0; i<NUMBER_OF_RESOURCES; i++)
					need[customerNum][i] = 0;
				finishState[customerNum] = true;
				finishCount++;
			}

			// It means that not only this customer is finished but also others are finished
			if ( finishCount == 5)requestState = 3;
		}
		
		// Print the request result and the current state
		printf("Request: ");
		for(int i=0; i<NUMBER_OF_RESOURCES; i++)
			printf("%d ", myRequest[i]);

		switch( requestState )
		{
			case -3:
			{
				printf("\nRequest Code -3: customer %d's request fails since the state is unsafe.\n\n", customerNum);
				printState();
				break;
			}

			case -2:
			{
				printf("\nRequest Code -2: customer %d's request fails since request exceeds available.\n\n", customerNum);
				printState();
				break;
			}

			case -1:
			{
				printf("\nRequest Code -1: customer %d's request fails since request exceeds need.\n\n", customerNum);
				printState();
				break;
			}

			case 0:
			{
				printf("\nRequest Code 0: customer %d doesn't request.\n\n", customerNum);
				printState();
				break;
			}
			
			case 1:
			{
				printf("\nRequest Code 1: customer %d's request succeeds.\n\n", customerNum);
				printState();
				break;
			}

			case 2:
			{
				printf("\nRequest Code 2: customer %d's request succeeds & finishes.\n\n", customerNum);
				printState();
				break;
			}

			case 3:
			{
				printf("\nRequest Code 3: customer %d's request succeeds & all customers finish.\n\n", customerNum);
				printState();
				break;				
			}

			default:
				break;
		}

		// End of mutex lock
		pthread_mutex_unlock( &mutexAvailable );
		sched_yield();
		
		// Randomly generate the array of release
		for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		{
			//srand(time(NULL)+i);
			myRelease[i] = ( rand() % (myMaximum[i]+1) );
		}

		// beginning of mutex lock (release procedure)
		pthread_mutex_lock( &mutexAvailable );
		if( (requestState != 2) && (requestState != 3) )
		{
			releaseState = releaseResources(customerNum, myRelease);

			// Print the release result
			printf("Release: ");
			for(int i=0; i<NUMBER_OF_RESOURCES; i++)
				printf("%d ", myRelease[i]);

			switch( releaseState )
			{
				case -1:
				{
					printf("\nRelease Code -1: customer %d's release fails since release exceeds allocation.\n\n", customerNum);
					printState();
					break;
				}

				case 0:
				{
					printf("\nRelease Code 0: customer %d doesn't release any resource.\n\n", customerNum);
					printState();
					break;
				}
				
				case 1:
				{
					printf("\nRelease Code 1: customer %d's release succeeds.\n\n", customerNum);
					printState();
					break;
				}

				default:
					break;
			}

			// Print the current state of all customers
		}
		// End of mutex lock
		pthread_mutex_unlock( &mutexAvailable );

		sched_yield();
	}while( finishState[customerNum] != true);

	// This customer is finished, thus terminate this function which is a thread 
}

bool checkSafe()
{
	// set up Work array
	int Work[NUMBER_OF_RESOURCES];
	bool Finish[NUMBER_OF_CUSTOMERS];

	// copy the Available array into the temporary array
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)Work[i] = available[i];
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)Finish[i] = false;

	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
	{
		if(Finish[i] == false)
		{
			bool condition = true;
			for(int j=0; j<NUMBER_OF_RESOURCES; j++)
			{
				// "need > Work" violates the rule of Banker's algorithm
				if(need[i][j]>Work[j])condition = false;
			}
			
			// Find an i such that both two rules of Banker's algorithm
			if(condition = true)
			{
				for(int j=0; j<NUMBER_OF_RESOURCES; j++)
				{
					Work[j] = Work[j] + allocation[i][j];
				}
				Finish[i] = true;
			}
			else;
		}
	}

	// All Finish[i] are true means that system is safe
	bool systemSafety = true;
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
	{
		if(Finish[i] == false)systemSafety = false;
	}

	return systemSafety;
}

void printState()
{	
	printf("Current state:\n");
	printf("Available resources: ");
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		printf("%2d ", available[i]);
	printf("\n\n");
	printf("               maximum       allocation       need\n");
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
	{
		printf("customer %d:  ", i);
		for(int j=0; j<NUMBER_OF_RESOURCES; j++)
			printf("%2d  ", maximum[i][j]);
		printf("   ");
		for(int j=0; j<NUMBER_OF_RESOURCES; j++)
			printf("%2d  ", allocation[i][j]);
		printf("   ");
		for(int j=0; j<NUMBER_OF_RESOURCES; j++)
			printf("%2d  ", need[i][j]);
		printf("\n");
	}
	printf("+--------------------------------------------------+\n");
}
