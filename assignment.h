#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include <vector>

#include "hash_table.h"
#include "hyper_cube.h"

std::vector<int> loyds(std::vector<std::vector<double>> data, std::vector<std::vector<double>> centroids, int data_size, Metric<double>* metric_ptr);

std::vector<int> lsh(HashTable<std::vector<double>> ** hash_tableptr, std::vector<std::vector<double>> data, std::vector<std::vector<double>> centroids, int data_size, int L, Metric<double>* metric_ptr);

std::vector<int> hypercube(HyperCube<std::vector<double>> * hyper_cubeptr, std::vector<std::vector<double>> data, std::vector<std::vector<double>> centroids, int probes, int M, int data_size, Metric<double>* metric_ptr);

#endif
