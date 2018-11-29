#include <vector>
#include <iostream>
#include <climits>

#include "utilities.h"
#include "update.h"

using namespace std;

vector<vector<double>> k_means(vector<vector<double>> data, vector<int> labels, vector<vector<double>> centroids, long double &objective_function, Metric<double>* metric_ptr)
{
	vector<vector<double>> new_centroids(centroids.size(), vector<double>(centroids[0].size()));
	vector<int> cluster_size(centroids.size());

	/*== k-means centroids cant have id, so we assign -1 as id*/
	for(unsigned int i=0; i<centroids.size(); i++)
		new_centroids.at(i).at(0) = -1;

	/*== we count how many points each cluster has*/
	for(unsigned int i=0; i<labels.size(); i++)
		cluster_size.at(labels[i])++;

	/*== find the mean of each cluster*/
	for(unsigned int i=0; i<labels.size(); i++)
	{
		for(unsigned int j=1; j<data[i].size(); j++)
			new_centroids.at(labels[i]).at(j) += data.at(i).at(j)/cluster_size.at(labels[i]);
	}

	/*== calculate objective function*/
	objective_function = 0;

	for(unsigned int i=0; i<labels.size(); i++)
		objective_function += metric_ptr->distance2(data[i], new_centroids[labels[i]]);

	return new_centroids;
}

vector<vector<double>> PAM_a_la_loyds(vector<vector<double>> data, vector<int> labels, vector<vector<double>> centroids, long double &objective_function, Metric<double>* metric_ptr)
{
	vector<vector<double>> medoids(centroids.size(), vector<double>(centroids[0].size()));
	vector<double> min_distances(centroids.size(), INT_MAX);
	vector<int> min_distance_ids(centroids.size());

	/*== calculate the medoid for each cluster*/
	for(unsigned int i=0; i<labels.size(); i++)
	{
		double distance;

		for(unsigned int j=0; j<labels.size(); j++)
		{
			/*== exclude itself && different clusters*/
			if(i == j || labels[i] != labels[j])
				continue;

			distance += metric_ptr->distance2(data[i], data[j]);
		}

		if(distance < min_distances[labels[i]])
		{
			min_distances[labels[i]] = distance;
			min_distance_ids[labels[i]] = data[i][0]-1;
		}
	}

	/*== assign to the medoids vector, the medoids we found before for each cluster*/
	for(unsigned int i=0; i<medoids.size(); i++)
		medoids.at(i) = data.at(min_distance_ids.at(i));

	/*== calculate objective function*/
	objective_function = 0;

	for(unsigned int i=0; i<min_distances.size(); i++)
		objective_function += min_distances[i];

	return medoids;
}

vector<long double> Silhouette(vector<vector<double>> data, vector<vector<double>> centroids, vector<int> labels, Metric<double>* metric_ptr)
{
	vector<long double> silhouette_array(labels.size());
	vector<int> cluster_counter(centroids.size());

	for(unsigned int i=0; i<labels.size(); i++)
		cluster_counter[labels[i]]++;

	for(unsigned int i=0; i<labels.size(); i++)
	{
		/*== find immediate closest centroid than the clusters*/
		double distance;
		double min_distance = INT_MAX;
		int cluster;

		for(unsigned int j=0; j<centroids.size(); j++)
		{
			if( j == labels[i] )
				continue;

			distance = metric_ptr->distance2(data[i], centroids[j]);

			if(distance < min_distance)
			{
				min_distance = distance;
				cluster = j;
			}
		}

		/*== find average of the cluster it belongs*/
		long double average_distance1=0;
		for(unsigned int j=0; j<labels.size(); j++)
		{
			if( j == i || labels[i] != labels[j] )
				continue;

			average_distance1 += metric_ptr->distance2(data[i], data[j])/cluster_counter[labels[i]];
		}

		/*== find average of the closest cluster*/
		long double average_distance2=0;
		for(unsigned int j=0; j<labels.size(); j++)
		{
			if( labels[j] != cluster )
				continue;

			average_distance2 += metric_ptr->distance2(data[i], data[j])/cluster_counter[cluster];
		}

		silhouette_array[i] = (average_distance2 - average_distance1)/fmax(average_distance2, average_distance1);
	}

	return silhouette_array;
}
