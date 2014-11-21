/**Apriori algorithm with Partitioning extension**/

/**
    Author: Sudheesh Singanamalla
    B.Tech CSE(2012-16), National Institute of Technology, Warangal
**/

#include<iostream>
#include<vector>
#include<fstream>
#include<cmath>
#include<algorithm>

using namespace std;

struct node
{
    int data;
    node* next;
};
struct tid_set
{
    vector<int> tid;
};

vector< tid_set *> occurences[1000],final_scan(1);
vector<node*> c[10000], l[10000], c_part[10000], l_part[10000];
int size_c[100000],size_l[100000], size_p_c[100000], size_p_l[100000];
int trans[1000][1000];
int t;
int items;
int min_sup = 10;// minimum support in percent.
int min_count;
int num_partition =  10;
int partition_size;

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
    while(h != NULL)
    {
        cout<<h->data<<" ";
        h = h->next;
    }
    cout<<",";
}

bool check_prefix(int i,int j,int k)
{
    node *p = l_part[i][j];
    node *q = l_part[i][k];

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

void intersection_tid(int i, int j, int k, int h)
{
    //cout<<"joinite"<<i<<j<<k<<h<<endl;
    vector<int> :: iterator it1,it2,it;
    it1 = (occurences[i][j]->tid).begin();
    occurences[i+1][h] = new tid_set();
    while((it1 != (occurences[i][j]->tid).end()) )
    {
        it = find((occurences[i][k]->tid).begin(),(occurences[i][k]->tid).end(),*it1);
        if(it != (occurences[i][k]->tid).end())
        {
            //cout<<*it1;
            (occurences[i+1][h]->tid).push_back(*it1);
        }
        it1++;
    }
}

vector<int> temp,temp1,temp2;
void phase2_intersection(node* l,int k)
{
    temp.clear();
    vector<int> present_temp(1),pres(1);
    present_temp.resize(t+1);
    //pres.resize(items);
    for(int i=1; i<=t; i++)
    {
        present_temp[i] = 1;
    }
    vector<int> :: iterator it1,it2;
    //int i=1;
    for(int i=1; i<=k; i++)
    {
        temp1.clear();
        temp1 = final_scan[l->data]->tid;

        it1 = temp1.begin();

        pres.clear();
        pres.resize(t+1);
        for(int i=1; i<=t; i++)
        {
            pres[i] = 0;
        }
        it1 = temp1.begin();
        while(it1!=temp1.end())
        {
            pres[*it1] = 1;
            it1++;
        }
        for(int j=1; j<=t; j++)
        {
            present_temp[j] = present_temp[j]&pres[j];
        }
        l = l->next;
    }
    for(int j=1; j<=t; j++)
    {
        if(present_temp[j])
        {
            temp.push_back(j);
        }
    }
}

bool itemset_present_already(node* l,int k, int s)
{
    int flag=0,flag1=1;
    node* h = l,*p;
    for(int i=1; i<=s; i++)
    {
        flag1 = 0;
        h = l;
        p = c[k][i];
        while(h != NULL)
        {
            if(h->data != p->data)
            {
                flag1 = 1;
                break;
            }
            h = h->next;
            p = p->next;
        }
        if(flag1 == 0)
            return true;
    }
    return false;
}

void join_itemset(int i)//joins L(i) with L(i) to generate C(i+1)
{
    c_part[i+1].clear();
    occurences[i+1].clear();
    c_part[i+1].resize(100000);
    occurences[i+1].resize(100000);
    int h=1;
    for(int j=1; j<=size_p_l[i]; j++)
    {
        for(int k=j+1; k<=size_p_l[i]; k++)
        {
            if(check_prefix(i,j,k))
            {
                //L[i][j] is being joined with L[i][j]
                node* q =l_part[i][k];
                while(q->next != NULL)
                    q = q->next;

                node *o = l_part[i][j];
                while(o != NULL)
                {
                    add_ll(c_part[i+1][h],o->data);
                    o = o->next;
                }
                add_ll(c_part[i+1][h], q->data);

                //find the occurences of C[i+1][h] by intersection of occurences of L[i][j] and L[i][j]..
                intersection_tid(i,j,k,h);

                h++;
            }
        }
    }
    size_p_c[i+1] = h-1;
    cout<<"\ncandidate "<<i+1<<" - itemsets are \n";
    for(int k=1; k<=size_p_c[i+1]; k++)
    {
        print_ll(c_part[i+1][k]);
    }
}

int main()
{
    cout<<"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

    ifstream fin("trans5.txt");
    ofstream fout;
    fin>>items>>t;
    partition_size = t/num_partition;
    int i=1;

    while(i<=t)
    {
        for(int j=1; j<=items; j++)
        {
            fin>>trans[i][j];
        }
        i++;
    }
    min_count = (int)((ceil)(((double)(min_sup * t))/100));
    //min_count = 2;
    final_scan.resize(items);
    for(int i=1; i<=items; i++)
    {
        c[i].resize(100000);
        l[i].resize(100000);
        size_c[i] = size_l[i] = 0;
        final_scan[i] = new tid_set();
    }
    int y=1;
    for(int k=1; k<=num_partition; k++) //reads the kth partition...
    {
        y=1;
        cout<<"\nPartition "<<k<<endl;
        c[1].resize(100000);
        c_part[1].clear();
        l_part[1].clear();
        int start = (k-1)*partition_size + 1;
        int stop = k*partition_size;
        if(k == num_partition)
        {
            stop = t;
            partition_size += (t%num_partition);
        }

        int partition_min_count = (int)((ceil)(((double)(min_sup * partition_size))/100));
        c_part[1].resize(10000);
        l_part[1].resize(10000);
        occurences[1].resize(100000);
        for(int j=1; j<=items; j++)
        {
            occurences[1][j] = new tid_set();
            add_ll(c_part[1][j],j);
        }

        for(int i = start; i<=stop; i++)
        {
            for(int j=1; j<=items; j++)
            {
                if(trans[i][j] == 1)
                {
                    occurences[1][j]->tid.push_back(i);
                }
            }
        }//at this point, we have C(1) of this partition...

        int h=1;

        //Calculating L(1)...
        cout<<"\nfrequent 1-itemsets are:"<<endl;
        for(int i=1; i<=items; i++)
        {
            if((occurences[1][i]->tid).size() >= partition_min_count)
            {
                occurences[1][h]->tid = occurences[1][i]->tid;
                l_part[1][h] = c_part[1][i];
                print_ll(l_part[1][h]);
                h++;
            }
        }
        size_p_l[1] = h-1;
        int l = size_c[1] + 1;
        //adding local large itemset to global candidate itemsets..
        for(int j=1; j<=size_p_l[1]; j++)
        {
            if(itemset_present_already(l_part[1][j], 1, l-1) == false)
            {
                c[1][l] = l_part[1][j];
                l++;
            }
        }
        size_c[1] = l-1;

        //Now, we have L(1)..

        for(int a=2; a<=items-1; a++)
        {
            join_itemset(a-1);//join L(a-1) with L(a-1)..
            int h=1;
            l_part[a].clear();
            l_part[a].resize(100000);
            cout<<"\nfrequent "<<a<<"-itemsets are:"<<endl;
            for(int j=1; j<=size_p_c[a]; j++)
            {
                if((occurences[a][j]->tid).size() >= partition_min_count)
                {

                    occurences[a][h]->tid = occurences[a][j]->tid;//cout<<"ading";
                    l_part[a][h] = c_part[a][j];
                    print_ll(l_part[a][h]);
                    h++;
                }
            }
            size_p_l[a] = h-1;
            int l = size_c[a] + 1;
            //adding local large itemset to global candidate itemsets..
            for(int j=1; j<=size_p_l[a]; j++)
            {
                if(itemset_present_already(l_part[a][j], a, l-1) == false)
                {
                    c[a][l] = l_part[a][j];
                    l++;
                }
            }
            size_c[a] = l-1;

            cout<<endl;
        }
    }
    cout<<"-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    //At this point, all local frequent itemsets have been evaluated..
    cout<<"\nStarting Phase-2";
    cout<<"\nThe global candidate itemsets are:\n";

    for(int a=1; a<=items-1; a++)
    {
        cout<<"\nCandidate "<<a<<"-itemsets are:\n";
        for(int i=1; i<=size_c[a]; i++)
        {
            print_ll(c[a][i]);
        }
        cout<<"\n";
    }

    for(int k=1; k<=t; k++)
    {
        for(int i=1; i<=items; i++)
        {
            if(trans[k][i])
            {
                (final_scan[i]->tid).push_back(k);
            }
        }
    }

    //Now, count occurences of all global candidate itemsets..

    //generating L(1)
    int d=1,h=1;
    while(h <= size_c[1])
    {
        int k = c[1][h]->data;
        if((final_scan[k]->tid).size() >= min_count)
        {
            l[1][d] = c[1][h];
            d++;
        }
        h++;
    }
    size_l[1] = d-1;//We have L(1) now..

    for(int a=2; a<=items-1; a++)
    {
        int d=1;
        for(int i=1; i<=size_c[a]; i++)
        {
            phase2_intersection(c[a][i],a);//cout<<"xfg";
            if(temp.size() >= min_count)
            {
                l[a][d++] = c[a][i];
            }

        }
        size_l[a] = d-1;
    }
    cout<<"-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

    cout<<"\nThe global frequent itemsets are:\n";
    for(int a=1; a<=items-1; a++)
    {
        cout<<"\NFrequent "<<a<<"-itemsets are:\n";
        for(int i=1; i<=size_l[a]; i++)
        {
            print_ll(l[a][i]);
        }
        cout<<"\n";
    }

    cout<<"-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    cout<<"\nPartitioning - based Apriori Algorithm finished execution\n";
    return 0;
}
