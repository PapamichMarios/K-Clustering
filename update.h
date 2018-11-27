#ifndef __UPDATE_H__
#define __UPDATE_H__

#include <vector>

#include "hash_table.h"
#include "hyper_cube.h"

std::vector<std::vector<double>> 	k_means(std::vector<std::vector<double>> data, std::vector<int> labels, std::vector<std::vector<double>> centroids);

std::vector<std::vector<double>> 	PAM_a_la_loyds(std::vector<std::vector<double>> data, std::vector<int> labels, std::vector<std::vector<double>> centroids);
#endif
