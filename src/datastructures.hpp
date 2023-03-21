#ifndef __DATA_STRUCTURES_H__
#define __DATA_STRUCTURES_H__
# include <unordered_map>
# include <list>

/* Declaration of the data structures that the policies will use */

typedef std::unordered_map<unsigned int, char> PageMap; // used for the page table and stores the key-value pair of address: bit
typedef std::list<unsigned int> LinkedList; // list used for the policies, to implement page eviction and such
typedef std::list<unsigned int>::iterator FramePointer; // used in LRU, to find page to be updated to most recently used
typedef std::unordered_map<unsigned int, FramePointer> IterMap; // map used to quickly reference pages in LRU

#endif