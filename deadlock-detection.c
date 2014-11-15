#include<stdio.h>

void main()
{
int found,flag,l,p[4][5],tp,c[4][5],i,j,k=1,m[5],r[5],a[5],temp[5],sum=0;
clrscr();
printf("enter total no of processes");
scanf("%d",&amp;tp);
printf("enter clain matrix");
for(i=1;;=4;i++)
for(j=1;j&lt;=5;j++)
{
scanf("%d",&amp;c[i][j]);
}
printf("enter allocation matrix");
for(i=1;i&lt;=4;i++)
for(j=1;j&lt;=5;j++)
{
scanf("%d",&amp;p[i][j]);
}
printf("enter resource vector:\n");
for(i=1;i&lt;=5;i++)
{
scanf("%d",&amp;r[i]);
}
printf("enter availability vector:\n");
for(i=1;i&lt;=5;i++)
{
scanf("%d",&amp;a[i]);
temp[i]=a[i];
}
for(i=1;i&lt;=4;i++)
{
sum=0;
for(j=1;j&lt;=5;j++)
{
sum+=p[i][j];
}
if(sum==0)
{
m[k]=i;
k++;
}
}
for(i=1;i&lt;=4;i++)
{
for(l=1;l&lt;k;l++)
if(i!=m[l])
{
flag=1;
for(j=1;j&lt;=5;j++)
if(c[i][j]&gt;temp[j])
{
flag=0;
break;
}
}
if(flag==1)
{
m[k]=i;
k++;
for(j=1;j&lt;=5;j++)
temp[j]+=p[i][j];
}
}
printf("deadlock causing processes are:");
for(j=1;j&lt;=tp;j++)
{
found=0;
for(i=1;i&lt;k;i++)
{
if(j==m[i])
found=1;
}
if(found==0)
printf("%d\t",j);
}
getch();
}