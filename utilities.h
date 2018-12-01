#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <vector>

#include "hash_table.h"
#include "hyper_cube.h"
#include "metric.h"

void rerunCheck(int argc, int args);

void getInlineArguments(int argc, char** argv, std::string &metric, short int &inputFileIndex, short int &configFileIndex, short int &outputFileIndex, int &completeFlag);
void getConfigurationParameters(char** argv, int &clusters, int &initializationpp_points, int &L, int &h, int &k, int &M, int &probes, short int configFileIndex);
int  getInputLines(char** argv, short int inputFileIndex);
std::vector<std::vector<double>> getInputData(char** argv, short int inputFileIndex);
Metric<double>* getMetric(std::string type);
void printOutput(char **argv, short int outputFileIndex, std::vector<int> labels, std::vector<std::vector<double>> centroids, std::vector<long double> silhouette_array, int i, int j, int z, std::string metric, double time_lasted, int completeFlag, std::vector<std::vector<double>> data);
void resetOutput(char **argv, short int outputFileIndex);

/*== clustering functions*/
std::vector<std::vector<double>> assignment_clustering(std::vector<std::vector<double>> data, int data_size, int clusters, int initializationpp_points, Metric<double>* metric_ptr, int i);
void clustering(std::vector<std::vector<double>> data, int data_size, Metric<double>*  metric_ptr, int L, int M, int probes, HashTable<std::vector<double>>** hash_tableptr, HyperCube<std::vector<double>>* hyper_cubeptr, int i, int j, int z, std::vector<std::vector<double>> &centroids, std::vector<int> &labels);
int changeClusteringCombination(int &i, int &j, int &z, int ii, int jj, int zz, std::vector<std::vector<double>> &init_centroids);

/*== HashTable & HyperCube utilities*/
HashTable<std::vector<double>> ** createHashTable(char** argv, int inputFileIndex, int L, int k, std::string type);
HyperCube<std::vector<double>> * createHyperCube(char ** argv, int inputFileIndex, int k, std::string type);

void fillHashTable(HashTable<std::vector<double>> ** hash_tableptr, char ** argv, int inputFileIndex, int L);
void fillHyperCube(HyperCube<std::vector<double>> * hyper_cubeptr, char ** argv, int inputFileIndex);

void unallocateHashTable(HashTable<std::vector<double>> ** hash_tableptr, int L);
void unallocateHyperCube(HyperCube<std::vector<double>> * hyper_cubeptr);

int binarySearch(std::vector<double> arr, int a, int z, double x);
#endif
