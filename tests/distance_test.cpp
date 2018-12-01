#include <gtest/gtest.h>
#include <vector>

#include "../metric.h"
#include "../utilities.h"

using namespace std;

TEST(DistanceEuclideanTest, VectorSizeAndResult)
{
	Metric<double>* metric_ptr = getMetric("euclidean");

	vector<double> x{1.0, 2.0, 3.0};
	vector<double> y{1.0, 2.0};

	ASSERT_EQ(-1, metric_ptr->distance2(x, y));

	y.push_back(4.0);
	ASSERT_EQ(1, metric_ptr->distance2(x, y));
}

TEST(DistanceCosineTest, VectorSizeAndResult)
{
	Metric<double>* metric_ptr = getMetric("cosine");
	
	vector<double> x{1.0, 2.0, 3.0};
	vector<double> y{1.0, 2.0};

	ASSERT_EQ(-1, metric_ptr->distance2(x, y));

	y.push_back(3.0);
	ASSERT_EQ(0, metric_ptr->distance2(x, y));
}
