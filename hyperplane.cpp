#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <random>
#include <cmath>
#include <climits>

#include "hyperplane.h"
#include "help_functions.h"

#define W 1 

using namespace std;
using namespace help_functions;

/*============================= Constructors && Destructors*/
Hyperplane::Hyperplane(int dim)
{
	/*== vector: single precision coords N(0,1)*/
	this->v = normal_distribution_vector(dim);
}

Hyperplane::~Hyperplane()
{
	free(this->v);
	this->v = NULL;	
}

Hyperplane_EUC::Hyperplane_EUC(int dim) : Hyperplane(dim)
{
	/*== w: 4 (can change from definition)*/
	this->w = W;

	/*== t: uniformly random in [0,w)*/
	random_device rd;
	default_random_engine generator(rd());
	uniform_real_distribution<double> distribution(0.0, double(w));

	this->t = distribution(generator);
}

Hyperplane_EUC::~Hyperplane_EUC(){}	

Hyperplane_COS::Hyperplane_COS(int dim) : Hyperplane(dim){}

Hyperplane_COS::~Hyperplane_COS(){}

/*============================= Rest of the functions*/
double * Hyperplane::getVector()
{
	return this->v;
}

int Hyperplane_EUC::getW()
{
	return this->w;
}

double Hyperplane_EUC::getT()
{
	return this->t;
}

void Hyperplane_EUC::printHyperplane(int dim)
{
	cout << "-t: " << this->t << endl;
	cout << "-w: " << this->w << endl;
	cout << "-vector: " << endl;

	for(int i=0; i<dim; i++)
		cout << this->v[i] << " " ;

	cout << endl << "---------" << endl << endl;
}

int Hyperplane_EUC::computeH(vector<double> p)
{
	int sum=0;
	long double dot_product=0;

	for(unsigned int i=0; i<p.size(); i++)
		dot_product += v[i]*p[i];

	/*== handle overflow by restarting*/
	if( dot_product > INT_MAX || dot_product < INT_MIN)
	{
		cout << "Overflow detected(dot product), exiting program..." << endl;
		exit(EXIT_FAILURE);
	}
 
	return sum = floor( (double) ((double)dot_product + this->t)/this->w );
}

int Hyperplane_COS::computeH(vector<double> p)
{
	int dot_product = 0;

	for(unsigned int i=0; i<p.size(); i++)
		dot_product += v[i]*p[i];

	if(dot_product > 0)
		return 1;
	
	return 0;
}

long long int Hyperplane_EUC::memory_used(int dim)
{
	long long int memory=0;

	memory += sizeof(this->w);
	memory += sizeof(this->t);
	memory += dim * sizeof(*this->v);

	return memory;
}

long long int Hyperplane_COS::memory_used(int dim)
{
	long long int memory=0;

	return memory += dim * sizeof(*this->v);
}
