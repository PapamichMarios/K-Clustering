#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <climits>
#include <random>
#include <cmath>

#include "utilities.h"
#include "initialization.h"
#include "assignment.h"
#include "update.h"

#define MAX_PROCESS_LOOPS 20

using namespace std;

void rerunCheck(int argc, int args)
{
	if( argc > args )
	{
		cout << "Rerun: ./cluster -i <input_file> -o <output_file> -c <configuration_file> -d <metric>" << endl;
		exit(EXIT_FAILURE);
	}
}

void getInlineArguments(int argc, char** argv, std::string &metric, short int &inputFileIndex, short int &configFileIndex, short int &outputFileIndex, int &completeFlag)
{
	int opt;

	/*== struct for getopt long options*/
	static struct option long_options[] = 
	{
		{"c"	 	, required_argument, NULL, 'c'},
		{"i"	 	, required_argument, NULL, 'i'},
		{"d"	 	, required_argument, NULL, 'd'},
		{"o"	 	, required_argument, NULL, 'o'},
		{"complete" , no_argument, NULL, 'p'}
	};

	while ((opt = getopt_long_only(argc, argv, "", long_options, NULL)) != -1)
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
			case 'p':
				completeFlag = 1;
				break;
            case '?':
                cout << "Invalid argument" << endl;
                exit(EXIT_FAILURE);
        }
    }
}

void getConfigurationParameters(char** argv, int &clusters, int &initializationpp_points, int &L, int &h, int &k, int &M, int &probes, short int configFileIndex)
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

	/*== initialization++ points*/	
	getline(configfile, line);
    istringstream initpppointsStream(line);
    initpppointsStream >> line >> initializationpp_points;

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

Metric<double>* getMetric(std::string type)
{
	Metric<double> * metric_ptr;

	if(type == "cosine")
		metric_ptr = new Metric_COS<double>();
	else if(type == "euclidean")
		metric_ptr = new Metric_EUC<double>();

	return metric_ptr;
}

void clustering(vector<vector<double>> data, int data_size, int clusters, int initializationpp_points, Metric<double>*  metric_ptr, int L, int M, int probes, HashTable<vector<double>>** hash_tableptr, HyperCube<vector<double>>* hyper_cubeptr, int i, int j, int z, vector<vector<double>> &centroids, vector<int> &labels)
{
	/*============== INITIALIZATION */
	switch(i)
	{
		case 0: 
			/*== simple random selection of k points*/
			centroids = randomSelection(data, data_size, clusters);
			break;

		case 1:
			/*== k-means++ initialisation*/
			centroids = k_meanspp(data, data_size, clusters, initializationpp_points, metric_ptr); 
			break;
	}
		

	long double objective_function=0;
	long double last_objective_function =0;
	int loops=0;
	do
	{
		last_objective_function = objective_function;

		/*============== ASSIGNMENT */
		switch(j)
		{
			case 0:
				/*== loyd's assignment*/
				labels = loyds(data, centroids, data_size, metric_ptr);
				break;

			case 1:
				/*== start lsh assignment by range search process*/
				labels = lsh(hash_tableptr, data, centroids, data_size, L, metric_ptr);
				break;

			case 2:
				/*== start hyper assignment by range search process*/
				labels = hypercube(hyper_cubeptr, data, centroids, probes, M, data_size, metric_ptr);
				break;
		}

		/*============== UPDATE */
		switch(z)
		{
			case 0:
				/*== k-means update*/
				centroids = k_means(data, labels, centroids, objective_function, metric_ptr); 
				break;

			case 1:
				/*== PAM update*/
				centroids = PAM_a_la_loyds(data, labels, centroids, objective_function, metric_ptr);
				break;
		}

		cout << objective_function - last_objective_function << endl;
		loops++;
	} while( abs(objective_function - last_objective_function) > (double)5/100 && loops < MAX_PROCESS_LOOPS);

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

int changeClusteringCombination(int &i, int &j, int &z, int ii, int jj, int zz)
{
	if( z == zz -1 )
	{
		z=0;
		if( j == jj -1)
		{
			j=0;
			if(i == ii -1)
				return 1;
			else
				i++;
		}
		else
			j++;
	}
	else
		z++;

	return 0;
}

void resetOutput(char **argv, short int outputFileIndex)
{
	
	/*== open outputfile for writing*/
	ofstream outfile;

	outfile.open(argv[outputFileIndex], ofstream::out | ofstream::trunc);
	if(!outfile.is_open())
	{
		cout << "Could not open output file" << endl;
		exit(EXIT_FAILURE);
	}

	outfile.close();
}

void printOutput(char **argv, short int outputFileIndex, vector<int> labels, vector<vector<double>> centroids, vector<long double> silhouette_array, int i, int j, int z, string metric, double time_lasted, int completeFlag, vector<vector<double>> data)
{
	vector<int> cluster_size(centroids.size());

	/*== count the final clusters size*/
	for(unsigned int i_=0; i_<labels.size(); i_++)
		cluster_size[labels[i_]]++;

	/*== open outputfile for writing*/
	ofstream outfile;

	outfile.open(argv[outputFileIndex], ofstream::app);
	if(!outfile.is_open())
	{
		cout << "Could not open output file" << endl;
		exit(EXIT_FAILURE);
	}

	outfile <<  "Algorithm: ";
	switch(i)
	{
		case 0: 
			cout << "RandomSelection";
			outfile << "I1" ;
			break;

		case 1:
			cout << "K-means++";
			outfile << "I2";
			break;
	}

	cout << "x";
	switch(j)
	{
		case 0:
			cout << "Loyds";
			outfile << "A1";
			break;

		case 1:
			cout << "LSH";
			outfile << "A2";
			break;

		case 2:
			cout << "Hypercube";
			outfile << "A3";
			break;
	}

	cout << " x ";
	switch(z)
	{
		case 0:
			cout << "K-means";
			outfile << "U1";
			break;

		case 1:
			cout << "PAM(loyds)";
			outfile << "U2" ;
			break;
	}

	cout << endl;
	outfile << endl << "Metric: " << metric << endl;

	for(unsigned int i_=0; i_<centroids.size(); i_++)
	{	
		outfile << "CLUSTER-" << i_+1 << " {size: " << cluster_size[i_] << ", centroid: ";

		/*== k-means centroid*/
		if(centroids[i_][0] == -1)
		{
			outfile << "[";
			for(unsigned int j_=1; j_<centroids[i_].size()-1; j_++)
				outfile << centroids[i_][j_] << ", " ;

			outfile << centroids[i_][centroids[i_].size()-1];
			outfile << "]";
		}
		else
		{
			outfile << centroids[i_][0];
		}

		outfile << "}" << endl;
	}

	outfile << "clustering_time: " << time_lasted << " seconds" << endl;

	outfile << "Silhouette: [" ;
	long double stotal=0;
	for(unsigned int i_=0; i_<silhouette_array.size(); i_++)
	{
		outfile << silhouette_array[i_] << ", ";
		stotal += silhouette_array[i_];
	}

	outfile << stotal/silhouette_array.size() << "]" << endl;

	/*== if complete inline arg was given*/
	if(completeFlag)
	{
		for(unsigned int i_=0; i_<centroids.size(); i_++)
		{
			outfile << "CLUSTER-" << i_+1 << " {";

			for(unsigned int j_=0; j_<labels.size()-1; j_++)
			{
				if(labels[j_] == i_)
					outfile << data[j_][0] << ", ";

			}

			outfile << "}" << endl;
		}
	}
	
	outfile << endl;
	outfile.close();
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
