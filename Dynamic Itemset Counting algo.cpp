/**
    Implementation of Dynamic Itemset Counting(DIC) Algorithm
**/
/**
    Author: Sudheesh Singanamalla
    B.Tech CSE(2012-16), National Institute of Technology, Warangal
**/

#include<iostream>
#include<cstdio>
#include<algorithm>
#include<cstring>
#include<cstdlib>
#include<cctype>
#include<cmath>
#include<climits>
#include<vector>
#include<map>
#include<iterator>
#include<set>
#include<bitset>
#include<ctime>
#include<iomanip>
#include<fstream>
#include<string>
#include<stdlib.h>
//#include "itoa.h"

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
struct itemset
{
    string id;
    node* i_set;
    int counter;
    int size_set;
    int start;
};
struct cmp_str
{
    bool operator()(char const *a, char const *b)
    {
        return std::strcmp(a, b) < 0;
    }
};
vector< tid_set *> temp_occurences(1), final_scan(1);
vector<itemset*> SS(1), SC(1), DS(1), DC(1), temp_DC(1), temp_DS(1);
vector<itemset*> :: iterator it;
int size_SS,size_SC, size_DS, size_DC,size_temp_DS, size_temp_DC;
int trans[1000][1000];
int t;
int items;
int min_sup = 10;// minimum support in percent.
int min_count;
int m = 3;
int start, stop;
map< string, bool> SS_DS, taken;

 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

void itoa(int n, char s[], int base)
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

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

string itemset_to_string(node* h)
{
    char temp[1000000], integ[10000];
    temp[0] = '\0';
    while(h != NULL)
    {
        itoa(h->data, integ, 10); //converts itemno. to a char array..
        strcat(temp, integ);
        strcat(temp, " ");
        h=h->next;
    }
    string s(temp);
    return s;
}

vector<int> temp_vec,temp1;
void tid_intersection(node* l, int k)
{
    temp_vec.clear();
    vector<int> present_temp(1),pres(1);
    pres.resize(t+1);
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
        temp1 = (temp_occurences[l->data])->tid;
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
    //cout<<"( ";
    for(int j=1; j<=t; j++)
    {
        if(present_temp[j])
        {
            cout<<j<<" , ";
            temp_vec.push_back(j);
        }
    }
    //cout<<")\n";
}

bool check_subsets(node* q, int k)
{
    int flag = 0;
    int i=1;
    node *t = NULL,*h = q;
    while(i <= k)
    {
        t = NULL;
        h = q;
        for(int j=1; j<i; j++)
        {
            add_ll(t,h->data);
            h = h->next;
        }
        h = h->next;
        for(int j=i+1; j<=k; j++)
        {
            add_ll(t,h->data);
            h = h->next;
        }//we have k-1 subset here..

        string c = itemset_to_string(t);
        if(SS_DS[c] == false)//if itemset is not solid or dashed square
        {
            print_ll(q);
            cout<<" rejected because "<<c<<"does not belong to squared-itemsets\n";
            return false;
        }
        i++;
    }//cout<<"hello2";
    return true;//all subsets are solid/dashed squares..
}

vector<int> superset(1),temp2(1),temp3(1);
vector<bool> p(1);
void construct_check_superset(node* l, int k)
{
    cout<<"checking supersets of ";
    print_ll(l);
    cout<<"\n";
    temp2.resize(items+1);
    p.clear();
    p.resize(items+1);
    temp2[0] = 0;
    int temp_size=0;
    while(temp_size != k)
    {
        temp2[++temp_size] = l->data;
        p[l->data] = true;
        l = l->next;
    }
    for(int i =1; i<=items; i++)
    {
        if(p[i] == false)
        {
            temp3.clear();

            temp3 = temp2;
            temp3.resize(items+1);
            temp3[++temp_size] = i;
            sort(temp3.begin(), temp3.begin() + temp_size + 1);
            node *q = NULL;
            for(int j=1; j<=temp_size; j++)
            {
                add_ll(q, temp3[j]);
            }
            if(check_subsets(q, k+1)==true && taken[itemset_to_string(q)]!=true)
            //if all immediate subsets are solid/dashed squares
            {
                itemset *temp = new itemset();
                temp->i_set = q;
                temp->id = itemset_to_string(q);
                temp->counter = 0;
                temp->size_set = k+1;
                temp->start = -5;//dummy value..
                temp_DC[++size_temp_DC] = temp;
                taken[itemset_to_string(q)] = true;
                print_ll(q);
                //the superset has been added to dashed circle..
            }
            temp_size--;
        }

    }
}

int main()
{
    cout<<"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

    ifstream fin("trans6.txt");
    ofstream fout;
    fin>>items>>t;
    int i=1;
    temp_occurences.resize(items+1);

    while(i<=t)
    {
        for(int j=1; j<=items; j++)
        {
            fin>>trans[i][j];
        }
        i++;
    }
    //min_count = (int)((ceil)(((double)(min_sup * t))/100));
    min_count = 2;
    DS.resize(100000);
    SS.resize(100000);
    DC.resize(100000);
    SC.resize(100000);
    temp_DS.resize(100000);//used for transferring itemsets from DC to DS..
    temp_DC.resize(100000);//used for transferring new itemsets to DC..
    size_DS = 0;
    size_SS = 0;
    size_DC = 0;
    size_SC = 0;

    temp_occurences.resize(items+1);
    for(int i=1; i<=items; i++)
    {
        temp_occurences[i] = new tid_set();
        itemset *temp = new itemset();
        node *t = NULL;
        add_ll(t ,i);
        temp->i_set = t;
        cout<<"adding \t";
        print_ll(temp->i_set);
        cout<<" to DC\n";
        temp->id = itemset_to_string(t);
        temp->counter = 0;
        temp->size_set = 1;
        temp->start = 1;
        DC[++size_DC] = temp;
    }//all 1-itemsets have been marked as dashed-circles..
    stop = 0;
    int read = 1;

    //start reading the m transaction at a time
    while(size_DS > 0 || size_DC > 0)
    {
        cout<<"\nreadno."<<read<<endl;

        read++;
        start = (stop+1);
        if(start > t)
            start = 1;
        stop = (start + m - 1);
        if(stop > t)
            stop = t;
        cout<<"\nstart,stop = "<<start<<" "<<stop<<endl;
        size_temp_DC = 0;
        size_temp_DS = 0;
        for(int i=1; i<=items; i++)
        {
            (temp_occurences[i]->tid).clear();
        }

        for(int i=start; i<=stop; i++)
        {
            for(int j=1; j<=items; j++)
            {
                if(trans[i][j] == 1)
                {
                    (temp_occurences[j]->tid).push_back(i);
                }
            }
        }//constructed a tid-list for all items in the m transactions.

        cout<<"\nDC\n";
        for(int i=1; i<=size_DC; i++) //count the occurences of dashed-circles
        {
            tid_intersection(DC[i]->i_set, DC[i]->size_set);
            DC[i]->counter += temp_vec.size();
            if(DC[i]->counter  >= min_count)//if the dashed temset is frequent
            {
                //remove the itemset from DC and add it to temp_DS..
                print_ll( DC[i]->i_set);
                cout<<" has been removed from DC to DS\n";
                temp_DS[++size_temp_DS] = DC[i];//itemset added to temp_DS..
                SS_DS[DC[i]->id] = true;
                int w=i;
                while(w<size_DC)//shift the itemsets by one to the left
                {
                    DC[w] = DC[w+1];
                    w++;
                }
                i--;
                size_DC -= 1;//removed the itemset from DC
            }
        }
        cout<<"\nDS\n";

        for(int i=1; i<=size_DS; i++) //count the occurences of dashed-squares
        {
            tid_intersection(DS[i]->i_set, DS[i]->size_set);
            DS[i]->counter += temp_vec.size();
        }


        //transferring those itemsets which have covered all transactions, from DC to SC..
        for(int i=1; i<=size_DC; i++)
        {
            if(stop == ((DC[i]->start) - 1) || (stop == t  && DC[i]->start == 1))//the itemset has been counted through all transactions..
            {
                itemset *temp = new itemset();
                temp->i_set = DC[i]->i_set;
                temp->id = DC[i]->id;
                temp->counter = 0;
                temp->size_set = DC[i]->size_set;
                //temp->start = 1;
                SC[++size_SC] = temp;//the itemset has been added to SC..
                print_ll(temp->i_set);
                cout<<" has been removed from DC to SC\n";
                int w=i;
                while(w<size_DC)//shift the itemsets by one to the left
                {
                    DC[w] = DC[w+1];
                    w++;
                }
                i--;
                size_DC -= 1;//removed the itemset from DC
            }
        }

        //checking supersets of those itemsets which are in temp_DS..
        for(int i=1; i<=size_temp_DS; i++)
        {
            construct_check_superset(temp_DS[i]->i_set, temp_DS[i]->size_set);//check all its supersets and add them to temp_DC if necessary..
        }

        //transferring new itemsets from temp_DS to DS
        for(int i=1; i<=size_temp_DS; i++)
        {
            DS[++size_DS] = temp_DS[i];
            SS_DS[temp_DS[i]->id] = true;
        }

        //transferring those itemsets which have covered all transactions, from DS to SS..
        for(int i=1; i<=size_DS; i++)
        {
            if(stop == ((DS[i]->start) - 1) || (stop == t  && DS[i]->start == 1))//the itemset has been counted through all transactions..
            {
                itemset *temp = new itemset();
                temp->i_set = DS[i]->i_set;
                temp->id = DS[i]->id;
                temp->counter = DS[i]->counter;
                temp->size_set = DS[i]->size_set;
                //temp->start = 1;
                SS[++size_SS] = temp;//the itemset has been added to SS..
                SS_DS[temp->id] = true;
                int w=i;
                while(w<size_DS)//shift the itemsets by one to the left
                {
                    DS[w] = DS[w+1];
                    w++;
                }
                i--;
                size_DS -= 1;//removed the itemset from DS
            }
        }
        //transferring new itemsets from temp_DC to DC
        for(int i=1; i<=size_temp_DC; i++)
        {
            DC[++size_DC] = temp_DC[i];
            DC[size_DC]->start = stop+1;
            if(stop+1 > t)
                DC[size_DC]->start = 1;
		//the new itemset will be counted from the next reading of transaction, hence the new start value..

        }
        cout<<"itemsets in DC are:\n";
        for(int i=1; i<=size_DC; i++)
        {
            print_ll(DC[i]->i_set);
        }
        cout<<"\nitemsets in DS are:\n";
        for(int i=1; i<=size_DS; i++)
        {
            print_ll(DS[i]->i_set);
        }
        cout<<"\n";
    }

        for(int j=1;j<=items;j++)
        {
                temp_occurences[j]->tid.clear();
        }
    //preparing the final temp_occurences..
    for(int i=1;i<=t;i++)
    {
        for(int j=1;j<=items;j++)
        {
            if(trans[i][j])
                temp_occurences[j]->tid.push_back(i);
        }
    }

    //the frequent itemsets are..
    cout<<"The following are the frequent itemsets:\n";
    for(int i=1; i<=size_SS; i++)
    {
        cout<<SS[i]->id;
        cout<<"   is present in transactions : ";
        tid_intersection(SS[i]->i_set, SS[i]->size_set);
         cout<<"\n";
    }
    return 0;
}
