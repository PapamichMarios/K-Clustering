#include <gtest/gtest.h>

#include "../utilities.h"
#include "../initialization.h"
#include "../assignment.h"
#include "../update.h"

using namespace std;

TEST(ClusteringTest, ValuesReturned)
{
	int data_size = 5000;
	vector<vector<double>> input(data_size);

	ifstream infile;
	string line;

	infile.open("../datasets/twitter_dataset_small_v2.csv");
	if(!infile.is_open())
	{
		cout << "Could not open input data file"<< endl;
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

	/*== initialization*/
	int clusters=5;
	vector<vector<double>> centroids = randomSelection(input, data_size, clusters);
	ASSERT_EQ(centroids.size(), clusters); 

	Metric<double> * metric_ptr = getMetric("euclidean");
	int init_points = 20;
	centroids = k_meanspp(input, data_size, clusters, init_points, metric_ptr); 
	ASSERT_EQ(centroids.size(), clusters);

	/*== assignment*/
	vector<int> labels = loyds(input, centroids, data_size, metric_ptr);
	ASSERT_EQ(labels.size(), data_size);

	/*== update*/
	long double obj_func;
	centroids = k_means(input, labels, centroids, obj_func, metric_ptr);
	ASSERT_EQ(centroids.size(), clusters);

	centroids = PAM_a_la_loyds(input, labels, centroids, obj_func, metric_ptr);
	ASSERT_EQ(centroids.size(), clusters);

	infile.close();
}
