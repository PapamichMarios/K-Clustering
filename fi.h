#ifndef __FI_H__
#define __FI_H__

#include "hyperplane.h"

class fi
{
	private:
	 	Hyperplane_EUC ** h_array;
		int * r_array;
		int k;
		int dim;

	public:
		fi(int k, int dim);
		~fi();

		Hyperplane_EUC ** getH_array();
		int * getR_array();
		int hashValue(std::vector<double> p, int table_size);
		int cubeValue(std::vector<double> x);

		std::string computeG(std::vector<double> p);
		long long int memory_used();
};
#endif
