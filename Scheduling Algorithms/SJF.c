#include <stdio.h>
int main()
{
   int n, *p,*w,*re,i;
   int sum=0;

   printf("Enter the number of processes:");
   scanf("%d", &n);
   p=(int*)malloc(n*sizeof(int));
   w=(int*)malloc(n*sizeof(int));
   re=(int*)malloc(n*sizeof(int));
   for(i=0; i<n; i++)
   {
      printf("Burst time for process %d:", i+1);
      scanf("%d", &p[i]);
      re[i]=i;
  }
  int j;
  for(i=0; i<n; i++)
  {
      for(j=0; j<n-1; j++)
      {
         if(p[j] > p[j+1])
         { 
            int tmp=p[j];
            p[j]=p[j+1];
            p[j+1]=tmp;
            tmp=re[j];
            re[j]=re[j+1];
            re[j+1]=tmp;
        }
    }
}
sum=0;
for(i=0; i<n; i++)
{
  w[i]=sum;
  sum+=p[i];
}
printf("\t\tProcess\t\tWaiting Time\n");
sum=0;
int cnt=0;
while(cnt!=n)
{
  for(i=0; i<n; i++)
  {
     if(cnt==re[i])
     {
        printf("\t\t%d\t\t%d\n", cnt+1, w[i]);
        sum+=w[i];
        cnt++;
    }
}
}

printf("Average waiting time:%d sec\n", sum/n);
return 0;
}