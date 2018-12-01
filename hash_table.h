#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include <string>
#include <algorithm>
#include <fstream>
#include <climits>
#include <ctime>
#include <map>
#include <vector>

#include "hash_node.h"
#include "help_functions.h"
#include "fi.h"
#include "g.h"

template <typename K>
class HashTable
{
	protected:
    	HashNode<K> **table;
		int tableSize;
		int buckets;

	public:
		HashTable(int tableSize, int k, int dim)
		{
			/*== constructing hash table*/
			this->table = new HashNode<K> *[tableSize]();

			this->tableSize = tableSize;
			this->buckets = 0;
		}
	
		virtual ~HashTable()
		{
			/*== destroying all buckets*/
			HashNode<K> * temp = NULL;
			HashNode<K> * prev = NULL;

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
		
		virtual int hash(const K &key)=0;
    	virtual void put(const K &key, std::string identifier) =0;

		virtual std::vector<std::string> ANN(const K &query, double &distance_ANN, double & time_ANN) =0;
		virtual std::vector<std::string> NN(const K &query, std::ofstream &outputfile, double &distance_NN) =0;
		virtual void RS(const K &query, int c, double R, std::vector<int>& labels, int cluster) = 0;
		virtual void RS_conflict(const std::vector<K> &queries, int c, double R, std::vector<int>& labels, std::vector<int> clusters) =0;
		virtual void loyds_unassigned(const std::vector<K> &queries, std::vector<int>& labels) =0;

		virtual long long int memory_used(int dim)=0;

		int getTableSize()
		{
			return this->tableSize;
		}

		void reset_assigned()
		{
			HashNode<K> * temp;

			for(int i=0; i<this->tableSize; i++)
			{
				temp = this->table[i];
				while(temp != NULL)
				{
					/*== reset assigned flag*/
					temp->setAssigned(0);

					/*== iterate to the next hash node*/
					temp = temp->getNext();
				}
			}
		}
};

template <typename K>
class HashTable_EUC : public HashTable<K>
{
	private:
		fi * hash_function;

	public:	
		HashTable_EUC(int tableSize, int k, int dim) : HashTable<K>(tableSize, k, dim)
		{
			/*== construct fi object, used for hashing*/
			this->hash_function = new fi(k, dim);
		}

		~HashTable_EUC()
		{
			delete hash_function;
		}

		int hash(const K &key)
		{
			return hash_function->hashValue(key, this->tableSize);
		}

    	void put(const K &key, std::string identifier)
		{
        	int hash_val = hash_function->hashValue(key, this->tableSize);
        	HashNode<K> *prev = NULL;
        	HashNode<K> *entry = this->table[hash_val];

        	while (entry != NULL) 
			{
            	prev = entry;
            	entry = entry->getNext();
        	}

        	if (entry == NULL) 
			{
				std::string G = hash_function->computeG(key);
            	entry = new HashNode<K>(key, G, identifier);
				this->buckets++;

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
		std::vector<std::string> NN(const K &query, std::ofstream &outputfile, double &distance_NN)
		{
			double distance;
			HashNode<K> * temp = NULL;
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
					/*== if they match calculate distance*/
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

			/*== save measurements into a vector of size 3
				 0: distance
				 1: time
			  == */

			distance_NN = min_distance;

			std::vector<std::string> measurements(2);
			measurements[0] = std::to_string(min_distance);
			measurements[1] = std::to_string(double(end_time - begin_time) / CLOCKS_PER_SEC);

			return measurements;
		}

		std::vector<std::string> ANN(const K &query, double &distance_ANN, double &time_ANN)
		{
			double min_distance = INT_MAX;
			std::string identifier = "NONE";
			double distance=0;
			clock_t end_time;

			int hash_val		= hash_function->hashValue(query, this->tableSize);
			std::string G		= hash_function->computeG(query);
			HashNode<K> * temp  = this->table[hash_val];
			const clock_t begin_time = clock();


			/*== get every point saved on the bucket and find the euclidean distance with query point*/
			while( temp != NULL )
			{
				/*== compare the g(query) with the g(point)*/
				if( G != temp->getG() )
				{
					temp = temp->getNext();
					continue;
				}

				/*== if they match, save the distance*/
				distance = help_functions::euclidean_distance(query, temp->getKey());
	
				if(distance < min_distance)
				{
					min_distance = distance;
					identifier = temp->getId();
				}

				/*== continue the iteration*/
				temp = temp->getNext();
			}
			end_time = clock();

			/*== save measurements into a vector of size 3
				 0: distance
				 1: identifier
				 2: time
			  == */

			if(min_distance < distance_ANN)
			{
				distance_ANN = min_distance;
				time_ANN = double(end_time - begin_time) / CLOCKS_PER_SEC;
			}

			std::vector<std::string> measurements(3);
			measurements[0] = std::to_string(min_distance);
			measurements[1] = identifier;
			measurements[2] = std::to_string(double(end_time - begin_time) / CLOCKS_PER_SEC);

			return measurements;
		}

		void RS(const K &query, int c, double R, std::vector<int>& labels, int cluster)
		{
			double distance=0;

			int hash_val		= hash_function->hashValue(query, this->tableSize);
			std::string G		= hash_function->computeG(query);
			HashNode<K> * temp  = this->table[hash_val];
			
			/*== iterate through the bucket*/
			while( temp != NULL )
			{
				/*== compare the g(query) with the g(point)*/
				if( G != temp->getG() || temp->getAssigned() == 1)
				{
					temp = temp->getNext();
					continue;
				}
			
				/*== if they match calculate the distance*/
				distance = help_functions::euclidean_distance(query, temp->getKey());
	
				if(distance<c*R)
				{
					labels[stoi(temp->getId())-1] = cluster;
					temp->setAssigned(1);
				}

				/*== iterate to the next node*/
				temp = temp->getNext();	
			}
		}

		void RS_conflict(const std::vector<K> &queries, int c, double R, std::vector<int>& labels, std::vector<int> clusters)
		{
			double distance=0;

			int hash_val		= hash_function->hashValue(queries[0], this->tableSize);
			std::string G		= hash_function->computeG(queries[0]);
			HashNode<K> * temp  = this->table[hash_val];

			/*== iterate through the bucket*/
			while( temp != NULL )
			{
				/*== compare the g(query) with the g(point)*/
				if( G != temp->getG() || temp->getAssigned() == 1)
				{
					temp = temp->getNext();
					continue;
				}
			
				/*== if they match calculate the distance*/
				double min_distance = INT_MAX;
				int cluster;
				for(unsigned int i=0; i<clusters.size(); i++)
				{
					distance = help_functions::euclidean_distance(queries[clusters[i]], temp->getKey());
					
					if(distance<min_distance)
					{
						min_distance = distance;
						cluster = clusters[i];
					}
				}

				if(min_distance<c*R)
				{
					labels[stoi(temp->getId())-1] = cluster;
					temp->setAssigned(1);
				}

				/*== iterate to the next node*/
				temp = temp->getNext();	
			}
		}

		void loyds_unassigned(const std::vector<K> &queries, std::vector<int>& labels)
		{
			HashNode<K> * temp;

			for(int i=0; i<this->tableSize; i++)
			{
				temp = this->table[i];
				while(temp != NULL)
				{
					/*== assign unassigned point*/
					if(temp->getAssigned() == 0)
					{
						double min_distance = INT_MAX;
						double distance;
						int cluster;

						for(unsigned int j=0; j<queries.size(); j++)
						{
							distance = help_functions::euclidean_distance(queries[j], temp->getKey());

							if(distance<min_distance)
							{
								min_distance = distance;
								cluster = j;
							}
						}

						labels[stoi(temp->getId())-1] = cluster;
					}

					/*== reset assigned flag*/
					temp->setAssigned(0);

					/*== iterate to the next hash node*/
					temp = temp->getNext();
				}
			}
		}

		long long int memory_used(int dim)
		{
			long long int memory=0;

			memory += sizeof(this->tableSize);
			memory += sizeof(this->buckets);
			memory += this->hash_function->memory_used();

			HashNode<K> * temp = this->table[0];
			int i=0;
			/*== iterate to find a bucket that has something in it*/
			while( temp == NULL)
			{
				++i;
				temp = this->table[i];
			}

			return memory += this->buckets * temp->memory_used(dim);
		}

};

template <typename K>
class HashTable_COS : public HashTable<K>
{
	private:
		G * hash_function;

	public:
		HashTable_COS(int tableSize, int k, int dim) : HashTable<K>(tableSize, k, dim)
		{
			/*== construct g object, used for hashing*/
			this->hash_function = new G(k, dim);
		}

		~HashTable_COS()
		{
			delete hash_function;
		}

		int hash(const K &key)
		{
			return hash_function->hashValue(key);
		}

    	void put(const K &key, std::string identifier)
		{
        	int hash_val = hash_function->hashValue(key);
        	HashNode<K> *prev = NULL;
        	HashNode<K> *entry = this->table[hash_val];

        	while (entry != NULL) 
			{
            	prev = entry;
            	entry = entry->getNext();
        	}

        	if (entry == NULL) 
			{
            	entry = new HashNode<K>(key, std::to_string(hash_val), identifier);
				this->buckets++;

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
		std::vector<std::string> NN(const K &query,std::ofstream &outputfile, double &distance_NN)
		{
			double distance;
			HashNode<K> * temp = NULL;
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
					/*== calculate distance*/
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

			/*== save measurements into a vector of size 3
				 0: distance
				 1: time
			  == */

			distance_NN = min_distance;

			std::vector<std::string> measurements(2);
			measurements[0] = std::to_string(min_distance);
			measurements[1] = std::to_string(double(end_time - begin_time) / CLOCKS_PER_SEC);

			return measurements;
		}

		std::vector<std::string> ANN(const K &query, double &distance_ANN, double &time_ANN)
		{
			double distance=0;
			double min_distance = INT_MAX;
			std::string identifier = "NONE";
			clock_t end_time;

			int hash_val		= hash_function->hashValue(query);
			HashNode<K> * temp  = this->table[hash_val];
			const clock_t begin_time = clock();

			/*== get every point saved on the bucket and find the euclidean distance with query point*/
			while( temp != NULL )
			{
				/*== calculate the distance*/
				distance = help_functions::cosine_distance(query, temp->getKey());
	
				if(distance < min_distance)
				{
					min_distance = distance;
					identifier = temp->getId();
				}

				/*== continue the iteration*/
				temp = temp->getNext();
			}
			end_time = clock();

			/*== save measurements into a vector of size 3
				 0: distance
				 1: identifier
				 2: time
			  == */

			if(min_distance < distance_ANN)
			{
				distance_ANN = min_distance;
				time_ANN = double(end_time - begin_time) / CLOCKS_PER_SEC;
			}

			std::vector<std::string> measurements(3);
			measurements[0] = std::to_string(min_distance);
			measurements[1] = identifier;
			measurements[2] = std::to_string(double(end_time - begin_time) / CLOCKS_PER_SEC);

			return measurements;
		}

		void RS(const K &query, int c, double R, std::vector<int>& labels, int cluster)
		{
			double distance=0;

			int hash_val		= hash_function->hashValue(query);
			HashNode<K> * temp  = this->table[hash_val];

			/*== iterate through the bucket*/
			while( temp != NULL )
			{
				if(temp->getAssigned() == 1)
				{
					temp = temp->getNext();
					continue;
				}

				/*== calculate the distance*/
				distance = help_functions::cosine_distance(query, temp->getKey());

				if(distance<c*R)
				{
					labels[stoi(temp->getId())-1] = cluster;
					temp->setAssigned(1);
				}

				/*== iterate to the next node*/
				temp = temp->getNext();	
			}	
		}
		
		void RS_conflict(const std::vector<K> &queries, int c, double R, std::vector<int>& labels, std::vector<int> clusters)
		{
			double distance=0;

			int hash_val		= hash_function->hashValue(queries[0]);
			HashNode<K> * temp  = this->table[hash_val];

			/*== iterate through the bucket*/
			while( temp != NULL )
			{
				if(temp->getAssigned() == 1)
				{
					temp = temp->getNext();
					continue;
				}
			
				/*== calculate the distance*/
				double min_distance = INT_MAX;
				int cluster;
				for(unsigned int i=0; i<clusters.size(); i++)
				{
					distance = help_functions::cosine_distance(queries[clusters[i]], temp->getKey());
					
					if(distance<min_distance)
					{
						min_distance = distance;
						cluster = clusters[i];
					}
				}

				if(min_distance<c*R)
				{
					labels[stoi(temp->getId())-1] = cluster;
					temp->setAssigned(1);
				}

				/*== iterate to the next node*/
				temp = temp->getNext();	
			}
		}

		void loyds_unassigned(const std::vector<K> &queries, std::vector<int>& labels)
		{
			
		}

		long long int memory_used(int dim)
		{
			long long int memory=0;

			memory += sizeof(this->tableSize);
			memory += sizeof(this->buckets);
			memory += this->hash_function->memory_used();

			HashNode<K> * temp = this->table[0];
			int i=0;
			/*== iterate to find a bucket that has something in it*/
			while( temp == NULL)
			{
				++i;
				temp = this->table[i];
			}

			return memory += this->buckets * temp->memory_used(dim);
		}
};

#endif
