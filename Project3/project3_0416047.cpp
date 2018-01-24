/* Project3_Sudoku_Solution_Validator */
/* 0416047_Andy_Wang */

#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <algorithm>

using namespace std;

/* validation function */
bool validator(vector<int> unit)
{
	sort( unit.begin(), unit.end() );
	bool flag = true;
	for(int i=0; i<9; i++)if( unit[i] != (i+1) )flag = false;
	return flag;
}

int main(int argc, char *argv[])
{
	ifstream inf;
	string file = argv[1];
	string str;
	vector< string >input;
	vector< vector<int> > row;
	vector< vector<int> > column;
	vector< vector<int> > subgrid;
	
	/* open txt file */
	inf.open(file, ios::in);//PLEASE ENTER THE INPUT FILE NAME HERE!
	if(!inf) cout<<"Error!"<<endl;
	
	/* vector input */
	for(int i=0; i<9; i++)
	{
		getline(inf, str);
		input.push_back(str);
		str.clear();
	}
	
	/* vector row */
	for(int i=0; i<9; i++)
	{
		vector<int> temp;
		str = input[i];
		for(int j=0; j<9; j++)temp.push_back(str[j]-'0');
		row.push_back(temp);
		str.clear();
	}
	
	/* vector column */
	for(int i=0; i<9; i++)
	{
		vector<int> temp;
		for(int j=0; j<9; j++)
		{
			str = input[j];
			temp.push_back(str[i]-'0');
		}
		column.push_back(temp);
		str.clear();
	}
	
	/* vector subgrid */
	vector<int> subgrid_temp1;
	vector<int> subgrid_temp2;
	vector<int> subgrid_temp3;
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			str = input[3*i+j];

			subgrid_temp1.push_back(str[0]-'0');
			subgrid_temp1.push_back(str[1]-'0');
			subgrid_temp1.push_back(str[2]-'0');

			subgrid_temp2.push_back(str[3]-'0');
			subgrid_temp2.push_back(str[4]-'0');
			subgrid_temp2.push_back(str[5]-'0');

			subgrid_temp3.push_back(str[6]-'0');
			subgrid_temp3.push_back(str[7]-'0');
			subgrid_temp3.push_back(str[8]-'0');

			str.clear();
		}
		subgrid.push_back(subgrid_temp1);
		subgrid.push_back(subgrid_temp2);
		subgrid.push_back(subgrid_temp3);
		
		subgrid_temp1.clear();
		subgrid_temp2.clear();
		subgrid_temp3.clear();
	}
	
	/* multi-thread validation */
	bool result = true;
	
	/* WAY 1: use 27 thread */
	/*
	future<bool> row_v1 = async(launch::async, validator, row[0]);
	future<bool> row_v2 = async(launch::async, validator, row[1]);
	future<bool> row_v3 = async(launch::async, validator, row[2]);
	future<bool> row_v4 = async(launch::async, validator, row[3]);
	future<bool> row_v5 = async(launch::async, validator, row[4]);
	future<bool> row_v6 = async(launch::async, validator, row[5]);
	future<bool> row_v7 = async(launch::async, validator, row[6]);
	future<bool> row_v8 = async(launch::async, validator, row[7]);
	future<bool> row_v9 = async(launch::async, validator, row[8]);
	
	future<bool> col_v1 = async(launch::async, validator, column[0]);
	future<bool> col_v2 = async(launch::async, validator, column[1]);
	future<bool> col_v3 = async(launch::async, validator, column[2]);
	future<bool> col_v4 = async(launch::async, validator, column[3]);
	future<bool> col_v5 = async(launch::async, validator, column[4]);
	future<bool> col_v6 = async(launch::async, validator, column[5]);
	future<bool> col_v7 = async(launch::async, validator, column[6]);
	future<bool> col_v8 = async(launch::async, validator, column[7]);
	future<bool> col_v9 = async(launch::async, validator, column[8]);
	
	future<bool> sub_v1 = async(launch::async, validator, subgrid[0]);
	future<bool> sub_v2 = async(launch::async, validator, subgrid[1]);
	future<bool> sub_v3 = async(launch::async, validator, subgrid[2]);
	future<bool> sub_v4 = async(launch::async, validator, subgrid[3]);
	future<bool> sub_v5 = async(launch::async, validator, subgrid[4]);
	future<bool> sub_v6 = async(launch::async, validator, subgrid[5]);
	future<bool> sub_v7 = async(launch::async, validator, subgrid[6]);
	future<bool> sub_v8 = async(launch::async, validator, subgrid[7]);
	future<bool> sub_v9 = async(launch::async, validator, subgrid[8]);
	
	result = row_v1.get() && row_v2.get() && row_v3.get() && row_v4.get() && row_v5.get() && row_v6.get() && row_v7.get() && row_v8.get() && row_v9.get() && col_v1.get() && col_v2.get() && col_v3.get() && col_v4.get() && col_v5.get() && col_v6.get() && col_v7.get() && col_v8.get() && col_v9.get() && sub_v1.get() && sub_v2.get() && sub_v3.get() && sub_v4.get() && sub_v5.get() && sub_v6.get() && sub_v7.get() && sub_v8.get() && sub_v9.get();
	*/
	
	/* WAY2: use 3 thread */
	for(int i=0; i<9; i++)
	{
		bool flag;
		
		future<bool> row_valid = async(launch::async, validator, row[i]);
		future<bool> column_valid = async(launch::async, validator, column[i]);
		future<bool> subgrid_valid = async(launch::async, validator, subgrid[i]);
		
		flag = row_valid.get() && column_valid.get() && subgrid_valid.get();
		result = result && flag;
	}
	
	/* print out the result */
	if(result==true)cout<<"TRUE!"<<endl;
	else cout<<"FALSE!"<<endl;
	
	inf.close();
	return 0;
}
