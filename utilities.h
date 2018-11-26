#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <vector>

#include "hash_table.h"

void rerunCheck(int argc, int args);
void getInlineArguments(int argc, char** argv, std::string &metric, short int &inputFileIndex, short int &configFileIndex, short int &outputFileIndex);
void getConfigurationParameters(char** argv, int &clusters, int &L, int &h, short int configFileIndex);
HashTable<std::vector<double>> ** createHashTable(char** argv, int inputFileIndex, int L, int k, std::string type);
void fillHashTable(HashTable<std::vector<double>> ** hash_tableptr, char ** argv, int inputFileIndex, int L);
int getInputLines(char** argv, short int inputFileIndex);
std::vector<std::vector<double>> getInputData(char** argv, short int inputFileIndex);
double euclidean_distance(std::vector<double> x, std::vector<double> y);
int binarySearch(std::vector<double> arr, int a, int z, int x);

/*===================================== INITIALIZATION FUNCTIONS */
std::vector<std::vector<double>> randomSelection(std::vector<std::vector<double>> data, int data_size, int k);
std::vector<std::vector<double>> k_meanspp(std::vector<std::vector<double>> data, int data_size, int clusters, int k);

/*===================================== ASSIGNMENT FUNCTIONS */
std::vector<int> loyds(std::vector<std::vector<double>> data, std::vector<std::vector<double>> centroids, int data_size);
std::vector<int> lsh(HashTable<std::vector<double>> ** hash_tableptr, std::vector<std::vector<double>> data, std::vector<std::vector<double>> centroids, int data_size, int L);
void hypercube();

/*===================================== UPDATE FUNCTIONS */
#endif
