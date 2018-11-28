#ifndef __HELP_FUNCTIONS_H__
#define __HELP_FUNCTIONS_H__

#include <cstdlib>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <random>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <map>

namespace help_functions
{
	inline void print_RS(std::map<std::string, double> dist_map, std::ofstream &outputfile)
	{
		for(std::map<std::string, double>::const_iterator it = dist_map.begin();it != dist_map.end(); ++it)
			//outputfile << it->first << " " << it->second << std::endl;
			outputfile << it->first << std::endl;
	}

	inline void print_NN_ANN_LSH(std::ofstream &outputfile, std::vector<std::vector<std::string>> hash_table_measurements, std::vector<std::string> measurements)
	{
		std::vector<double> distance_list;

		for(unsigned int i=0; i<hash_table_measurements.size(); i++)
			distance_list.push_back(stod(hash_table_measurements.at(i).at(0)));
			
		auto min = std::min_element(distance_list.begin(), distance_list.end());
		int min_index = std::distance(distance_list.begin(), min);

		outputfile << "Nearest neighbor: " << hash_table_measurements.at(min_index).at(1) << std::endl;
		outputfile << "distanceLSH: " << hash_table_measurements.at(min_index).at(0) << std::endl;
		outputfile << "distanceTrue: " << measurements.at(0) << std::endl;
		outputfile << "tLSH: " << hash_table_measurements.at(min_index).at(2) << std::endl;
		outputfile << "tTrue: " << measurements.at(1) << std::endl;
	}

	inline void print_NN_ANN_CUBE(std::ofstream &outputfile, double distance_ANN, double time_ANN, double distance_NN, double time_NN, std::string identifier_ANN)
	{
		outputfile << "Nearest neighbor: " << identifier_ANN << std::endl;
		outputfile << "distanceLSH: " << distance_ANN << std::endl;
		outputfile << "distanceTrue: " << distance_NN << std::endl;
		outputfile << "tLSH: " << time_ANN << std::endl;
		outputfile << "tTrue: " << time_NN << std::endl;
	}

	inline double * normal_distribution_vector(int dim)
	{
	 	double* vector = (double*)std::calloc(dim+1, sizeof(double));

		std::random_device rd;
		std::default_random_engine generator(rd());
		std::normal_distribution<double> distribution(0.0, 1.0);

		for(int i=0; i<dim; i++)
			vector[i] = distribution(generator);

		return vector;
	}

	template <typename K>
	inline double euclidean_distance(const K &vector1, const K &vector2)
	{
		double distance = 0;

		for(unsigned int i=0; i<vector1.size(); i++)
			distance += pow(vector1[i] - vector2[i], 2);

		return sqrt(distance);
	}

	template <typename K>
	inline double cosine_distance(const K &vector1, const K &vector2)
	{
		double distance = 0;

		double dot_product = 0;
		
		double length1=0, length2=0;
		double length_product = 0;

		for(unsigned int i=0; i<vector1.size(); i++)
		{
			dot_product += vector1[i]*vector2[i];
			length1 += pow(vector1[i], 2);
			length2 += pow(vector2[i], 2);
		}

		length_product = sqrt(length1*length2);

		/*== dist(x,y) = 1 - cos(x,y) 
			 cos(x,y) = x*y/|x|*|y|
		  == */
		return distance = 1 - dot_product/length_product;
	}

	inline int hamming_distance(int x, int y)
	{
  		int dist = 0;
		char val = x^y;

  		while(val)   
  		{
    		++dist; 
    		val &= val - 1; 
  		}

 		return dist;
	}

	inline int calculate_dimensions(std::ifstream& infile)
	{
		int dimensions=0;
		std::string line, coord;

		getline(infile, line);
		getline(infile, line);
		std::istringstream iss_d(line);
		while(getline(iss_d, coord, ','))
			dimensions++;

		dimensions -= 1;

		infile.clear();
		infile.seekg(0, std::ios::beg);

		return dimensions;
	}

	inline int count_lines_query(std::ifstream& infile, std::string type)
	{
		int lines_counted = 0;
		std::string line;

		getline(infile, line);
		while(getline(infile, line))	
			++lines_counted;

		infile.clear();
		infile.seekg(0, std::ios::beg);

		return lines_counted;
	}

	inline int calculate_tableSize(std::ifstream& infile, std::string type, int k)
	{
		std::string line;
		int table_size;

		if(type == "euclidean")
		{
			int lines_counter = 0;

			while(getline(infile, line))	
				++lines_counter;

			
			table_size = lines_counter/16;

			infile.clear();
			infile.seekg(0, std::ios::beg);
		}
		else
			table_size = pow(2, k);

		return table_size;
	}

	inline std::string find_type(std::ifstream& infile)
	{
		std::string line, type;

		getline(infile, line);
		if( line.compare(0, 6, "cosine") == 0 )
			type = "COS";
		else
			type = "EUC";

		infile.clear();
		infile.seekg(0, std::ios::beg);

		return type;
	}

}
#endif
