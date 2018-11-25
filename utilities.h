#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <vector>

void rerunCheck(int argc, int args);
void getInlineArguments(int argc, char** argv, std::string &metric, short int &inputFileIndex, short int &configFileIndex, short int &outputFileIndex);
void getConfigurationParameters(char** argv, int &clusters, int &L, int &h, short int configFileIndex);
int getInputLines(char** argv, short int inputFileIndex);
std::vector<std::vector<double>> getInputData(char** argv, short int inputFileIndex);
double euclidean_distance(std::vector<double> x, std::vector<double> y);
int binarySearch(std::vector<double> arr, int a, int z, int x);

/*===================================== INITIALIZATION FUNCTIONS */
std::vector<std::vector<double>> randomSelection(std::vector<std::vector<double>> data, int data_size, int k);
std::vector<std::vector<double>> k_meanspp(std::vector<std::vector<double>> data, int data_size, int clusters, int k);

/*===================================== ASSIGNMENT FUNCTIONS */

/*===================================== UPDATE FUNCTIONS */
#endif
