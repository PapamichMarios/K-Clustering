#include <vector>
#include <gtest/gtest.h>

#include "../hash_table.h"
#include "../hash_node.h"
#include "../hyper_cube.h"
#include "../hyper_node.h"
#include "../fi.cpp"
#include "../g.cpp"
#include "../assignment.cpp"
#include "../initialization.cpp"
#include "../update.cpp"
#include "../help_functions.h"
#include "../hyperplane.cpp"
#include "../metric.h"
#include "../utilities.cpp"
 
using namespace std;

TEST(BinarySearchTest, IndexReturned)
{ 
	vector<double> arr;
	arr.push_back(0);
	arr.push_back(1.2);
	arr.push_back(2.1);
	arr.push_back(3.0);
	arr.push_back(3.5);
	arr.push_back(4.0);

    ASSERT_EQ(1, binarySearch(arr, 0, 6, 1.0));
    ASSERT_EQ(2, binarySearch(arr, 0, 6, 1.82));
    ASSERT_EQ(3, binarySearch(arr, 0, 6, 2.4));
    ASSERT_EQ(4, binarySearch(arr, 0, 6, 3.3));
	ASSERT_EQ(5, binarySearch(arr, 0, 6, 3.75));
}
 
