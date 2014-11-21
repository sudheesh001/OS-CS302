/**Basic implementation of Apriori algorithm**/

/**
    Author: Sudheesh Singanamalla
    B.Tech CSE(2012-16), National Institute of Technology, Warangal
**/

#include<iostream>
#include<vector>
#include<fstream>

using namespace std;

struct node
{
    int data;
    node* next;
};

vector<node*> c[1000], l[1000];
int size_c[100000],size_l[100000];
int trans[1000][1000];
int t;
int items;
int min_sup = 10;// minimum support in percent.
int min_count;

void add_ll(node*& h,int k)
{
    if(h == NULL)
    {
        h = new node();
        h->data = k;
        h->next = NULL;
    }
    else
    {
        node *p,*l = h;
        while(l->next != NULL)
        {
            l = l->next;
        }
        p = new node();
        p->data = k;
        p->next = NULL;
        l->next = p;
    }
}

void print_ll(node* h)
{
    //cout<<endl;
    while(h != NULL)
    {
        cout<<h->data<<" ";
        h = h->next;
    }
    cout<<",";
}

bool check_prefix(int i,int j,int k)
{
    node *p = l[i][j];
    node *q = l[i][k];

    int flag = 0,w=1;
    while(w<i)
    {
        if(p->data != q->data)
        {
            flag = 1;
            break;
        }
        p = p->next;
        q = q->next;
        w++;
    }
    if(flag == 0)
    {
        if(p->data < q->data)
            return true;
    }
    return false;
}

void join_itemset(int i)//joins L(i) with L(i) to generate C(i+1)
{
    c[i+1].resize(100000);
    int h=1;
    for(int j=1; j<=size_l[i]; j++)
    {
        for(int k=j+1; k<=size_l[i]; k++)
        {
            //cout<<"check_pre "<<i<<" "<<j<<" "<<k<<endl;
            if(check_prefix(i,j,k))
            {

                node* q =l[i][k];
                while(q->next != NULL)
                    q = q->next;

                node *o = l[i][j];
                while(o != NULL)
                {
                    add_ll(c[i+1][h],o->data);
                    o = o->next;
                }
                add_ll(c[i+1][h++], q->data);
            }
        }
    }
    size_c[i+1] = h-1;
    cout<<"\ncandidate "<<i+1<<" - itemsets are \n";
    for(int k=1; k<=size_c[i+1]; k++)
    {
        print_ll(c[i+1][k]);
    }
}

bool check_freq(int i,int j)//takes an itemset from C(k) and return true if it is to be included in L(k).
{
    node *l;
    l = c[i][j];
    int freq=0,flag;

    for(int k=1; k<=t; k++)
    {
        flag = 0;
        l = c[i][j];
        while(l != NULL)
        {
            if(trans[k][l->data] == 0)
            {
                flag = 1;
                break;
            }
            l = l->next;
        }
        if(flag == 0)
        {
            freq++;
            if(freq >= min_count)
                return true;
        }
    }
    return false;
}

int main()
{
    cout<<"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

    ifstream fin("trans5.txt");
    ofstream fout;
    fin>>items>>t;
    int i=1;

    while(i<=t)
    {
        for(int j=1; j<=items; j++)
        {
            fin>>trans[i][j];
        }
        i++;
    }
    min_count = (min_sup * t)/100;
    //min_count = 2;
    c[1].resize(100000);
    int k=1;
    for(int i=1; i<=items; i++)
    {

        c[1][i] = NULL;
        add_ll(c[1][i],i);
        size_c[1] += 1;
    }
    l[1].resize(100000);
    cout<<"\nGenerating frequent 1 "<<"-  itemsets...\n";
    for(int i=1; i<=size_c[1]; i++)
    {
        if(check_freq(1,i))
        {
            l[1][k++] = c[1][i];
        }
    }
    size_l[1] = k-1;
    for(int i=1; i<=size_l[1]; i++)
    {
        print_ll(l[1][i]);
    }

    for(int k=2; k<=items; k++)
    {
        join_itemset(k-1);
        int h=1;
        l[k].resize(100000);
        for(int i=1; i<=size_c[k]; i++)
        {
            if(check_freq(k,i))
            {
                l[k][h++] = c[k][i];//cout<<"hello";
            }
        }
        size_l[k] = h-1;
        cout<<endl;cout<<"\nGenerating frequent "<<k<<" - itemsets...\n";
        for(int i=1; i<=size_l[k]; i++)
        {
            print_ll(l[k][i]);
        }
        cout<<endl;
    }
    cout<<"-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    cout<<"\nApriori Algorithm finished execution\n";
    return 0;
}

