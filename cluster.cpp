#include <iostream>
#include <vector>
#include <getopt.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#include "utilities.h"
#include "hash_table.h"

#define NECESSARY_ARGUMENTS   9
#define INITIALIZATION_POINTS 20

using namespace std;

int main(int argc, char ** argv)
{
	string metric;
	short int inputFileIndex;
	short int configFileIndex;
	short int outputFileIndex;
	
	int clusters;
	int L = 5;
	int h = 4;

	srand(time(NULL));

	/*== check number of args given*/
	rerunCheck(argc, NECESSARY_ARGUMENTS);

	/*== get all input arguments through getopt()*/
	getInlineArguments(argc, argv, metric, inputFileIndex, configFileIndex, outputFileIndex);

	/*== get data size*/
	int data_size = getInputLines(argv, inputFileIndex);

	/*== get config elements*/
	getConfigurationParameters(argv, clusters, L, h, configFileIndex);

	/*== get input data*/
	vector<vector<double>> data = getInputData(argv, inputFileIndex);

	/*== simple random selection of k points*/
	vector<vector<double>> centroids = randomSelection(data, data_size, clusters);
		
	/*== k-means++ initialisation*/
	centroids = k_meanspp(data, data_size, clusters, INITIALIZATION_POINTS); 

	/*== loyd's assignment*/
	vector<int> labels = loyds(data, centroids, data_size);

	/*== create lsh table*/
	HashTable<std::vector<double>>** hash_tableptr = createHashTable(argv, inputFileIndex, L, h, metric);

	/*== fill hash_table*/
	fillHashTable(hash_tableptr, argv, inputFileIndex, L);

	/*== start lsh process*/
	lsh(hash_tableptr, data, centroids, data_size, L);
	
	exit(EXIT_SUCCESS);
}
