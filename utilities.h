#ifndef __UTILITIES_H__
#define __UTILITIES_H__

void getInlineArguments(int argc, char** argv, std::string &metric, short int &inputFileIndex, short int &configFileIndex, short int &outputFileIndex);
void getConfigurationParameters(char** argv, int &clusters, int &L, int &h, short int configFileIndex);

#endif
