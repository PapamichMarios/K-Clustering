#include <iostream>
#include <vector>
#include <getopt.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "utilities.h"
#include "update.h"
#include "metric.h"

#define NECESSARY_ARGUMENTS   		10

#define INITIALIZATION_FUNCTIONS 	2
#define ASSIGNMENT_FUNCTIONS 		3
#define UPDATE_FUNCTIONS 			2

using namespace std;

int main(int argc, char ** argv)
{
	short int inputFileIndex;
	short int configFileIndex;
	short int outputFileIndex;
	int completeFlag=0;

	string metric;
	Metric<double>* metric_ptr;
	
	int clusters;
	int initializationpp_points;
	int L = 5;
	int h = 4;

	int k = 3;
	int M = 10;
	int probes = 2;

	srand(time(NULL));

	/*== check number of args given*/
	rerunCheck(argc, NECESSARY_ARGUMENTS);

	/*== get all inline arguments through getopt()*/
	getInlineArguments(argc, argv, metric, inputFileIndex, configFileIndex, outputFileIndex, completeFlag);

	/*== get data size*/
	int data_size = getInputLines(argv, inputFileIndex);

	/*== get configuration*/
	getConfigurationParameters(argv, clusters, initializationpp_points, L, h , k, M, probes, configFileIndex);

	/*== get input data*/
	vector<vector<double>> data = getInputData(argv, inputFileIndex);

	/*== create metric object*/
	metric_ptr = getMetric(metric);

	/*== create & fill lsh table*/
	HashTable<std::vector<double>>** hash_tableptr = createHashTable(argv, inputFileIndex, L, h, metric);
	fillHashTable(hash_tableptr, argv, inputFileIndex, L);

	/*== create && fill hypercube*/
	HyperCube<std::vector<double>> * hyper_cubeptr = createHyperCube(argv, inputFileIndex, k, metric);
	fillHyperCube(hyper_cubeptr, argv, inputFileIndex);

	/*== reset output file in case there are data inside*/
	resetOutput(argv, outputFileIndex);

	int i=0;
	int j=0;
	int z=0;
	int termination=0;

	/*== choose different combination of functions for each loop*/
	vector<vector<double>> init_centroids;
	while(termination != 1)
	{
		vector<vector<double>> centroids;
		vector<int> labels;
		vector<long double> silhouette_array;

		/*== initialization*/
		if(init_centroids.size() == 0)
			init_centroids = assignment_clustering(data, data_size, clusters, initializationpp_points, metric_ptr, i);

		clock_t begin_time = clock();
		
		/*== assignment && update*/
		centroids = init_centroids;
		clustering(data, data_size, metric_ptr, L, M, probes, hash_tableptr, hyper_cubeptr, i, j, z, centroids, labels);

		double time_lasted = double(clock() - begin_time)/CLOCKS_PER_SEC;

		/*== calculate silhouette*/
		silhouette_array = Silhouette(data, centroids, labels, metric_ptr);

		/*== print results*/
		printOutput(argv, outputFileIndex, labels, centroids, silhouette_array, i, j, z, metric, time_lasted, completeFlag, data);	

		/*== change combination*/
		termination = changeClusteringCombination(i, j, z, INITIALIZATION_FUNCTIONS, ASSIGNMENT_FUNCTIONS, UPDATE_FUNCTIONS, init_centroids);
	}

	/*== unallocate memory*/
	unallocateHashTable(hash_tableptr, L);
	unallocateHyperCube(hyper_cubeptr);
	delete metric_ptr;

	exit(EXIT_SUCCESS);
}
