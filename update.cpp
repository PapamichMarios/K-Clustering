#include <vector>
#include <iostream>
#include <climits>

#include "utilities.h"
#include "update.h"

using namespace std;

vector<vector<double>> k_means(vector<vector<double>> data, vector<int> labels, vector<vector<double>> centroids)
{
	vector<vector<double>> new_centroids(centroids.size(), vector<double>(centroids[0].size()));
	vector<int> cluster_size(centroids.size());

	/*== k-means centroids cant have id, so we assign -1 as id*/
	for(unsigned int i=0; i<centroids.size(); i++)
		new_centroids.at(i).at(0) = -1;

	/*== we take the sum of all the points & we count how many points each cluster has*/
	for(unsigned int i=0; i<labels.size(); i++)
	{
		for(unsigned int j=1; j<data[i].size(); j++)
			new_centroids.at(labels[i]).at(j) += data.at(i).at(j);

		cluster_size.at(labels[i])++;
	}

	/*== find the mean of each cluster*/
	for(unsigned int i=0; i<new_centroids.size(); i++)
	{
		for(unsigned int j=1; j<new_centroids[i].size(); j++)
		{
			new_centroids.at(i).at(j) /= cluster_size.at(i);
		}
	}

	return new_centroids;
}

vector<vector<double>> PAM_a_la_loyds(vector<vector<double>> data, vector<int> labels, vector<vector<double>> centroids)
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

			distance += euclideanDistance2(data[i], data[j]);
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

	return medoids;
}
