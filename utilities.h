#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <vector>

#include "hash_table.h"
#include "hyper_cube.h"

void rerunCheck(int argc, int args);

void getInlineArguments(int argc, char** argv, std::string &metric, short int &inputFileIndex, short int &configFileIndex, short int &outputFileIndex);
void getConfigurationParameters(char** argv, int &clusters, int &L, int &h, int &k, int &M, int &probes, short int configFileIndex);
int  getInputLines(char** argv, short int inputFileIndex);
std::vector<std::vector<double>> getInputData(char** argv, short int inputFileIndex);

HashTable<std::vector<double>> ** createHashTable(char** argv, int inputFileIndex, int L, int k, std::string type);
HyperCube<std::vector<double>> * createHyperCube(char ** argv, int inputFileIndex, int k, std::string type);

void fillHashTable(HashTable<std::vector<double>> ** hash_tableptr, char ** argv, int inputFileIndex, int L);
void fillHyperCube(HyperCube<std::vector<double>> * hyper_cubeptr, char ** argv, int inputFileIndex);

double euclideanDistance2(std::vector<double> x, std::vector<double> y);
int binarySearch(std::vector<double> arr, int a, int z, double x);

#endif
