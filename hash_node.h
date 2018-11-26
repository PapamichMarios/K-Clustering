#ifndef __HASH_NODE_H__
#define __HASH_NODE_H__

template <typename K>
class HashNode
{
	private:
    	K key;
    	std::string G;
		HashNode *next;
		std::string id;
		int assigned;

	public:
    	HashNode(const K &key, const std::string G, const std::string id) : key(key), G(G), next(NULL), id(id), assigned(0) { }
		
		~HashNode()
		{
			
		}

		K getKey()
		{
			return this->key;
		}

		void setAssigned(int x)
		{
			this->assigned = x;
		}

		int getAssigned()
		{
			return this->assigned;
		}

		std::string getG()
		{
			return this->G;
		}

		std::string getId()
		{
			return this->id;
		}

		HashNode *getNext()
		{
			return this->next;
		}

		void setNext(HashNode *next)
		{
			this->next = next;
		}

		long long int memory_used(int dim)
		{
			long long int memory=0;

			memory += sizeof(this->G);
			memory += sizeof(this->next);
			memory += sizeof(this->id);
			memory += dim * sizeof(this->key[0]);

			return memory;
		}
};
#endif
