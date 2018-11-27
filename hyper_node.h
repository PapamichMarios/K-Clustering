#ifndef __HYPER_NODE_H__
#define __HYPER_NODE_H__

template <typename K>
class HyperNode
{
	private:
		K point;
		std::string id;
		HyperNode* next;
		int assigned;

	public:
		HyperNode(const K &point, const std::string id) : point(point), id(id), next(NULL), assigned(0) { }

		~HyperNode()
		{
			
		}
		
		K getKey()
		{
			return this->point;
		}

		int getAssigned()
		{
			return this->assigned;
		}

		void setAssigned(int x)
		{
			this->assigned = x;
		}

		std::string getId()
		{
			return this->id;
		}

		HyperNode *getNext()
		{
			return this->next;
		}

		void setNext(HyperNode *next)
		{
			this->next = next;
		}
		
		long long int memory_used(int dim)
		{
			long long int memory=0;

			memory += sizeof(this->next);
			memory += sizeof(this->id);
			memory += dim * sizeof(this->point[0]);

			return memory;
		}
};
#endif

