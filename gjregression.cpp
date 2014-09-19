/**
    Nth - degree polynomial regression using Gauss-Jordan elimination..
    The program returns the best-fit curve of mentioned degree/ order, that fits the given set of data points.

    Reference: 1) http://arachnoid.com/sage/polynomial.html
                        2) http://www.algebra.com/algebra/homework/Linear-equations/THEO_2010.lesson?content_action=show_source
**/

#include<iostream>
#include<fstream>
#include<algorithm>
#include<iomanip>
#include<cmath>

using namespace std;

long double aug_mat[1000][1000]; // this is the augmented matrix..
long double point_x[10000], point_y[10000];
int degree, points;

void calculate_cell(int row, int col)
{
    long double result = 0.0;
    for(int i=0;i<points;i++)
    {
        result += (long double)(pow(point_x[i], row + col));
    }
    aug_mat[row][col] = result;
}

void calculate_constant_column(int row)
{
    long double result = 0.0;
    for(int i=0;i<points;i++)
    {
        result += (long double)((long double)(pow(point_x[i], row)) * (long double)(point_y[i]));
    }
    aug_mat[row][degree+1] = result;
}

void divide_by_constant(int row, long double constant)
{
    for(int i=0; i<degree+2; i++)
    {
        aug_mat[row][i] = ((long double)(aug_mat[row][i])) / constant;
    }
}

void multiply_by_constant(int row, long double constant)
{
    for(int i=0; i<degree+2; i++)
    {
        aug_mat[row][i] = ((long double)(aug_mat[row][i])) * constant;
    }
}

void add_row_to_another(int row1, int row2, long double constant)
{
    for(int i=0; i<degree+2; i++)
    {
        aug_mat[row1][i] = aug_mat[row1][i] + (((long double)(aug_mat[row2][i])) * constant);
    }
}
void display()
{
    cout<<endl;
    for(int i=0; i<degree+1; i++)
    {
        for(int j=0; j<degree+2; j++)
        {
            cout<<setw(15)<<aug_mat[i][j]<<" ";
        }
        cout<<endl;
    }
}

void forward_processing(int col)
{
    if(aug_mat[col][col] == 0)
    {
        int row = col;
        for(int i=col; i<degree+1; i++)
        {
            if(aug_mat[i][col] != 0)
            {
                row = i;
                break;
            }
        }
        for(int i=0; i<degree+2; i++)
        {
            int temp = aug_mat[col][i];
            aug_mat[col][i] = aug_mat[row][i];
            aug_mat[row][i] = temp;
        }
    }
    //make the element aug_mat[col][col] equal to 1
    divide_by_constant(col, aug_mat[col][col]); // divide the whole row by the element aug_mat[col][col]..

    // make the elements aug_mat[i][col] equal to 0, for all subseqent i..
    for(int i=col+1; i<degree+1; i++)
    {
        add_row_to_another(i, col, (-1) * aug_mat[i][col]);
    }
}

void backward_processing(int col)
{
    for(int i=col-1; i>=0; i--)
    {
        add_row_to_another(i, col, (-1) * aug_mat[i][col]);
    }
}

int main()
{
    ifstream fin("test7.txt");
    ofstream fout;
    fin>>degree;
    fin>>points;
    // int degree;
    //cin>>degree;
    for(int i=0; i<points; i++)
    {
        fin>>point_x[i]>>point_y[i];
    }
    for(int i=0; i<degree+1; i++)
    {
        for(int j=0; j<degree+2; j++)
        {
            calculate_cell(i, j);
        }
    }
    for(int i=0;i<degree+1;i++)
    {
        calculate_constant_column(i);
    }
    cout<<"\nThis is the augmented matrix for Gauss-Jordan Elimination\n";
    display();

    for(int i=0; i<degree+1; i++)
    {
        forward_processing(i);
    }
    for(int i=degree; i>=0; i--)
    {
        backward_processing(i);
    }
    cout<<endl;
    cout<<"\n The best-fit curve of degree "<<degree<<" for the given data points is:\n";
    cout<<"y =  ";
    for(int i=degree;i>0;i--)
    {
        cout<<"("<<aug_mat[i][degree+1]<<") x^ "<<i<<"  +  ";
    }
    cout<<aug_mat[0][degree+1]<<endl;
}
