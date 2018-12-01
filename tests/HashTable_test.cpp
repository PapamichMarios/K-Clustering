#include <gtest/gtest.h>
#include <vector>

#include "../utilities.h"

using namespace std;

TEST(HashTableTestEuclidean, ConstructorTest)
{
	int L=3;

	HashTable<vector<double>> ** hash_tableptr = new HashTable<vector<double>>*[L];

	for(int i=0; i<L; i++)
	{
		hash_tableptr[i] = new HashTable_EUC<vector <double>>(100, 5, 16);

		ASSERT_EQ(hash_tableptr[i]->getTableSize(), 100);
	}


	for(int i=0; i<L; i++)
		delete hash_tableptr[i];

	delete[] hash_tableptr;
}

TEST(HashTableTestCosine, Constructor)
{
	int L=3;

	HashTable<vector<double>> ** hash_tableptr = new HashTable<vector<double>>*[L];
	
	for(int i=0; i<L; i++)
	{
		hash_tableptr[i] = new HashTable_COS<vector <double>>(100, 5, 16);

		ASSERT_EQ(hash_tableptr[i]->getTableSize(), 100);
	}

	for(int i=0; i<L; i++)
		delete hash_tableptr[i];

	delete[] hash_tableptr;
}
