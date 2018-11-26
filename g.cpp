#include <vector>
#include <iostream>

#include "g.h"
#include "hyperplane.h"

using namespace std;

/*================================ Constructor && Destructor*/
G::G(int k, int dim)
{
	int i;
	
	this->k   = k;
	this->dim = dim;

	/*== constructing the hs*/
	this->h_array = new Hyperplane_COS*[k];
	for(i=0; i<k; i++)
	{
		this->h_array[i] = new Hyperplane_COS(dim);
		
		/*== checking that hs differ*/
	}
}

G::~G()
{
	for(int i=0; i<k; i++)
	{
		delete this->h_array[i];
		this->h_array[i] = NULL;
	}

	delete[] this->h_array;
}

/*=============================== Rest of the funcs*/
Hyperplane_COS ** G::getH_array()
{
	return this->h_array;
}

int G::hashValue(vector<double> x)
{
	string hash_val;

	/*== take dot product of x with ri*/
	for(int i=0; i<this->k; i++)
		hash_val += to_string(h_array[i]->computeH(x));

	return stoi(hash_val, nullptr, 2);
}

long long int G::memory_used()
{
	long long int memory=0;

	memory += sizeof(this->k);
	memory += sizeof(this->dim);
	memory += this->k * h_array[0]->memory_used(this->dim);

	return memory;
}
