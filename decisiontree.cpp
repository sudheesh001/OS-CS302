/**
    Implementation of decision tree using ID-3 algorithm(for discrete values only)
    Author : Sudheesh Singanamalla
    References: http://www.decisiontrees.net/
**/
#include<iostream>
#include<fstream>
#include<algorithm>
#include<vector>
#include<string>
#include<map>
#include<cmath>

using namespace std;

int t, attributes;
vector<string> attrib(1);

struct dec_node
{
    int attr;
    int num_child;
    dec_node** child;
    string val;

    dec_node(int n)
    {
        num_child = n;
        child = new dec_node*[n];
        for(int i=0; i<num_child; i++)
        {
            child[i] = NULL;
        }
    }
};

class db_tuple
{
public:
    vector<string> values;
    string tuple_class;

    db_tuple()
    {
        values.resize(10000);
    }
    void setAttribute(int attr, string val)
    {
        values[attr] = val;
    }
};

db_tuple database[10000];
dec_node *root = NULL;

void print_db(int index[], int siz)
{
    for(int i=0; i<siz; i++)
    {
        for(int j=0; j<attributes; j++)
        {
            cout<<database[index[i]].values[j]<<" ";
        }
        cout<<endl;
    }

}

int calc_distinct_values(int attr, int index[], vector<string>& val, int siz)
{
    int s = 0;
    map<string, bool> seen;
    seen.clear();
    for(int i=0; i<siz; i++)
    {
        string v = database[index[i]].values[attr];
        if(seen[v] == false)
        {
            val[s++] = v;
            seen[v] = true;
        }
    }
    return s;
}

int create_partition(int attr, string val, int index[], int *sub_index, int siz)
{
    int s = 0;
    for(int i=0; i<siz; i++)
    {
        string v = database[index[i]].values[attr];
        if(v == val)
        {
            sub_index[s++] = index[i];
            //cout<<"inserted "<<index[i]<<endl;
        }

    }
    return s;
}

double extract_info_gain(int index[], int siz)//extracts part of the  info_gain from a given database partition..
{
    double info = 0.0;
    vector<string> val(1);
    val.resize(10000);
    map<string, int> count_val;
    int s = calc_distinct_values(attributes-1, index, val, siz);//the tuple in the database partition belong to 's' different classes..
    for(int i=0; i<siz; i++) // counts the occurences of class in  the partition..
    {
        string s = database[index[i]].values[attributes-1];
        count_val[s] += 1;
    }
    for(int i=0; i<s; i++)
    {
        info += ((double)count_val[val[i]]/siz) * (log(((double)count_val[val[i]]/siz)) / log(2.0));
    }
    info *= (-1.0);
    return info;
}

int attribute_selection(int index[], int siz)
{
    double min_info_gain = 99999999.0, sel_attr;
    //check which attributes can be split..i.e which attributes of database have more than one distinct values?
    for(int i=0; i<attributes-1; i++)
    {
        double info_gain = 0.0;
        vector<string> val(1);
        val.resize(10000);
        int s =  calc_distinct_values(i, index, val, siz);
        if(s > 1)//this attribute can be split since it has more than one distinct value..
        {
            for(int j=0; j<s; j++)
            {
                int sub_index[10000];
                int part_size = create_partition(i, val[j], index, sub_index, siz );
                info_gain += ((double)(part_size)/siz) * (double)extract_info_gain(sub_index, part_size);
            }
            if(info_gain < min_info_gain)
            {
                min_info_gain = info_gain;
                sel_attr =i;
            }
        }
    }
    //cout<<"splitting on "<<sel_attr<<" with info = "<<min_info_gain<<endl;
    return sel_attr;//this is the attribute selected for splitting..
}

void construct_dec_tree(dec_node *& tree, int index[], int attr, string value, int siz)
{
    vector<string> val(1);
    val.resize(10000);
    int p = calc_distinct_values(attributes-1, index, val, siz);
    if(p > 1)
    {
        int sub_index[10000];
        if(attr<0)
        {
            int new_attr  = attribute_selection(index, siz);

            int s = calc_distinct_values(new_attr, index, val, siz);
            tree = new dec_node(s);
            tree->attr = attributes;
            tree->val = "";
            attr = new_attr;
            for(int i=0; i<s; i++)
            {
                int part_size = create_partition(attr, val[i], index, sub_index, siz);
                construct_dec_tree(tree->child[i], sub_index, attr, val[i], part_size);
            }
        }
        else
        {
            int new_attr  = attribute_selection(index, siz);
            int s = calc_distinct_values(new_attr, index, val, siz);
            tree = new dec_node(s);
            tree->attr = attr;
            attr = new_attr;
            tree->val = value;
            for(int i=0; i<s; i++)
            {

                int part_size = create_partition(attr, val[i], index, sub_index, siz);
                construct_dec_tree(tree->child[i], sub_index, attr, val[i], part_size);
            }
        }
    }
    else
    {
        tree = new dec_node(1);
        tree->attr = attr;
        tree->val = value;
        tree->child[0] = new dec_node(1);
        tree->child[0]->attr = attributes-1;
        tree->child[0]->val =  database[index[0]].values[attributes-1];
        tree->num_child = 1;
    }
}

void extract_rules(dec_node* node, string rules)
{
    if(node == NULL)
    {
        int l  = rules.length();
        for(int i=10; i<l-4; i++)
        {
            cout<<rules[i];
        }
        cout<<endl;
        return;
    }

    rules += node->val;
    rules += " ( " + attrib[node->attr] + " ) ";
    rules += " -> ";
    for(int i=0; i<node->num_child; i++)
    {
        extract_rules(node->child[i], rules);
    }
}

int main()
{
    ifstream fin("trans11.txt");
    fin>>t>>attributes;
    cout<<t;
    string s;
    attrib.resize(attributes+1);
    for(int j=0; j<attributes; j++)
    {
        fin>>s;
        attrib[j] = s;
    }
    attrib[attributes] = "1";
    for(int i=0; i<t; i++)
    {
        database[i].values.resize(10000);
        for(int j=0; j<attributes; j++)
        {
            fin>>s;
            database[i].setAttribute(j, s);
        }
    }
    int index[10000];
    for(int i=0; i<t; i++)
    {
        index[i] = i;
    }
    //print_db(index, t);
    construct_dec_tree(root, index, -1, "tree", t);
    //cout<<"drg";
    cout<<"The following are the inferred rules:\n";
    extract_rules(root, "");
    return 0;
}
