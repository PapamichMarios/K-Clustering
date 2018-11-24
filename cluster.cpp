#include <iostream>
#include <getopt.h>
#include <sstream>
#include <fstream>

#include "utilities.h"

#define NECESSARY_ARGUMENTS 9

using namespace std;

int main(int argc, char ** argv)
{
	string metric;
	short int inputFileIndex;
	short int configFileIndex;
	short int outputFileIndex;
	
	int clusters;
	int L = 5;
	int h = 4;

	if( argc != NECESSARY_ARGUMENTS )
	{
		cout << "Rerun: ./cluster -i <input_file> -o <output_file> -c <configuration_file> -d <metric>" << endl;
		exit(EXIT_FAILURE);
	}

	/*== get all input arguments through getopt()*/
	getInlineArguments(argc, argv, metric, inputFileIndex, configFileIndex, outputFileIndex);

	/*== get config elements*/
	getConfigurationParameters(argv, clusters, L, h, configFileIndex);

	exit(EXIT_SUCCESS);
}
