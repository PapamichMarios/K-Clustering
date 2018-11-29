#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <climits>
#include <random>
#include <cmath>

#include "utilities.h"
#include "assignment.h"

#define MAX_TIMES_RADIUS 10

using namespace std;


vector<int> loyds(vector<vector<double>> data, vector<vector<double>> centroids, int data_size, Metric<double>* metric_ptr)
{
	vector<int> cluster_assigned(data_size);
	double min_distance = INT_MAX;
	int cluster;
	double temp_distance;
	
	/*== calculate the distance of all the points with the centroids
		 assign them to the closest centroid
	  == */
	for(unsigned int i=0; i<data.size(); i++)
	{
		for(unsigned int j=0; j<centroids.size(); j++)
		{
			temp_distance = metric_ptr->distance2(data[i], centroids[j]);

			if(temp_distance < min_distance)
			{
				min_distance = temp_distance;
				cluster = j;
			}
		}

		/*== assign point to cluster*/
		cluster_assigned[i] = cluster;

		/*== reset min_distance*/
		min_distance = INT_MAX;
	}

	return cluster_assigned;
}

vector<int> lsh(HashTable<vector<double>> ** hash_tableptr, vector<vector<double>> data, vector<vector<double>> centroids, int data_size, int L, Metric<double>* metric_ptr)
{
	/*== set unassigned points to -1*/
	vector<int> labels(data_size);
	for(int i=0; i<data_size; i++)
		labels[i] = -1;

	vector<vector<int>> hash_centroids_index(centroids.size());
	
	/*== assign centroids to another vector because they already contain the id*/
	vector<vector<double>> temp_centroids(centroids.size());
	for(unsigned int ii=0; ii<centroids.size(); ii++)
	{
		for(unsigned int jj=1; jj<centroids[ii].size(); jj++)
		{
			temp_centroids.at(ii).push_back(centroids.at(ii).at(jj));
		}
	}

	/*== find where centroids hash*/
	for(int i=0; i<L; i++)
	{
		for(unsigned int j=0; j<centroids.size(); j++)
		{
			hash_centroids_index.at(i).push_back( hash_tableptr[i]->hash(temp_centroids[j]) );
		}
	}

	/*== calculate radius*/
	double min_distance = INT_MAX;
	for(unsigned int j=0; j<centroids.size(); j++)
	{
		for(unsigned int z=j+1; z<centroids.size(); z++)
		{
			double temp_distance = metric_ptr->distance2(centroids[j], centroids[z]);
			temp_distance = sqrt(temp_distance);

			if(temp_distance < min_distance)
				min_distance = temp_distance;
		}
	}

	double radius = min_distance/2;

	for(int i=0; i<MAX_TIMES_RADIUS; i++)
	{
		for(int ii=0; ii<L; ii++)
		{
			/*== get points that their distance is lesser than the radius*/
			for(unsigned int j=0; j<centroids.size(); j++)
			{
				vector<int> conflicts;

				/*== find if two centroids hash to the same bucket*/
				for(unsigned int z=j+1; z<centroids.size(); z++)
				{
					if(hash_centroids_index.at(ii).at(j) == hash_centroids_index.at(ii).at(z))
					{
						conflicts.push_back(z);
					}
				}

				/*== find neighbours through Range Search*/
				if(conflicts.size()==0)
				{
					hash_tableptr[ii]->RS(temp_centroids[j], 1, radius, labels, j);	
				}
				else
				{
					conflicts.push_back(j);

					hash_tableptr[ii]->RS_conflict(temp_centroids, 1, radius, labels, conflicts);
				}
			}
		}

		/*== double the radius*/
		radius *= 2;
	}

	/*== assign the rest of the unassigned points to a cluster through loyds*/
	for(int i=0; i<data_size; i++)
	{
		/*== if the point has not been assigned, assign it with loyds*/
		if(labels[i] == -1)
		{
			double distance;
			double min_distance = INT_MAX;
			int cluster;

			for(unsigned int j=0; j<temp_centroids.size(); j++)
			{
				distance = metric_ptr->distance2(data[i], centroids[j]);

				if(distance < min_distance)
				{
					min_distance = distance;
					cluster = j;
				}
			}

			/*== update labels*/
			labels[i] = cluster;
		}
	}

	/*== reset assigned points*/
	for(int i=0; i<L; i++)
		hash_tableptr[i]->reset_assigned();

	return labels;
}

vector<int> hypercube(HyperCube<vector<double>> * hyper_cubeptr, vector<vector<double>> data, vector<vector<double>> centroids, int probes, int M, int data_size, Metric<double>* metric_ptr)
{
	/*== set unassigned points to -1*/
	vector<int> labels(data_size);
	for(int i=0; i<data_size; i++)
		labels[i] = -1;

	vector<int> hash_centroids_index(centroids.size());
	
	/*== assign centroids to another vector because they already contain the id*/
	vector<vector<double>> temp_centroids(centroids.size());
	for(unsigned int ii=0; ii<centroids.size(); ii++)
	{
		for(unsigned int jj=1; jj<centroids[ii].size(); jj++)
		{
			temp_centroids.at(ii).push_back(centroids.at(ii).at(jj));
		}
	}

	/*== find where centroids hash*/
	for(unsigned int j=0; j<centroids.size(); j++)
		hash_centroids_index[j] = hyper_cubeptr->hash(temp_centroids[j]);

	/*== calculate radius*/
	double min_distance = INT_MAX;
	for(unsigned int j=0; j<centroids.size(); j++)
	{
		for(unsigned int z=j+1; z<centroids.size(); z++)
		{
			double temp_distance = metric_ptr->distance2(centroids[j], centroids[z]);
			temp_distance = sqrt(temp_distance);

			if(temp_distance < min_distance)
				min_distance = temp_distance;
		}
	}

	double radius = min_distance/2;

	for(int i=0; i<MAX_TIMES_RADIUS; i++)
	{
		/*== get points that their distance is lesser than the radius*/
		for(unsigned int j=0; j<centroids.size(); j++)
		{
			vector<int> conflicts;

			/*== find if two centroids hash to the same bucket*/
			for(unsigned int z=j+1; z<centroids.size(); z++)
			{
				if(hash_centroids_index[j] == hash_centroids_index[z])
				{
					conflicts.push_back(z);
				}
			}

			/*== find neighbours through Range Search*/
			if(conflicts.size()==0)
			{
				hyper_cubeptr->RS(temp_centroids[j], 1, radius, probes, M, labels, j);	
			}
			else
			{
				conflicts.push_back(j);

				hyper_cubeptr->RS_conflict(temp_centroids, 1, radius, probes, M, labels, conflicts);
			}
		}

		/*== double the radius*/
		radius *= 2;
	}

	/*== assign the rest of the unassigned points to a cluster through loyds*/
	for(int i=0; i<data_size; i++)
	{
		/*== if the point has not been assigned, assign it with loyds*/
		if(labels[i] == -1)
		{
			double distance;
			double min_distance = INT_MAX;
			int cluster;

			for(unsigned int j=0; j<temp_centroids.size(); j++)
			{
				distance = metric_ptr->distance2(data[i], centroids[j]);

				if(distance < min_distance)
				{
					min_distance = distance;
					cluster = j;
				}
			}

			/*== update labels*/
			labels[i] = cluster;
		}
	}

	/*== reset assigned points*/
	hyper_cubeptr->reset_assigned();

	return labels;
}
