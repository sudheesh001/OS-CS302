/*
	Frequent patterns are being generated but their count is not being shown correctly.
*/

#include<iostream>
#include<fstream>
#include<cstdio>
#include<ctime>
#include<vector>
#include<algorithm>
#include<iterator>
#include<map>
#include<cmath>

#define DATA_ITEMS 5

using namespace std;

typedef map <vector <int>, int> itemset;
typedef pair<vector <int>, int> itempair;

vector<int> T[50];
int t_num, min_sup = 2;
itemset Fq, Fqq;

/*struct fp_node
{
	int item;
	int count;
	fp_node* left_child;
	fp_node* right_sibling;	
};*/

struct fp_node
{
	int item;
	int count;
	vector<fp_node*> child;		// for children
	fp_node* next;				// for elements of list
	fp_node* parent;			// to point to the parent  node
};

struct side_list
{
	int item;
	int count;
	fp_node *next;	
};

vector<side_list> mylist;	// index 0 to DATA_ITEMS-1
int item_count[DATA_ITEMS+1] = {0,0};		// index 1 to DATA_ITEMS

// next_node[i] points to the node where next pointer of item i+1 is pointing
fp_node* next_node[DATA_ITEMS];		// side_list index is from 0
		
void print_transactions();
void print_side_list(vector <side_list> *l);

// clears the vector array T[] storing transactions
void initialise_transactions()
{
	for(int i=1 ; i<=t_num ; ++i)
		T[i].clear();	
}

bool compare_list_node(const side_list &p1, const side_list &p2)
{
	if(p1.count > p2.count)		// decreasing order
		return true;
	return false;
}

bool compare_item_count(const int &p1, const int &p2)
{
	if(item_count[p1] > item_count[p2])		// decreasing order
		return true;
	return false;
}

bool compare_int(const int &a, const int &b)
{
	if(a<b)
		return true;
	return false;	
}

void print_int(const int&a)
{
	cout << a << " ";
}

// T points to the root of the tree, v is the vector that has to be added
// By the implementation, T need not be a reference pointer.
void insert_fptree(fp_node* &T, vector <int> v, int increment=1);

void print_fptree(fp_node* T);

// Assumes that the side_list mylist has not been sorted in L order
void give_next_pointer(fp_node *T);

void display_itemset(itemset* L);

// removes those nodes whose count is less than ms
void prune_fptree(fp_node *T, int ms);

void delete_nodes(fp_node* &p);

// Takes root of a tree and prints all the root to leaf paths starting with 'tree_item'
void generate_path_combinations(fp_node *sub_tree, int tree_item);
void generate_path_c(int A[], int end, fp_node *T);

// prints all the combinations of k-items from array A[start..end]
void make_combination(int A[], int comb[], int c, int k, int start, int end);

//----------------
void generate_path_combinations_with_freq(fp_node *sub_tree, int tree_item);
void generate_path_c_with_freq(int A[], int B[], int end, fp_node *T);		// A has items, B has corresponding freqency
void make_combination_with_freq(int A[], int B[], int comb[], int fcomb[], int c, int k, int start, int end); // k combinations of A[] using comb[]. B[] and fcomb[] just change correspondingly
//----------------

int main()
{
	ifstream fin;
	char tr[10];
	int pause, i, value, k, j;
	vector <int> testv;
	bool done;
    fin.open("tree_dataset.txt");
    //fin.open("mydataset.txt");
    
	if(!fin)
	{
		cout << "File opening failed big time\n";
		cin>>pause;
		return -1;
	}
	
	initialise_transactions();
	
	// enter default data in mylist
	for(i=0 ; i<DATA_ITEMS ; ++i)
	{
		side_list node;
		node.item = i+1;
		node.count = 0;
		node.next = NULL;
		mylist.push_back(node);
	}
	
	fin >> tr;
    fin >> tr;	// take the first item number
    // the items are numbered from 1 to DATA_ITEMS
    t_num = 1;
    while(!fin.eof())
    {
     	value = atoi(tr);	// gives 0 for non-integer data
    	while(value != 0 && !fin.eof())  
     	{
       		T[t_num].push_back(value);		// save value in transaction
       		mylist[value-1].count++;		// increment count of item in mylist vector
        	fin>>tr;
    		value = atoi(tr);
     	}
     	t_num++;
    	fin >> tr;
    }
    t_num--;  
    fin.close();
    
    // the transactions have been saved in main memory
    cout << "Original transactions are\n";
	print_transactions();
	
	//print_side_list(&::mylist);
	
	// save the frequency of each item in ::item_count[] array
	for(i=0 ; i<mylist.size() ; ++i)
		::item_count[i+1] = mylist[i].count;
	
	// sort the side list in decending order after making the tree and then giving the next pointers
	
	cout << "Item count is:\n";
	for(i=1 ; i<=DATA_ITEMS ; ++i)
		cout << ::item_count[i] << " "; cout << endl;
		
	// sort all the transactions based on ::item_count[] array
	for(i=1 ; i<=t_num ; ++i)
		sort(T[i].begin() , T[i].end() , compare_item_count);
	
	cout << "Sorted transactions are\n";
	print_transactions();
	
	fp_node *root = NULL;		// make root of the tree
	root = new fp_node;
	root->item = -1;
	root->count = -1;
	root->next = NULL;
	root->child.clear();
	root->parent = NULL;
		
	for(i=1 ; i<=t_num ; ++i)
	{
		testv.clear();
		testv = T[i];
		//cout << endl;
		//for(j=0 ; j<testv.size() ; ++j)
		//cout  << testv[j] << " "; cout << endl;
		insert_fptree(root, testv);
	}
	
	cout << "Postorder traversal of tree is\n";
	cout << "--------------------------------------------\n";	
	print_fptree(root);
	cout << "--------------------------------------------\n";
	
	// next_node[] array helps to assign next pointers to all the nodes of the tree
	for(i=0 ; i<DATA_ITEMS ; ++i)
		next_node[i] = mylist[i].next;
		
	// do postorder traversal of the tree and give next pointers to all the nodes
	give_next_pointer(root);
	
	// sort the side list in decending order
	sort(::mylist.begin(), ::mylist.end() , compare_list_node);
		cout << "The side list is\n";
	print_side_list(&::mylist);
	
	// *************************************************************************************************************************
	
	itemset C;		// to store the condition pattern base (vector with count)
	Fq.clear(); Fqq.clear();
	fp_node *p;
	for(i=DATA_ITEMS-1 ; i>=0 ; --i)
	{
		p = mylist[i].next;
		int tree_item = p->item;
		C.clear();
		while(p)
		{
			vector <int> v;
			int freq = p->count;
			fp_node *pp = p->parent;
			while(pp->parent != NULL)
			{
				v.push_back(pp->item);
				pp = pp->parent;
			}
			if(v.size()>0)
			{
				reverse(v.begin(), v.end());
				C.insert(itempair(v, freq));
			}
			p = p->next;
		}
		if(C.size()>0)
		{
			// print the conditional pattern base
			cout << "\nConditional pattern base\n";
			display_itemset(&C);
			
			// make small fp tree from the conditional code base
			fp_node *sub_root = NULL;
			sub_root = new fp_node;
			sub_root->item = -1;
			sub_root->count = -1;
			sub_root->next = NULL;
			sub_root->child.clear();
			sub_root->parent = NULL;
			
			for(itemset::iterator it = C.begin() ; it != C.end() ; ++it)
			{		
				testv.clear();
				testv = it->first;
				insert_fptree(sub_root, testv, it->second);
			}
			cout << "\nFP tree for the pattern base\n";
			//cout << "--------------------------------------------\n";
			//print_fptree(sub_root);
			//cout << "--------------------------------------------\n";
			prune_fptree(sub_root, ::min_sup);
			cout << "--------------------------------------------\n";
			print_fptree(sub_root);
			cout << "--------------------------------------------\n";
			
			//generate_path_combinations(sub_root, tree_item);
			//cout << "+++\n";
			generate_path_combinations_with_freq(sub_root, tree_item);
		}
	}
	
	// print all the frequent item-sets stored in Fq and Fqq
	//cout << "---> Fq is <---\n";
	//display_itemset(&::Fq);
	cout << "---> Fqq is <---\n";
	display_itemset(&::Fqq);
	cout<<"the end\n";
	
	return 0;	
}

void give_next_pointer(fp_node *T)
{
	for(int i=0 ; i<T->child.size() ; ++i)
		give_next_pointer(T->child[i]);
	if(T->item == -1) return;	// root node of the tree
	int itm = T->item;
	if(next_node[itm-1] == NULL)	// first link of the side_list
	{
		mylist[itm-1].next = T;
		next_node[itm-1] = T;
	}
	else
	{
		next_node[itm-1]->next = T;		// continuation of the side_list
		next_node[itm-1] = next_node[itm-1]->next;
	}
}

void print_side_list(vector <side_list> *l)
{
	cout << "--------------------------------------------\n";
	for(int i=0 ; i<(*l).size() ; ++i)
	{
		cout << (*l)[i].item << "-" << (*l)[i].count << ": ";
		fp_node *t = (*l)[i].next;
		while(t)
		{
			cout << t->item << ":" << t->count << " ";
			t = t->next;
		}
		cout << endl;
	}
	cout << "--------------------------------------------\n";
}

void print_transactions()
{
	cout << "--------------------------------------------\n";
	for(int i=1 ; i<=::t_num ; ++i)
	{
		cout<<"T"<<i<<":\t";
		for(int j=0 ; j<T[i].size() ; ++j)
			cout<<T[i][j]<<" ";
		cout<<endl;
	}
    cout << "--------------------------------------------\n";	
}

void insert_fptree(fp_node* &T, vector <int> v, int increment)
{
	if(v.size() == 0)
		return;
	int i,j;
	bool done = false;
	for(j=0 ; j<T->child.size() ; ++j)
	{
		if(T->child[j]->item == v[0])
		{
			//T->child[j]->count += ::item_count[v[0]];
			T->child[j]->count += increment;
			vector <int>::iterator it = v.begin();
			v.erase(it);	// removing the first element of vector
			insert_fptree(T->child[j], v, increment);
			done = true;
			break;
		}
	}
	if(!done)
	{
		fp_node *node = new fp_node;
		node->item = v[0];
		node->count = increment;	// initial value is increment
		node->next = NULL;
		node->child.clear();
		node->parent = T;
		T->child.push_back(node);
		vector <int>::iterator it = v.begin();
		v.erase(it);	// removing the first element of vector
		insert_fptree(T->child[T->child.size()-1], v, increment);
	}
	return;
}

void print_fptree(fp_node* T)
{
	if(!T) return;		// this step is needed to print a pruned tree having null childrenin between
	for(int i=0 ; i<T->child.size() ; ++i)
		print_fptree(T->child[i]);
	cout << T->item << " : " << T->count;
	cout << ", parent ";
	if(T->parent) cout << T->parent->item << " : " << T->parent->count << "\n";
	else cout << "NULL\n";
}

void display_itemset(itemset* L)
{
	int count = 1;
	cout << "--------------------------------------------\n";
	for(itemset::iterator it = L->begin() ; it != L->end() ; ++it)
	{
		cout << count++ << ")\t";
		// it->first points to the vector
		for(int i=0 ; i<it->first.size() ; ++i)
			cout << it->first[i] << "-";
		// it->second points to the integer value	
		cout << " : " << it->second << "\n";
	}
	cout << "--------------------------------------------\n";
}

void delete_nodes(fp_node* &p)
{
	for(int i=0 ; i<p->child.size() ; ++i)
	{
		delete_nodes(p->child[i]);
		p->child[i] = NULL;
		p->child.clear();
	}
	p->parent = NULL;
	delete p;
}

void prune_fptree(fp_node *T, int ms)
{
	for(int i=0 ; i<T->child.size() ; ++i)
	{
		if(T->child[i]->count < ms)
		{
			fp_node *p = T->child[i];
			T->child[i]->parent = NULL;
			T->child[i] = NULL;		// the child pointer is not actually removed form the list!
			delete_nodes(p);
		}
		else
			prune_fptree(T->child[i], ms);
	}
}

void generate_path_combinations(fp_node *T, int tree_item)
{
	int end;
	int A[DATA_ITEMS];	// to store the list of items
	bool done = false;
	A[0] = tree_item;		// the freq also needs to saved here!
	end = 0;
	
	for(int i=0 ; i<T->child.size() ; ++i)
	{
		if(T->child[i])	// to consider pruned trees also
		{
			done = true;
			generate_path_c(A, end, T->child[i]);
		}
	}
	if(!done)	// no non-null child was found. < This part should NOT be reached ever >
	{
		
		// to print the path. A[0..end] consists of a root-leaf path
		cout << ">>> should not come here ";
		for(int i=0 ; i<=end ; ++i)
			cout << A[i] << " ";
		cout << "\n";
		
		int comb[DATA_ITEMS];
		for(int k=1 ; k<=end+1 ; ++k)	// to generate all possible k-item combinations for the array A[]
			make_combination(A, comb, 0, k, 0, end);
	}
}

void generate_path_c(int A[], int end, fp_node *T)
{
	A[++end] = T->item;		// the count of the item also needs to be saved somewhere !
	
	bool done = false;
	for(int i=0 ; i<T->child.size() ; ++i)
	{
		if(T->child[i])		// this case is needed as there may be null child due to pruning done before
		{
			done = true;
			generate_path_c(A, end, T->child[i]);
		}
	}
	if(!done)	// no non-null child was found then print this path. < This part will be executed! >
	{
		
		// to print the path. A[0..end] consists of a root-leaf path
		cout << ">>> ";
		for(int i=0 ; i<=end ; ++i)		// the frequency of this set is the min of all the frequencies of individual items
			cout << A[i] << " ";
		cout << "\n";
		
		int comb[DATA_ITEMS];
		for(int k=1 ; k<=end+1 ; ++k)	// to generate all possible k-item combinations for the array A[]
			make_combination(A, comb, 0, k, 0, end);
		// for each combination, the freq is the min frequency amongst all the individual frequencies of the items of the combination
		// but freq is not saved now in this code!
	}
}

void make_combination(int A[], int comb[], int c, int k, int start, int end)
{
	//print all the single elements of A along with comb array
	if(k == 1)
	{
		vector <int> v;
		for(int i=start ; i<=end ; ++i)
		{
			v.clear();	// store all elements of comb[] in v
			for(int j=0 ; j<=c-1 ; ++j)
			{
				v.push_back(comb[j]);
			}
			//cout << A[i] << "\n";
			v.push_back(A[i]);
			sort(v.begin(), v.end(), compare_int);
			Fq.insert(itempair(v, 0));			// frequency still not saved ! Duplicate is not stored. That's good
		}
		return;
	}
	for(int i=start ; i<=end-k+1 ; ++i)	// this i gives the index of the first element to be put in comb[]
	{
		comb[c] = A[i];
		make_combination(A, comb, c+1, k-1, i+1, end);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

void generate_path_combinations_with_freq(fp_node *T, int tree_item)
{
	int end;
	int A[DATA_ITEMS], B[DATA_ITEMS];	// to store the list of items and corresponding frequency
	bool done = false;
	A[0] = tree_item;		// the freq also needs to saved here!
	B[0] = 10000;	// INF value
	end = 0;	// last filled index of A and B
	
	for(int i=0 ; i<T->child.size() ; ++i)
	{
		if(T->child[i])	// to consider pruned trees also
		{
			done = true;
			generate_path_c_with_freq(A, B, end, T->child[i]);
		}
	}
	if(!done)	// no non-null child was found. < This part should NOT be reached ever >
	{
		
		// to print the path. A[0..end] consists of a root-leaf path
		cout << ">>> should not come here ";
		for(int i=0 ; i<=end ; ++i)
			cout << A[i] << "-" << B[i] << " ";
		cout << "\n";
	}
}

void generate_path_c_with_freq(int A[], int B[], int end, fp_node *T)
{
	// cout << "Item added = " << T->item << "-" << T->count << "\n";
	A[++end] = T->item;		// the count of the item also needs to be saved somewhere !
	B[end] = T->count;
	
	bool done = false;
	for(int i=0 ; i<T->child.size() ; ++i)
	{
		if(T->child[i])		// this case is needed as there may be null child due to pruning done before
		{
			done = true;
			generate_path_c_with_freq(A, B, end, T->child[i]);
		}
	}
	if(!done)	// no non-null child was found then print this path. < This part will be executed! >
	{
		// to print the path. A[0..end] consists of a root-leaf path
		/*cout << ">>> ";
		for(int i=0 ; i<=end ; ++i)		// the frequency of this set is the min of all the frequencies of individual items
			cout << A[i] << "-" << B[i] << " ";
		cout << "\n";*/
		
		int comb[DATA_ITEMS];
		int fcomb[DATA_ITEMS];
		comb[0] = A[0]; fcomb[0] = B[0];	// first element is already occupied. Make combination from index 1 to end.
		for(int k=1 ; k<=end+1 ; ++k)	// to generate all possible k-item combinations for the array A[]
			make_combination_with_freq(A, B, comb, fcomb, 1, k, 1, end);
		// start index is 1 because index 0 has the element that has to be present with all the combinations. ie, all printed combinations
		// will be atleast two item sets.
	}
}

void make_combination_with_freq(int A[], int B[], int comb[], int fcomb[], int c, int k, int start, int end)
{
	// print all the single elements of A along with comb array
	if(k == 1)
	{
		vector <int> v;
		int min_freq = 1000;
		for(int i=0 ; i<=c-1 ; ++i)
			if(fcomb[i] < min_freq)
				min_freq = fcomb[i];
				
		for(int i=start ; i<=end ; ++i)
		{
			v.clear();	// store all elements of comb[] in v
			for(int j=0 ; j<=c-1 ; ++j)
			{
				v.push_back(comb[j]);
			}
			
			v.push_back(A[i]);
			sort(v.begin(), v.end(), compare_int);
			
			// the freq is the min amongst fcomb and B[i]
			int f = (B[i] < min_freq) ? B[i] : min_freq;
			bool done = false;
			for(itemset::iterator it = Fqq.begin() ; it != Fqq.end() ; ++it)
				if(it->first == v)
				{
					//cout << "freq added for vector : ";
					//for_each(v.begin() , v.end() , print_int); cout << ": " << f << endl;
					it->second += f;
					done = true;
					break;
				}
			if(!done)
			{
				Fqq.insert(itempair(v, f));			// frequency still not saved ! Duplicate is not stored. That's good
				//cout << "added vector : ";
				//for_each(v.begin() , v.end() , print_int); cout << ": " << f << endl;
			}
		}
		return;
	}
	for(int i=start ; i<=end-k+1 ; ++i)	// this i gives the index of the first element to be put in comb[]
	{
		comb[c] = A[i];
		fcomb[c] = B[i];
		make_combination_with_freq(A, B, comb, fcomb, c+1, k-1, i+1, end);
	}
}

