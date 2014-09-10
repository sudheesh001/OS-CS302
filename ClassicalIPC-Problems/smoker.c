#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void smoker(int * id);
void agent();
void putdown(int type);
void pickup(int smoker_type);
int mayIgo(int smoker_type);

pthread_mutex_t table;

pthread_t sm[3];
pthread_t ag;
pthread_cond_t self;


int t=0,m=0,p=0,agnt=1;
 
int main () {
  int i,pid[3];
  int * who;

 /* Create some LOCKS  */
  if (pthread_mutex_init(&table, NULL)) {
    perror("pthread_mutex_init");
    exit(1);
  }
 
  pthread_cond_init(&self, NULL); //create the conditional variable 
  
  /* Create an agent threads */
  if (pthread_create(&(ag), NULL, (void *)agent, NULL ) != 0) {
    perror("pthread_create");
    exit(1);
  }
  
  /* Create some smokers threads */
  for (i=0; i < 3; i++) {
    if((who = (int *)malloc(sizeof(int))) == NULL) {
      /* I am using malloc as a way to make sure that each
       * thread uses different memory
       */
      perror("malloc");
      exit(1);}
    *who = i;
    if (pthread_create(&(sm[i]), NULL, (void *)smoker, who) != 0) {
      perror("pthread_create");
      exit(1);
    }
  }
  //pthread_mutex_unlock(&table);
  pthread_join(ag, NULL); /* Wait for agent thread to die */
  printf("Thread agent has terminated\n");
  printf("main is going now, time to kill off smokers\n");
  for (i=0; i < 3; i++) {
    pthread_kill(sm[i], 0);
    printf("smoker %d has terminated\n", i);
  }
  exit(0);
}


void smoker(int * id){//i am using one big fucntion for all three smokers
  //you could have used seperate functions

  printf("smoker %d here\n",*id);
  while (1){
    /*
    pthread_mutex_lock(&table); 
    switch(*id){
    case 0:
      while(t != 1){
	pthread_cond_wait(&self, &table);
      }
      printf("smoker tobacco  smoking\n");
      t=0;
      agnt=1;
      break;
    case 1:
      while(m != 1){
	pthread_cond_wait(&self, &table);
      }
      
      m=0;
      agnt=1;
      printf("smoker matches  smoking\n");
      break;
    case 2:
      while(p != 1){
	pthread_cond_wait(&self, &table);
      }
      p=0;
      agnt=1;
      printf("smoker paper  smoking\n");
      break;
    }
    pthread_cond_broadcast(&self);
    pthread_mutex_unlock(&table);
    */
    pickup(*id);
    sleep(1);//smoke
  }
  exit(0);
}

void agent(){
  int rn,x=0;
  printf("agent here\n");
  
  while(x<10){
    rn=rand()%3;
    putdown(rn);
    sleep(1); //explain why this is needed!!!
    x++;
  }
  printf("agent done\n");
  return;
}
void putdown(int type){
  pthread_mutex_lock(&table);
  while(agnt != 1){
    pthread_cond_wait(&self, &table);
  }
  switch (type){
  case 0: //tobacco
    t=1;
    printf("agent placed matches and paper on table\n");
    break;
  case 1://matches
    m=1;
    printf("agent placed tobacco and paper on table\n");
    break;
  case 2://paper
    p=1;
    printf("agent placed matches and tobacco on table\n");
    break;      
    }
  pthread_cond_broadcast(&self);
  pthread_mutex_unlock(&table);  
}

void pickup(int smoker_type){//one function for all smokers based on smoker id
  pthread_mutex_lock(&table); 
  switch(smoker_type){
  case 0:
    while(t != 1){
      pthread_cond_wait(&self, &table);
    }
    printf("smoker tobacco  smoking\n");
    t=0;
    agnt=1;
    break;
  case 1:
    while(m != 1){
      pthread_cond_wait(&self, &table);
    }
    
    m=0;
    agnt=1;
    printf("smoker matches  smoking\n");
    break;
    case 2:
      while(p != 1){
	pthread_cond_wait(&self, &table);
      }
      p=0;
      agnt=1;
      printf("smoker paper  smoking\n");
      break;
  }
  pthread_cond_broadcast(&self);
  pthread_mutex_unlock(&table);
}