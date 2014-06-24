#ifndef EvTree_hh
#define EvTree_hh
#include "Named.hh"
#include "data_structures.hh"


class TTree;

class EvTree: public Named {
	public:
		EvTree();
		EvTree(const char*, const char*, struct event_t&, int);
		virtual ~EvTree();
		void Fill();
		void Write();
	private:
		TTree* f_tree;
};
#endif
