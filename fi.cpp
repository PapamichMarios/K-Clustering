#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <climits>
#include <string>
#include <cmath>

#include "fi.h"
#include "hyperplane.h"

using namespace std;

/*================================ Constructor && Destructor*/
fi::fi(int k, int dim)
{
	int i;
	vector<double> t_list;
	
	this->k   = k;
	this->dim = dim;

	/*== constructing the hs*/
	this->h_array = new Hyperplane_EUC*[k];
	for(i=0; i<k; i++)
	{
		this->h_array[i] = new Hyperplane_EUC(dim);
		
		/*== checking that hs differ*/
		if( std::find(t_list.begin(), t_list.end(), h_array[i]->getT()) != t_list.end() )
		{
			cout << "potential conflict in hs, getting a new one" << endl;
			i-=1;
			delete this->h_array[i];
			
			continue;
		}
		else
			t_list.push_back(this->h_array[i]->getT());
	}

	/*== constructing the rs*/
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution(-50,50);
	
	this->r_array = new int[k];
	for(i=0; i<k; i++)
		this->r_array[i] = distribution(generator);
}

fi::~fi()
{
	delete[] r_array;

	for(int i=0; i<this->k; i++)
	{
		delete h_array[i];
		h_array[i] = NULL;
	}
	delete[] h_array;
		
}
/*================================ Rest of the functions*/
Hyperplane_EUC ** fi::getH_array()
{
	return this->h_array;
}

int * fi::getR_array()
{
	return this->r_array;
}

int fi::hashValue(vector<double> p, int table_size)
{
	int i;
	int sum[this->k];
	long long int hash_val=0;

	/*== for every h compute the floor of (p*v+t)/w */
	for(i=0; i<this->k; i++)
	{
		sum[i] = 0;
		sum[i] = h_array[i]->computeH(p);
	}

	for(i=0; i<this->k; i++)
		hash_val += r_array[i]*sum[i];

	/*== handle overflow*/
	if( hash_val > INT_MAX || hash_val < INT_MIN )
	{
		cout << "Overflow detected(fi), repicking ris and recomputing fi" << endl;

		int d = 50;
		while(hash_val > INT_MAX || hash_val < INT_MIN)
		{
			std::random_device rd;
			std::default_random_engine generator(rd());
			std::uniform_int_distribution<int> distribution(0, d/2);
			hash_val = 0;

			for(i=0; i<k; i++)
			{
				this->r_array[i] = distribution(generator);
				hash_val += r_array[i]*sum[i];
			}
		}
	}

	/*== mod with table size, to fit our hash array*/
	hash_val = hash_val % table_size;

	/*== handle negative hash values*/
	if( hash_val < 0 )
		hash_val = table_size + hash_val;

	return (int)hash_val;
}

int fi::cubeValue(vector<double> x)
{
	string hash_val;

	for(int i=0; i<this->k; i++)
		hash_val += to_string(abs(h_array[i]->computeH(x) % 2));

	return stoi(hash_val, nullptr, 2);
}

string fi::computeG(vector<double> p)
{
	string G;

	for(int i=0; i<k; i++)
		G += to_string(this->h_array[i]->computeH(p)) + ".";	

	return G;
}

long long int fi::memory_used()
{
	long long int memory=0;

	memory += sizeof(this->k);
	memory += sizeof(this->dim);
	/*== r_array*/
	memory += this->k * sizeof(int);
	memory += this->k * h_array[0]->memory_used(this->dim);

	return memory;
}
