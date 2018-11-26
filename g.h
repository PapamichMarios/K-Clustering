#ifndef __G_H__
#define __G_H__

#include "hyperplane.h"

class G
{
	private:
		Hyperplane_COS ** h_array;
		int k;
		int dim;
	
	public:	
		G(int k, int dim);
		~G();

		Hyperplane_COS ** getH_array();
		int hashValue(std::vector<double> p);

		long long int memory_used();
};
#endif
