/**
    Basic implementation of Apriori algorithm using hash-based pruning...

    Step 1: If at any point, we hace C(k), use it to generate L(k) and H(k+1).
    Step 2: Join L(k) and use H(k+1) to generate C(k+1). Go to step 1.
**/

/**
    Author: Sudheesh Singanamalla
    B.Tech CSE(2012-16), National Institute of Technology, Warangal
**/

#include<iostream>
#include<vector>
#include<fstream>
#include<stack>
#include<map>

using namespace std;

struct node
{
    int data;
    node* next;
};

const int num_buckets = 5;

map<int, int> bucket;

vector<node*> c[1000], l[1000],temp;
int temp_size;
int size_c[100000],size_l[100000];
int trans[1000][1000];
int t;
int items;
int min_sup = 40;// minimum support in percent.
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
    cout<<",\n";
}

int hash_func(node* h)
{
    int num = 0;
    node* l = h;
    while(l != NULL)
    {
        num = num*10 + l->data;
        l = l->next;
    }
    return num % num_buckets;
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

void prune(int k)//A temporary candidate k-itemset is converted to C(k) using the hash-table H(k)
{
    c[k].resize(100000);
    int h = 1;
    for(int i=1; i<=temp_size; i++)
    {
        int b = hash_func(temp[i]);
        if(bucket[b] >= min_count)
        {
            node *l = temp[i];
            while(l != NULL)
            {
                add_ll(c[k][h], l->data);
                l = l->next;
            }
            h++;
        }
    }
    size_c[k] = h-1;
    cout<<"\nThere are "<<size_c[k]<<" items after pruning.";
    cout<<"\nCandidate "<<k<<" - itemsets after pruning are \n";
    for(int i=1; i<=size_c[k]; i++)
    {
        print_ll(c[k][i]);
    }
}

void join_itemset(int i)//joins L(i) with L(i) to generate C(i+1)
{
    temp.clear();
    temp.resize(100000);
    int h=1;
    for(int j=1; j<=size_l[i]; j++)
    {
        for(int k=j+1; k<=size_l[i]; k++)
        {
            if(check_prefix(i,j,k))
            {

                node* q =l[i][k];
                while(q->next != NULL)
                    q = q->next;

                node *o = l[i][j];
                while(o != NULL)
                {
                    add_ll(temp[h], o->data);
                    o = o->next;
                }
                add_ll(temp[h++], q->data);
            }
        }
    }

    temp_size = h-1;
    cout<<"\nThere are "<<temp_size<<" items before pruning.";
    cout<<"\nCandidate "<<i+1<<" - itemsets before pruning are \n";
    for(int k=1; k<=temp_size; k++)
    {
        print_ll(temp[k]);
        //int b_no = hash_func(temp[k]);
        //bucket[b_no] += 1;
    }
    prune(i+1);
}

bool check_freq(int i,int j)//takes an itemset from C(k) and return true if it is to be included in L(k).It also adds some k+1 itemsets to buckets
{
//    cout<<"checking for "<<endl;
//    print_ll(c[i][j]);
//    cout<<endl;
    node *l;
    l = c[i][j];
    int b_no = hash_func(c[i][j]);
    int freq=0,flag,last;

    for(int k=1; k<=t; k++)
    {
        flag = 0;
        l = c[i][j];
        while(l != NULL)
        {
            last = l->data;
            if(trans[k][l->data] == 0)
            {
                flag = 1;
                break;
            }
            l = l->next;
        }
        if(flag == 0)
        {
            //cout<<"found it in trans. "<<k<<endl;
            freq++;
            for(int p=last+1; p<=items; p++)
            {
                if(trans[k][p] == 1)
                {
                    ;
                    int q = b_no*10 + p;
                    //cout<<"extending it with "<<p<<" goes into bucket "<<q%num_buckets<<endl;
                    bucket[q%num_buckets] += 1;
                }
            }
        }
    }
    if(freq >= min_count)
        return true;
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
    //min_count = (min_sup * t)/100;
    min_count = 2;
    c[1].resize(100000);
    int k=1;
    for(int i=1; i<=items; i++)
    {

        c[1][i] = NULL;
        add_ll(c[1][i],i);
        size_c[1] += 1;
    }
    l[1].resize(100000);
    bucket.clear();
    cout<<"\nGenerating frequent 1 "<<"-  itemsets...\n";
    for(int i=1; i<=size_c[1]; i++)
    {
        if(check_freq(1,i))
        {
            l[1][k++] = c[1][i];
        }
    }
    //At this point, we have L(1) and hash-table for 2-itemsets, i.e. H(2).
    size_l[1] = k-1;
    for(int i=1; i<=size_l[1]; i++)
    {
        print_ll(l[1][i]);
    }
    join_itemset(1);//now we have C(2) using L(1) and H(2)

    for(int k=2; k<=items; k++)
    {
        bucket.clear();
        //join_itemset(k-1);
        int h=1;
        l[k].resize(100000);
        for(int i=1; i<=size_c[k]; i++)
        {
            if(check_freq(k,i))
            {
                l[k][h++] = c[k][i];//cout<<"hello";
            }
        }
        //At this point, we have L(k) and hash-table for k+1 itemsets, i.e. H(k+1).
        size_l[k] = h-1;
        cout<<endl;
        cout<<"Generating frequent "<<k<<" - itemsets...\n";
        for(int i=1; i<=size_l[k]; i++)
        {
            print_ll(l[k][i]);
        }
        cout<<endl;
        if(size_l[k] == 0)
        {
            cout<<"There are no frequent "<<k<<" - itemsets..Algorithm aborting\n";
            break;
        }
        join_itemset(k);

        //Now, we have C(k+1) using L(k) and H(k+1).
    }
    cout<<"-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    cout<<"\nApriori Algorithm finished execution\n";
    return 0;
}


