#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "utilities.h"

using namespace std;

void getInlineArguments(int argc, char** argv, std::string &metric, short int &inputFileIndex, short int &configFileIndex, short int &outputFileIndex)
{
	int opt;

    while ((opt = getopt(argc, argv, "c:i:d:o:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                inputFileIndex = optind-1;
                break;
            case 'c':
                configFileIndex = optind-1;
                break;
            case 'd':
				metric = optarg;
                break;
            case 'o':
                outputFileIndex = optind-1;
                break;
            case '?':
                cout << "Invalid argument" << endl;
                exit(EXIT_FAILURE);
        }
    }
}

void getConfigurationParameters(char** argv, int &clusters, int &L, int &h, short int configFileIndex)
{
	std::ifstream configfile;
	std::string line;

	configfile.open(argv[configFileIndex]);
	if(!configfile.is_open())
	{
		cout << "Could not open configuration file" << endl;
		exit(EXIT_FAILURE);
	}
	
	/*== clusters*/
	getline(configfile, line);
    std::istringstream clusterStream(line);
    clusterStream >> line >> clusters;
	
	/*== hash functions h*/
	getline(configfile, line);
    std::istringstream hStream(line);
    hStream >> line >> h;

	/*== hash tables L*/
	getline(configfile, line);
    std::istringstream LStream(line);
    LStream >> line >> L;
}
