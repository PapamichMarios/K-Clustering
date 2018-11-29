#ifndef __INITIALIZATION_H__
#define __INITIALIZATION_H__

#include <vector>

std::vector<std::vector<double>> randomSelection(std::vector<std::vector<double>> data, int data_size, int k);

std::vector<std::vector<double>> k_meanspp(std::vector<std::vector<double>> data, int data_size, int clusters, int k, Metric<double> * metric_ptr);

#endif
