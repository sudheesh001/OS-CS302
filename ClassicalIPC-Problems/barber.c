#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>

/* globals */

#define totalcustomers  10
#define totalbarbers  2
#define maxcapacity  6
#define totalchairs  6
#define totalfilmfares  0


sem_t max_capacity;
int freechairs = totalchairs;
sem_t mutex2;
int freefilmfares=totalfilmfares;
sem_t mutexfilmfare;
sem_t filmfare;
sem_t barberchair;
int chair[totalchairs];
sem_t cust_ready;

sem_t finished[totalcustomers];
sem_t done[totalcustomers];
sem_t finishedcounter;
sem_t leave_bchair[totalchairs];

//Queue Q1,Q2; /* Q1 pipe between customer <->  barber
//                Q2 pipe between customer <-> cashcounter */
sem_t mutex3;
sem_t mutex4;

//semaphore readywithpayment = 0; /* cashcounter waits for customer */
sem_t readywithpayment;
//semaphore receipt[50] /* cutomer waits for cashcounter,
//                         individualized for each cutomer */
sem_t receipt[totalcustomers];

int pipe1[2],pipe2[2];

void write_pipe(writefd,value)
  int writefd;
  int  value;
{
   if(write(writefd,&value,sizeof(int)) != sizeof(int)) {
        printf("pipe write error");
        exit(0);
   }
}

void read_pipe(readfd,value,status)
    int readfd;
    int *value;
    int *status;
{
    int n;
     if( (n =  read(readfd,value,sizeof(int))) == -1) {
        printf ("pipe read error");
        exit(0);
     }
     *status = (char)n;
}

void Customer(CustomerId) {
 int i,k;

    /* stand outside patiently if no room in shop */
    sem_wait(&max_capacity);
        printf("Customer %d enters shop\n",CustomerId);
        /* do we go straight to a barber's chair? or read filmfare?
           or are we forced to study OS? check securely */
	sem_wait(&mutex2);
        sem_wait(&mutexfilmfare);
        if ((freechairs == 0) || (freefilmfares < totalfilmfares)) {
           sem_post(&mutexfilmfare);
	   sem_post(&mutex2);
               /* enact read OS by sem-wait on filmfare (metaphorical,
                  not procedural),
                  an animated procedure will require sem_post handler
                  interrupts. one of the reasons why OS's have
                  software interrupts (traps) in addition to
                  semaphores  */
	       printf("Customer %d deciding what to read\n",CustomerId);
               sem_wait(&mutexfilmfare);
               if (freefilmfares <= 0) {
	         printf("Customer %d reads S+G OS\n",CustomerId);
		}
	       freefilmfares--;
               sem_post(&mutexfilmfare);
               sem_wait(&filmfare); 
            
            /* enact read filmfare by sem-wait on barberchair.
               same reason as above.*/
               printf("Customer %d reads Filmfare\n",CustomerId);
               sem_wait(&barberchair); /* read filmfare*/
               /*release filmfare*/
               sem_wait(&mutexfilmfare);
               freefilmfares++;
               sem_post(&mutexfilmfare);
               sem_post(&filmfare);
               printf("Customer %d released Filmfare\n",CustomerId);
        }  
        else { sem_post(&mutexfilmfare);sem_post(&mutex2);sem_wait(&barberchair); }

        /* barberchairs, here we come to select */
         /*choose a free chair. show preference in order - simplest.
           perhaps the first chair has a lurid interesting poster
           ahead on the wall and we wish to be as close as possible? 
	   chair[i] = -1 means it is unoccupied */
	 sem_wait(&mutex2);
         i = 0;
         while ((chair[i] != -1) && (i < totalchairs)) i++;
         if (i == totalchairs) {
            printf("This is not possible! we are here because at least 1 chair empty!");
            printf("which idiot sem_posted barberchair? psychoanalysis suggested!");
            exit(0);
         }
         else {
             /* occupy chair i */
             /* the secure selection ensures we aren't sitting on somebody's lap*/
              chair[i] = CustomerId;
              freechairs--; 
	      printf("Customer %d occupies chair %d. freechairs = %d\n",CustomerId,i,freechairs);
         }
         sem_post(&mutex2);

         /* now that we're properly seated, we're ready for a hair cut*/
         /* let's put our name and our chair's name in a pipe to barber */
         /*   and sem_post that we are ready */
         sem_wait(&mutex3);
//              enqueue(Q1,(CustomerId,i)); 
	      write_pipe(pipe1[1],CustomerId);
	      write_pipe(pipe1[1],i);
         sem_post(&mutex3);
         sem_post(&cust_ready);

         /*wait for sem_post from barber that hair cut done */
         sem_wait(&finished[CustomerId]);

//         leavebarberchair; /*animated procedure*/
         /* sem_post the right chair, ow the next guy may sit on our lap */
         sem_wait(&mutex2);
           freechairs++;
           chair[i] = -1;
           sem_post(&leave_bchair[i]);
         sem_post(&mutex2);
	 printf("Customer %d left chair %d. freechairs = %d\n",CustomerId,i,freechairs); 

         /*want to pay. let's put our id on to a pipe to the cashcounter*/
         sem_wait(&mutex4);
//            enqueue(Q2,CustomerId);
	    write_pipe(pipe2[1],CustomerId);
         sem_post(&mutex4);
//         pay; /* animated procedure, cash comes out of pocket,
//                 have to hand over to somebody */
	 printf("Customer %d ready to pay\n",CustomerId);

         sem_post(&readywithpayment);
         sem_wait(&receipt[CustomerId]);

         //leaveshop; /* animated procedure */
	 printf("Customer %d leaving shop\n",CustomerId);
         sem_post(&done[CustomerId]);
         /* sem_post one less in shop, have a good day */
         sem_post(&max_capacity);
	 if (CustomerId == (totalcustomers-1)) {
		 puts("served the last. wait for others to finish");
		 for(k=0;k <= (totalcustomers-1); k++) sem_wait(&done[k]);
		 puts("All done. GAME OVER. BYE BYE");
		 exit(0);
         }
	 else pthread_exit(0);
}

/* the right way to program the barber process would be
   to register two sem_post handlers to cut hair and 
   accept payment and sleep in a loop (or pause, check
   out pause in /usr/include/unistd.h .
   cust_ready should be a sem_post/interrupt from the cutomer on
   which the barber should do the necessary for cutting
   hair. similarly there should be a sem_post/interrupt from
   the cash counter to accept payment.
   it will be instructive to try out the modifications */

void Barber(number) {
int myCustomer, myChair, status;
 while(1) {

	printf("Barber %d: waiting for a call\n",number);
     /* we sleep till there is there a customer seated on a chair
        or there is a call from the cash counter */
     sem_wait(&cust_ready);

     /* obtain customerid and chair no securely */
     sem_wait(&mutex3);
//         dequeue(Q1,(myCustomer,myChair));
	 read_pipe(pipe1[0],&myCustomer,&status);
	 read_pipe(pipe1[0],&myChair,&status);
     sem_post(&mutex3);
     
     if (myCustomer != -1) {
       //a regular customer
         //cuthair; may be a random delay here?
	 printf("Barber %d cuts hair of Customer %d on chair %d\n",number,myCustomer,myChair);

         /* tell the chapie he can go, have a good day */ 
         sem_post(&finished[myCustomer]);
         /* wait till the chapie gets up. individualized,
            so that we get sem_post from the right guy */
         sem_wait(&leave_bchair[myChair]);

	 printf("Barber %d has seen  Customer %d leave chair %d\n",number,myCustomer,myChair);
         /* now sem_post that a chair is free */
         sem_post(&barberchair);
      }
      else { 
         /* we have a call from the cashcounter */

           //acceptpayment; /*animated procedure */
	   printf("Barber %d accepts payment\n",number);
           sem_post(&finishedcounter);
      }
 } 
}

void cashcounter() {
int Customer;
int MyId = -1; /* i'm a special client of the barbers */
int Mychairno = 0; /*we treat the cash counter is a special dummy chair */
int status;
 while(1) {
  /* wait for a customer to arrive here with money in hand */
  sem_wait(&readywithpayment);

  /* good to see money. let's see who the guy is */
  sem_wait(&mutex4);
//  dequeue(Q2,Customer);
  read_pipe(pipe2[0],&Customer,&status);
  sem_post(&mutex4);

	 
  printf("Cashcounter: Customer %d has arrived with payment. Calling a barber\n",Customer);
  /* call a barber. again metaphorical, not procedural */
  /* first put our id on the demand Q to the barber,
     and then sem_post we are ready. we are a special customer */
   sem_wait(&mutex3);
//       enqueue(Q1,(MyId,Mychairno)); 
       write_pipe(pipe1[1],MyId);
       write_pipe(pipe1[1],Mychairno);
   sem_post(&mutex3);
   sem_post(&cust_ready);
   
   /* wait for a sem_post from a barber that payment has been received */
   printf("Cashcounter: Waiting to hear that Customer %d has paid to a barber\n",Customer);
   sem_wait(&finishedcounter);
   printf("Cashcounter: Customer %d has paid to a barber\n",Customer);

  sem_post(&receipt[Customer]); /* let go the right customer */
 } 
}

main() { 
 int i;
 pthread_t threads[100];
 sem_init(&max_capacity,0,maxcapacity);
 sem_init(&mutex2,0,1);
 sem_init(&mutex3,0,1);
 sem_init(&mutex4,0,1);
 sem_init(&mutexfilmfare,0,1);
 sem_init(&filmfare,0,totalfilmfares);
 sem_init(&barberchair,0,totalchairs);
 sem_init(&cust_ready,0,0);
 sem_init(&finishedcounter,0,0);
 sem_init(&readywithpayment,0,0);
 for(i=0;i<totalcustomers;i++)
    { sem_init(&finished[i],0,0); sem_init(&receipt[i],0,0);}
 for(i=0;i<totalchairs;i++)
    { 	sem_init(&leave_bchair[i],0,0); chair[i]=-1;}

 if (pipe(pipe1) < 0) printf("can't create pipe1\n"); 
 if (pipe(pipe2) < 0) printf("can't create pipe2\n"); 

// pthread_create(&threads[i],NULL,(void *)&cashcounter,(void *)i);
 printf("creating barbers\n");
 i=1;
 while (i <= totalbarbers) {
	 pthread_create(&threads[i],NULL,(void *)&Barber,(void *)i);
		 i++;
 }
 printf("creating customers\n");
 i=0;
 while (i < totalcustomers) {
	 pthread_create(&threads[i+totalbarbers],NULL,(void *)&Customer,(void *)i);
		 i++;
 }
 cashcounter();
}