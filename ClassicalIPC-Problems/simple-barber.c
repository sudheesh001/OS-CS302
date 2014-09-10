/* 

Sleeping Barber simulation with simultaneous processes, random delays, and semaphores implemented via pipes.

- Barber finishes cutting hair, checks the waiting room and gets the next customer. If not goes to sleep
- each arriving customer wakes up sleeping Barber
- each customer if no chairs in the waiting room, leaves

- customer must acquire Mutex before entering the shop

*/

#include <stdio.h>
#include <time.h>

void randomwait();
void barber_run();
void customer_run();

int seats[2];
int customers[2];
int barber[2];
int freeseats[2];

void V(int pd[]) {
	int a=1;
	write(pd[1],&a,sizeof(int));
}

void P(int pd[]) {
	int a;
	read(pd[0],&a,sizeof(int));
}

int main() {
	pipe(seats); // pipes uses to implement semaphores
	pipe(customers);
	pipe(barber);
	
	pipe(freeseats); // implemented to pass the integer value for number of seats
	
	V(seats); // Sets the initial value of seats to 1
	
	int num=3; // Freeseats = 3
	write(freeseats[1],&num,sizeof(int));
	
	if (fork() == 0) {
		srand(time(0)+1); // Randomized seed based on time
		barber_run();
		return;
	}
	int i;
	for (i=1;i<=5;i++) { // Start five customers simultaneously
		if (fork() == 0) {
			srand(time(0)+2*i); // different seed tban barber for different delays
			customer_run();
			return;
		}
	}
	sleep(10);
	printf("\ndone\n\n");
	return 0;
}

void barber_run() {
	int num,i;
	for (i=1;i<=10;++i) {
		printf("Barber %d is trying to get a customer\n",i);
		P(customers); // reserve customer
		printf("Barber %d is waiting for the seat to become free\n",i);
		P(seats); // reserve seat
		
		 // use pipes to communicate free seats
		read(freeseats[0],&num,sizeof(int));
		num++; //free seats++
		write(freeseats[1],&num,sizeof(int));
		
		printf("Barber %d is increasing the number of free seats to %d\n",i,num);
		V(barber); // Barber ready to cut
		V(seats); // release lock on chairs
		printf("Barber is now cutting hair %d\n",i);
		randomwait();	
	}
}

void customer_run() {
	int num,i;
	for (i=1;i<=2;++i) {
		printf("New customer trying to find a seat\n");
		P(seats); // acquire a lock on chairs
		
		// use pipes to communicate free seats
		read(freeseats[0],&num,sizeof(int));
		if (num > 0) 
		{
			num--;
			write(freeseats[1],&num,sizeof(int));
			printf("Customer is decreasing the number of free seats to %d\n",num);
			V(customers); // notify Barber
			V(seats); // release lock on chairs
			printf("Customer is now waiting for the barber\n");
			P(barber); // wait until Barber is free
			printf("Customer is now getting a hair cut\n");
		}
		else
		{
			write(freeseats[1],&num,sizeof(int));
			V(seats); // release lock on chairs
			printf("Customer giving up: No free chairs in waiting room\n");
		}
		randomwait();
	}
}

void randomwait() { // random delay in nanoseconds = 0.0-0.9 sec
	int delay;
	struct timespec tim, tim2;
    tim.tv_sec = 0;
	delay = rand() % 900000000;
	tim.tv_nsec = delay;
	printf("  - wait: %d\n",delay); // debugging - value of wait time
	nanosleep(&tim,&tim2);
}