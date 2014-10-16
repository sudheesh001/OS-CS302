#include<stdio.h>
/* NPROC = number of processes, NRES = number of resources */
#define NPROC 5
#define NRES  3
#define BOOL int

// add vector B to vector A
void vadd(int *A, int *B, int sgn)  //flag 1 = add,  -1 = subtract
{
    int i;
    for(i=0;i<NRES;i++) A[i] = A[i] + (sgn * B[i]);
}

// determine if A<B
BOOL vcmp(int *A, int *B)
{ int i, j;
  j = 1;
  for(i=0;j && (i<NRES);i++) if (A[i]>B[i]) j = 0;
  return j;
}

// print a NPROCxNRES array
void print2d(int ** A)
{ int i, j;
  for(i=0;i<NPROC;i++) 
      { printf("P%d  ",i);
    for(j=0;j<NRES;j++) printf("%d ",A[i][j]);
        printf("\n");
      }
  printf("\n");
} // print2d

// determine if a state is safe
BOOL safe(int * Finish,
      int * Available,
      int * Work,
      int** Allocation,
      int** Need
         )
{ int i, j;
  // find i such that finish[i] = 0 and need[i] < work
  for(j=0;j<NRES;j++) printf("%d ",Work[j]); printf("  (work)\n");
  i = 0;   // proc counter
  j = 1;   // all finished
  while ((i<NPROC) && (Finish[i] || (!vcmp(Need[i],Work))))
      {
      j = j && Finish[i];
      i++;
      }
  if (i==NPROC) return j;
  // simulate acquisition and release of resources
  printf("Process %d can finish\n\n",i);
  Finish[i] = 1;
  vadd(Work,Allocation[i],1);
  return safe(Finish,Available,Work,Allocation,Need);
} // safe

// determine if a request by a process will lead to a safe state
BOOL grant(int pid,
       int * Request,
       int * Available,
       int** Allocation,
       int** Need)
{   int i;
    int Finish[NPROC];
    int Work[NRES];
    for(i=0;i<NPROC;i++) Finish[i] = 0;
    if (!vcmp(Request,Need[pid])) return 0;
    if (!vcmp(Request,Available)) return 0;
    // simulate acquisition
    vadd(Available,Request,-1);
    vadd(Allocation[pid],Request,1);
    vadd(Need[pid],Request,-1);
    for(i=0;i<NRES;i++) Work[i] = Available[i];    
    printf("Allocation:\n"); print2d(Allocation);
    printf("Need:\n"); print2d(Need);
    return safe(Finish,Available,Work,Allocation,Need);
}

// main function implements example in dinosaur book
int main()
{ int i, j;
  int Av[NRES];      // available
  int Req[NRES];     // request
  int Work[NRES];    
  int Finish[NPROC]; 
  int ** Al;  int ** Nd;  // allocation and need are dynamic 2d arrays
  Al = (int **) malloc(sizeof(int*)*NPROC);
  for(i=0;i<NPROC;i++) Al[i] = (int*) malloc(sizeof(int)*NRES);
  Nd = (int **) malloc(sizeof(int*)*NPROC);
  for(i=0;i<NPROC;i++) Nd[i] = (int*) malloc(sizeof(int)*NRES);

  //implicit: initially Av[0] = 10;  Av[1] = 5;  Av[2] = 7;
  
  Al[0][0] = 0; Al[0][1] = 1; Al[0][2] = 0;
  Al[1][0] = 2; Al[1][1] = 0; Al[1][2] = 0;
  Al[2][0] = 3; Al[2][1] = 0; Al[2][2] = 2;
  Al[3][0] = 2; Al[3][1] = 1; Al[3][2] = 1;
  Al[4][0] = 0; Al[4][1] = 0; Al[4][2] = 2;

  Nd[0][0] = 7; Nd[0][1] = 4; Nd[0][2] = 3;
  Nd[1][0] = 1; Nd[1][1] = 2; Nd[1][2] = 2;
  Nd[2][0] = 6; Nd[2][1] = 0; Nd[2][2] = 0;
  Nd[3][0] = 0; Nd[3][1] = 1; Nd[3][2] = 1;
  Nd[4][0] = 4; Nd[4][1] = 3; Nd[4][2] = 1;

  Av[0] = 3; Av[1] = 3; Av[2] = 2;

  for(i=0;i<NRES;i++) Work[i] = Av[i];    
  for(i=0;i<NPROC;i++) Finish[i] = 0;
  if (safe(Finish,Av,Work,Al,Nd)) printf("state is safe\n");
    else printf("state is not safe\n");

  // simulate request for 1,0,2 by P1:
  Req[0] = 1; Req[1] = 0; Req[2] = 2;
  if (grant(1,Req,Av,Al,Nd)) printf("P1 can ");
      else printf("P1 cannot ");
  printf("be granted request for 1,0,2\n");

  // simulate request for 0,2,0 by P0:
  Req[0] = 0; Req[1] = 2; Req[2] = 0;
  if (grant(0,Req,Av,Al,Nd)) printf("P0 can ");
      else printf("P0 cannot ");
  printf("be granted request for 0,2,0\n");

  for(i=0;i<NPROC;i++) { free(Al[i]); free(Nd[i]); }
  free(Al);  free(Nd);
  exit(0);
}
