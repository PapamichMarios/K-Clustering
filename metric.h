#ifndef __METRIC_H__
#define __METRIC_H__

#include <vector>
#include <cmath>

template <typename K>
class Metric
{
	public:
		virtual double distance2(std::vector<K> x, std::vector<K> y)=0;
};

template <typename K>
class Metric_EUC : public Metric<K>
{
	public:
		double distance2(std::vector<K> x, std::vector<K> y)
		{
			double distance=0;

			for(unsigned int i=1; i<x.size(); i++)
				distance += (x[i]-y[i])*(x[i]-y[i]);

			return distance;
		}
};

template <typename K>
class Metric_COS : public Metric<K>
{
	public:
		double distance2(std::vector<K> x, std::vector<K> y)
		{
			double distance = 0;
			double dot_product = 0;
			
			double length1=0, length2=0;
			double length_product = 0;

			for(unsigned int i=1; i<x.size(); i++)
			{
				dot_product += x[i]*y[i];
				length1 += x[i]*x[i];
				length2 += y[i]*y[i];
			}

			length_product = sqrt(length1*length2);

			/*== dist(x,y) = 1 - cos(x,y) 
				 cos(x,y) = x*y/|x|*|y|
			  == */
			return distance = 1 - dot_product/length_product;
		}
};
#endif
