#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <climits>
#include <random>
#include <cmath>

#include "utilities.h"

using namespace std;

void rerunCheck(int argc, int args)
{
	if( argc != args )
	{
		cout << "Rerun: ./cluster -i <input_file> -o <output_file> -c <configuration_file> -d <metric>" << endl;
		exit(EXIT_FAILURE);
	}
}

void getInlineArguments(int argc, char** argv, std::string &metric, short int &inputFileIndex, short int &configFileIndex, short int &outputFileIndex)
{
	int opt;

    while ((opt = getopt(argc, argv, "c:i:d:o:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                inputFileIndex = optind-1;
                break;
            case 'c':
                configFileIndex = optind-1;
                break;
            case 'd':
				metric = optarg;
                break;
            case 'o':
                outputFileIndex = optind-1;
                break;
            case '?':
                cout << "Invalid argument" << endl;
                exit(EXIT_FAILURE);
        }
    }
}

void getConfigurationParameters(char** argv, int &clusters, int &L, int &h, int &k, int &M, int &probes, short int configFileIndex)
{
	ifstream configfile;
	string line;

	configfile.open(argv[configFileIndex]);
	if(!configfile.is_open())
	{
		cout << "Could not open configuration file" << endl;
		exit(EXIT_FAILURE);
	}
	
	/*== clusters*/
	getline(configfile, line);
    istringstream clusterStream(line);
    clusterStream >> line >> clusters;
	
	/*== hash functions h*/
	getline(configfile, line);
    std::istringstream hStream(line);
    hStream >> line >> h;

	/*== hash tables L*/
	getline(configfile, line);
    std::istringstream LStream(line);
    LStream >> line >> L;

	/*== hyper cube k(d' dimensions)*/
	getline(configfile, line);
	std::istringstream kStream(line);
	kStream >> line >> k;

	/*== hyper cube M edges to search*/
	getline(configfile, line);
	std::istringstream MStream(line);
	MStream >> line >> M;

	/*== hyper cube probes points to search*/
	getline(configfile, line);
	std::istringstream probesStream(line);
	probesStream >> line >> probes;

	configfile.close();
}

HashTable<vector<double>> ** createHashTable(char** argv, int inputFileIndex, int L, int k, std::string type)
{
	HashTable<vector<double>> ** hash_tableptr;

	/*== open input.dat*/
	ifstream infile;

	infile.open(argv[inputFileIndex]);
	if(!infile.is_open())
	{
		cout << "Could not open input data file" << endl;
		exit(EXIT_FAILURE);
	}

	/*== find table size*/
	int tableSize = help_functions::calculate_tableSize(infile, type, k);

	/*== find dimensions*/
	int dimensions = help_functions::calculate_dimensions(infile);

	/*== create the table*/
	if(type == "cosine")
	{
		hash_tableptr = new HashTable<vector<double>>*[L];

		for(int i=0; i<L; i++)
			hash_tableptr[i] = new HashTable_COS<vector<double>>(tableSize, k, dimensions);
	}
	else if(type == "euclidean")
	{
		hash_tableptr = new HashTable<vector<double>>*[L];

		for(int i=0; i<L; i++)
			hash_tableptr[i] = new HashTable_EUC<vector <double>>(tableSize, k, dimensions);
	}

	infile.close();

	return hash_tableptr;
}

HyperCube<vector<double>> * createHyperCube(char ** argv, int inputFileIndex, int k, std::string type)
{
	HyperCube<vector<double>> * hyper_cubeptr;

	/*== open input.dat*/
	ifstream infile;

	infile.open(argv[inputFileIndex]);
	if(!infile.is_open())
	{
		cout << "Could not open input data file" << endl;
		exit(EXIT_FAILURE);
	}

	/*== find table size*/
	int tableSize = pow(2, k);

	/*== find dimensions*/
	int dimensions = help_functions::calculate_dimensions(infile);

	if(type == "cosine")
	{
		hyper_cubeptr = new HyperCube_COS<vector<double>>(tableSize, k, dimensions);
	}
	else if(type == "euclidean")
	{
		hyper_cubeptr = new HyperCube_EUC<vector<double>>(tableSize, k, dimensions);
	}

	infile.close();

	return hyper_cubeptr;
}

void fillHashTable(HashTable<vector<double>> ** hash_tableptr, char ** argv, int inputFileIndex, int L)
{
	ifstream infile;

	infile.open(argv[inputFileIndex]);
	if(!infile.is_open())
	{
		cout << "Could not open input data file" << endl;
		exit(EXIT_FAILURE);
	}

	string line;
	string identifier, coord;
	vector<double> point;

	while(getline(infile, line))
	{
		istringstream iss(line);
		getline(iss, identifier, ',');
		while(getline(iss, coord, ','))
		{
			try{
				point.push_back(stod(coord));
			} catch(const exception& e){
				std::cerr << e.what();
				exit(EXIT_FAILURE);
			}
		}

		for(int i=0; i<L; i++)
			hash_tableptr[i]->put(point, identifier);

		point.clear();
	}

	infile.close();
}

void fillHyperCube(HyperCube<vector<double>> * hyper_cubeptr, char ** argv, int inputFileIndex)
{
	ifstream infile;

	infile.open(argv[inputFileIndex]);
	if(!infile.is_open())
	{
		cout << "Could not open input data file" << endl;
		exit(EXIT_FAILURE);
	}

	string line;
	string identifier, coord;
	vector<double> point;

	while(getline(infile, line))
	{
		istringstream iss(line);
		getline(iss, identifier, ',');
		while(getline(iss, coord, ','))
		{
			try{
				point.push_back(stod(coord));
			} catch(const exception& e){
				std::cerr << e.what();
				exit(EXIT_FAILURE);
			}
		}

		hyper_cubeptr->put(point, identifier);

		point.clear();
	}

	infile.close();
}

int getInputLines(char ** argv, short int inputFileIndex)
{
	ifstream infile;
	string line;
	int lines=0;

	infile.open(argv[inputFileIndex]);
	if(!infile.is_open())
	{
		cout << "Could not open input data file" << endl;
		exit(EXIT_FAILURE);
	}

	while(getline(infile, line))
		lines++;

	infile.close();

	return lines;
}

vector<vector<double>> getInputData(char ** argv, short int inputFileIndex)
{
	vector<vector<double>> input(getInputLines(argv, inputFileIndex));
	ifstream infile;
	string line;

	infile.open(argv[inputFileIndex]);
	if(!infile.is_open())
	{
		cout << "Could not open input data file" << endl;
		exit(EXIT_FAILURE);
	}

	/*== assign all points to a vector*/
	string coord;
	int size=0;
	while(getline(infile, line))
	{
		istringstream iss(line);
		while(getline(iss, coord, ','))
		{
			try{
				input.at(size).push_back(stod(coord));
			} catch(const exception& e){
				std::cerr << e.what();
				exit(EXIT_FAILURE);
			}
		}

		size++;
	}

	infile.close();

	return input;
}

double euclideanDistance2(vector<double> x, vector<double> y)
{
	double distance=0;

	for(unsigned int i=1; i<x.size(); i++)
		distance += (x[i]-y[i])*(x[i]-y[i]);

	return distance;
}

int binarySearch(vector<double> arr, int a, int z, double x)
{
	int mid = a + (z - a)/2;

	/*== If the element is present at the middle itself*/
	if(z-a == 1)
		return z;

	/*== continue with left subarray*/
	if (arr[mid] > x)
		return binarySearch(arr, a, mid, x);

	/*== continue with right subarray*/
	return binarySearch(arr, mid, z, x);
}
