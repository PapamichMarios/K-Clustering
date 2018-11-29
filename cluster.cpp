#include <iostream>
#include <vector>
#include <getopt.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "utilities.h"
#include "initialization.h"
#include "assignment.h"
#include "update.h"
#include "hash_table.h"
#include "metric.h"

#define NECESSARY_ARGUMENTS   		10

#define MAX_PROCESS_LOOPS 			25

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

	/*============== CLUSTERING PROCESS */
	vector<vector<double>> centroids;
	vector<long double> silhouette_array;
	vector<int> labels;

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

	while(termination != 1)
	{
		clock_t begin_time = clock();
		
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
			//difference = abs(objective_function - last_objective_function);
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

		double time_lasted = double(clock() - begin_time)/CLOCKS_PER_SEC;

		/*== calculate silhouette*/
		silhouette_array = Silhouette(data, centroids, labels, metric_ptr);

		/*== print results*/
		printOutput(argv, outputFileIndex, labels, centroids, silhouette_array, i, j, z, metric, time_lasted, completeFlag, data);	

		/*== change combination*/
		termination = changeClusteringCombination(i, j, z, INITIALIZATION_FUNCTIONS, ASSIGNMENT_FUNCTIONS, UPDATE_FUNCTIONS);
	}

	exit(EXIT_SUCCESS);
}
