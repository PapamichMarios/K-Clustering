#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <climits>
#include <random>

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

void getConfigurationParameters(char** argv, int &clusters, int &L, int &h, short int configFileIndex)
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
				continue;
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

	for(unsigned int i=0; i<x.size(); i++)
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
/*===================================== INITIALIZATION FUNCTIONS */
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
		p_array.resize(k-i+1);
		p_array_index.resize(k-i+1);

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
		p_array.clear();
		p_array_index.clear();
		p_array_counter=1;
		sum = 0.0;
		max_distance = INT_MIN;

	}

	return centroids;
}
/*===================================== ASSIGNMENT FUNCTIONS */
vector<int> loyds(vector<vector<double>> data, vector<vector<double>> centroids, int data_size)
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
			temp_distance = euclideanDistance2(data[i], centroids[j]);

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

void lsh()
{
	
}

void hypercube()
{
	
}

/*===================================== UPDATE FUNCTIONS */
