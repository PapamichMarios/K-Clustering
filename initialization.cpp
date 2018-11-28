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

using namespace std;

vector<vector<double>> randomSelection(vector<vector<double>> data, int data_size, int clusters)
{
	vector<vector<double>> centroids(clusters);
	int index;

	for(int i=0; i<clusters; i++)
	{
		index = rand() % data_size;
		centroids[i] = data[index];
	}

	return centroids;
}

vector<vector<double>> k_meanspp(vector<vector<double>> data, int data_size, int clusters, int k)
{
	vector<vector<double>> centroids(clusters);
	int centroids_assigned=0;

	vector<vector<double>> points(k);
	vector<int> points_available(k);

	/*== choose k points from dataset*/
	int index;

	for(int i=0; i<k; i++)
	{
		index = rand() % data_size;
		points[i] = data[index];

		points_available[i] = 1;
	}
	
	/*== 1. Choose a centroid uniformly at random*/
	index = rand() % k;
	centroids[0] = points[index];
	centroids_assigned++;
	points_available[index] = 0;

	/*== 2 & 3. Pick the rest of the centroids */
	double max_distance = INT_MIN;

	vector<double> p_array;
	vector<int> p_array_index;
	int p_array_counter=1;

	double sum = 0;
	double random_index;
	int centroid_index;

	for(int i=1; i<clusters; i++)
	{
		vector<double> p_array(k-i+1);
		vector<int> p_array_index(k-i+1);

		/*== make P array*/
		p_array[0] = 0;
		p_array_index[0] = 0;
		p_array_counter = 1;

		/*== find max D(i)*/
		for(int j=0; j<k; j++)
		{
			if(points_available[j])
			{
				double temp_distance;
				double min_distance = INT_MAX;
				for(int ii=0; ii<centroids_assigned; ii++)
				{
					temp_distance = euclideanDistance2(points[j], centroids[ii]);
					if(temp_distance < min_distance)
						min_distance = temp_distance;
				}

				/*== calculate max D(i) in order to simplify our array*/
				if(min_distance > max_distance)
					max_distance = min_distance;
			}
		}

		/*== if point is not a centroid,
			 calculate the sum of the distances with previous non centroid points

			 ex. 4 points , point 0 centroid and we are checking point 2
				 sum[2] = distance(1)[from point 0]^2 + distance(2)[from point 0]^2
		  == */
		for(int j=0; j<k; j++)
		{
			if(points_available[j])
			{
				/*== get the minimum distance out of all the centroids*/
				double min_distance = INT_MAX;
				double temp_distance;
				for(int ii=0; ii<centroids_assigned; ii++)
				{
					temp_distance = euclideanDistance2(points[j], centroids[ii]);
					if(temp_distance < min_distance)
						min_distance = temp_distance;
				}

				/*== add distance to sum & divide with max D(i)*/
				sum += min_distance/max_distance;

				/*== assign sum of D(i)s to the p_array*/
				p_array[p_array_counter] = sum;
				p_array_index[p_array_counter] = j;
				p_array_counter++;
			}
		}

		/*== pick a random number (0,P(n-t)]*/
		random_device rd;
		default_random_engine generator(rd());
  		uniform_real_distribution<double> distribution(0.0, p_array[p_array_counter-1]);
		random_index = distribution(generator);

		/*== search the p_array to find the next centroid*/
		centroid_index = p_array_index [binarySearch(p_array, 0, p_array_counter, random_index)];
		centroids[i] = points[centroid_index];
		centroids_assigned++;
		points_available[centroid_index] = 0;

		/*== reset vars to find next centroid*/
		p_array_counter=1;
		sum = 0.0;
		max_distance = INT_MIN;

	}

	return centroids;
}
