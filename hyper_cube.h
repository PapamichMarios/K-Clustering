#ifndef __HYPER_CUBE_H__
#define __HYPER_CUBE_H__

#include <string>
#include <algorithm>
#include <fstream>
#include <climits>
#include <ctime>

#include "help_functions.h"
#include "hyper_node.h"
#include "fi.h"
#include "g.h"

template <typename K>
class HyperCube
{
	protected:
		int tableSize;
		HyperNode<K> ** table;
		int points;

	public:
		HyperCube(int tableSize,int k)
		{
			this->table = new HyperNode<K> * [tableSize]();
			this->tableSize = tableSize;	
			this->points = 0;
		}

		virtual ~HyperCube()
		{
			/*== destroying all buckets*/
			HyperNode<K> * temp = NULL;
			HyperNode<K> * prev = NULL;

			for(int i=0; i<this->tableSize; i++)
			{
				temp = table[i];
				while(temp != NULL)
				{
					prev = temp;
					temp = temp->getNext();
					delete prev;
				}

				table[i] = NULL;
			}

			delete[] table;
		}	

		virtual void put(const K &point, std::string identifier) =0;

		virtual void ANN(const K &query, std::ofstream& outputfile, int probes, int M, double &distance_ANN, double &time_ANN, std::string &identifier_ANN) =0;
		virtual void NN(const K &query, std::ofstream& outputfile, double &distance_NN,double &time_NN) =0;
		virtual void RS (const K &query,std::ofstream& outputfile, int c, double R, int probes, int M) =0;
		virtual long long int memory_used(int dim)=0;
};

template <typename K>
class HyperCube_EUC : public HyperCube<K>
{
	private:
		fi * cube_function;

	public:
		HyperCube_EUC(int tableSize, int k, int dim) : HyperCube<K>(tableSize, k)
		{
			this->cube_function = new fi(k, dim);	
		}

		~HyperCube_EUC()
		{
			delete this->cube_function;
			this->cube_function = NULL;	
		}

		void put(const K &key, std::string identifier)
		{
			int hash_val = cube_function->cubeValue(key);

			HyperNode<K> * prev = NULL;
			HyperNode<K> * entry = this->table[hash_val];
			
        	while (entry != NULL) 
			{
            	prev = entry;
            	entry = entry->getNext();
        	}

        	if (entry == NULL) 
			{
            	entry = new HyperNode<K>(key, identifier);
				this->points++;

            	if (prev == NULL) 
				{
                	/*== insert as first bucket*/
                	this->table[hash_val] = entry;
            	} 	
				else 
				{
               		prev->setNext(entry);
            	}
        	} 
		}
		
		/*== Neighbour functions*/
		void NN(const K &query, std::ofstream& outputfile, double &distance_NN, double &time_NN)
		{
			double distance;
			HyperNode<K> * temp = NULL;
			clock_t end_time;

			double min_distance = INT_MAX;
			std::string identifier = "NONE";

			const clock_t begin_time = clock();

			/*== start iterating through the hash table*/
			for(int i=0; i<this->tableSize; i++)
			{
				/*== assign temp to the head of the bucket*/
				temp = this->table[i];	

				/*== iterate through every node in the bucket*/
				while( temp != NULL )
				{
					distance = help_functions::euclidean_distance(query, temp->getKey());
					
					if(distance < min_distance)
					{
						min_distance = distance;
						identifier = temp->getId();
					}

					/*== continue the iteration*/
					temp = temp->getNext();
				}
			}
			
			end_time = clock();

			distance_NN = min_distance;
			time_NN = double(end_time - begin_time) / CLOCKS_PER_SEC;
		}

		void ANN(const K &query, std::ofstream& outputfile, int probes, int M, double &distance_ANN, double &time_ANN, std::string &identifier_ANN)
		{
			std::vector<int> hamming_neighbours;

			double min_distance = INT_MAX;
			std::string identifier = "NONE";
			double distance=0;
			clock_t end_time;
			int probes_visited=0;
			int M_visited=0;

			int hash_val		= this->cube_function->cubeValue(query);
			HyperNode<K> * temp = this->table[hash_val];
			const clock_t begin_time = clock();


			/*== find the hamming distance for the hash_val*/
			for(int i=0; i<this->tableSize; i++)
				hamming_neighbours.push_back(help_functions::hamming_distance(hash_val, i));
			hamming_neighbours[hash_val] = INT_MAX;

			/*== search untill condition of hypercube*/
			while( probes_visited < probes && M_visited < M )
			{
				/*== get every point saved on the bucket and find the euclidean distance with query point*/
				while( temp != NULL )
				{
					distance = help_functions::euclidean_distance(query, temp->getKey());
		
					if(distance < min_distance)
					{
						min_distance = distance;
						identifier = temp->getId();
					}

					/*== continue the iteration*/
					M_visited++;
					temp = temp->getNext();
				}

				if( M_visited < M && probes_visited < probes )
				{
					/*== continue to next probe*/
					auto min = std::min_element(hamming_neighbours.begin(), hamming_neighbours.end());
					if(*min == INT_MAX)
					{
						std::cout << "Max probes searched" << std::endl;
						break;
					}

					int index = std::distance(hamming_neighbours.begin(), min);		

					/*== set pointer to closer probe*/
					temp = this->table[index];
					hamming_neighbours[index] = INT_MAX;

					probes_visited++;
				}
			}
			end_time = clock();

			distance_ANN = min_distance;
			identifier_ANN = identifier;
			time_ANN = double(end_time - begin_time) / CLOCKS_PER_SEC;
		}

		void RS(const K &query, std::ofstream& outputfile, int c, double R, int probes, int M)
		{
			std::vector<double> distance_list;
			std::vector<int> hamming_neighbours;
			std::vector<std::string> identifier_list;
			double distance=0;
			int probes_visited=0, M_visited=0;

			int hash_val		= cube_function->cubeValue(query);
			HyperNode<K> * temp  = this->table[hash_val];


			/*== find the hamming distance for the hash_val*/
			for(int i=0; i<this->tableSize; i++)
				hamming_neighbours.push_back(help_functions::hamming_distance(hash_val, i));
			hamming_neighbours[hash_val] = INT_MAX;

			/*== search untill condition of hypercube*/
			while( probes_visited < probes && M_visited < M )
			{
				/*== iterate through the bucket*/
				while( temp != NULL )
				{
					distance = help_functions::euclidean_distance(query, temp->getKey());
		
					if(distance<c*R)
					{
						distance_list.push_back(distance);
						identifier_list.push_back(temp->getId());
					}	

					/*== iterate to the next node*/
					temp = temp->getNext();	
					M_visited++;
				}	

				if( M_visited < M && probes_visited < probes )
				{
					/*== continue to next probe*/
					auto min = std::min_element(hamming_neighbours.begin(), hamming_neighbours.end());
					if(*min == INT_MAX)
					{
						std::cout << "Max probes searched" << std::endl;
						break;
					}

					int index = std::distance(hamming_neighbours.begin(), min);		

					/*== set pointer to closer probe*/
					temp = this->table[index];
					hamming_neighbours[index] = INT_MAX;

					probes_visited++;
				}
			}

			/*== print neighbours*/
			for(unsigned int i=0; i<distance_list.size(); i++)
				//outputfile << identifier_list[i] << " " << distance_list[i] << std::endl;
				outputfile << identifier_list[i] << std::endl;
		}

		long long int memory_used(int dim)
		{
			long long int memory=0;

			memory += sizeof(this->tableSize);
			memory += sizeof(this->points);
			memory += this->cube_function->memory_used();

			HyperNode<K> * temp = this->table[0];
			int i=0;
			while(temp==NULL)
			{
				++i;
				temp = this->table[i];
			}

			return memory += temp->memory_used(dim) * this->points;
		}
};

template <typename K>
class HyperCube_COS : public HyperCube<K>
{
	private:
		G * cube_function;	

	public:
		HyperCube_COS(int tableSize, int k, int dim) : HyperCube<K>(tableSize, k)
		{
			this->cube_function = new G(k, dim); 
		}

		~HyperCube_COS()
		{
			delete this->cube_function;
			this->cube_function = NULL;
		}
		
		void put(const K &key, std::string identifier)
		{
        	int hash_val = cube_function->hashValue(key);

        	HyperNode<K> *prev = NULL;
        	HyperNode<K> *entry = this->table[hash_val];

        	while (entry != NULL) 
			{
            	prev = entry;
            	entry = entry->getNext();
        	}

        	if (entry == NULL) 
			{
            	entry = new HyperNode<K>(key, identifier);
				this->points++;

            	if (prev == NULL) 
				{
                	/*== insert as first bucket*/
                	this->table[hash_val] = entry;
            	} 	
				else 
				{
               		prev->setNext(entry);
            	}
        	} 
		}
		
		void ANN(const K &query, std::ofstream &outputfile, int probes, int M, double &distance_ANN, double &time_ANN, std::string &identifier_ANN)
		{
			std::vector<int> hamming_neighbours;

			double min_distance = INT_MAX;
			std::string identifier = "NONE";
			double distance=0;
			clock_t end_time;
			int probes_visited=0;
			int M_visited=0;

			int hash_val		= this->cube_function->hashValue(query);
			HyperNode<K> * temp = this->table[hash_val];
			const clock_t begin_time = clock();


			/*== find the hamming distance for the hash_val*/
			for(int i=0; i<this->tableSize; i++)
				hamming_neighbours.push_back(help_functions::hamming_distance(hash_val, i));
			hamming_neighbours[hash_val] = INT_MAX;

			/*== search untill condition of hypercube*/
			while( probes_visited < probes && M_visited < M )
			{
				/*== get every point saved on the bucket and find the euclidean distance with query point*/
				while( temp != NULL )
				{
					distance = help_functions::cosine_distance(query, temp->getKey());
		
					if(distance < min_distance)
					{
						min_distance = distance;
						identifier = temp->getId();
					}

					/*== continue the iteration*/
					M_visited++;
					temp = temp->getNext();
				}

				if( M_visited < M && probes_visited < probes )
				{
					/*== continue to next probe*/
					auto min = std::min_element(hamming_neighbours.begin(), hamming_neighbours.end());
					if(*min == INT_MAX)
					{
						std::cout << "Max probes searched" << std::endl;
						break;
					}

					int index = std::distance(hamming_neighbours.begin(), min);		

					/*== set pointer to closer probe*/
					temp = this->table[index];
					hamming_neighbours[index] = INT_MAX;

					probes_visited++;
				}
			}
			end_time = clock();

			distance_ANN = min_distance;
			identifier_ANN = identifier;
			time_ANN = double(end_time - begin_time) / CLOCKS_PER_SEC;
		}

		void NN(const K &query, std::ofstream &outputfile, double &distance_NN, double &time_NN)
		{
			double distance;
			HyperNode<K> * temp = NULL;
			clock_t end_time;

			double min_distance = INT_MAX;
			std::string identifier = "NONE";

			const clock_t begin_time = clock();

			/*== start iterating through the hash table*/
			for(int i=0; i<this->tableSize; i++)
			{
				/*== assign temp to the head of the bucket*/
				temp = this->table[i];	

				/*== iterate through every node in the bucket*/
				while( temp != NULL )
				{
					distance = help_functions::cosine_distance(query, temp->getKey());
					
					if(distance < min_distance)
					{
						min_distance = distance;
						identifier = temp->getId();
					}

					/*== continue the iteration*/
					temp = temp->getNext();
				}
			}
			
			end_time = clock();

			distance_NN = min_distance;
			time_NN = double(end_time - begin_time) / CLOCKS_PER_SEC;
		}

		void RS (const K &query,std::ofstream& outputfile, int c, double R, int probes, int M)
		{
			std::vector<double> distance_list;
			std::vector<int> hamming_neighbours;
			std::vector<std::string> identifier_list;
			double distance=0;
			int probes_visited=0, M_visited=0;

			int hash_val		= cube_function->hashValue(query);
			HyperNode<K> * temp  = this->table[hash_val];


			/*== find the hamming distance for the hash_val*/
			for(int i=0; i<this->tableSize; i++)
				hamming_neighbours.push_back(help_functions::hamming_distance(hash_val, i));
			hamming_neighbours[hash_val] = INT_MAX;

			/*== search untill condition of hypercube*/
			while( probes_visited < probes && M_visited < M )
			{
				/*== iterate through the bucket*/
				while( temp != NULL )
				{
					distance = help_functions::cosine_distance(query, temp->getKey());
		
					if(distance<c*R)
					{
						distance_list.push_back(distance);
						identifier_list.push_back(temp->getId());
					}	

					/*== iterate to the next node*/
					temp = temp->getNext();	
					M_visited++;
				}	

				if( M_visited < M && probes_visited < probes )
				{
					/*== continue to next probe*/
					auto min = std::min_element(hamming_neighbours.begin(), hamming_neighbours.end());
					if(*min == INT_MAX)
					{
						std::cout << "Max probes searched" << std::endl;
						break;
					}

					int index = std::distance(hamming_neighbours.begin(), min);		

					/*== set pointer to closer probe*/
					temp = this->table[index];
					hamming_neighbours[index] = INT_MAX;

					probes_visited++;
				}
			}

			/*== print neighbours*/
			for(unsigned int i=0; i<distance_list.size(); i++)
				//outputfile << identifier_list[i] << " " << distance_list[i] << std::endl;
				outputfile << identifier_list[i] << std::endl;
		}

		long long int memory_used(int dim)
		{
			long long int memory=0;

			memory += sizeof(this->tableSize);
			memory += sizeof(this->points);
			memory += this->cube_function->memory_used();

			HyperNode<K> * temp = this->table[0];
			int i=0;
			while(temp==NULL)
			{
				++i;
				temp = this->table[i];
			}

			return memory += temp->memory_used(dim) * this->points;
		}
};
#endif
