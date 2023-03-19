#ifndef __DATA_STRUCTURES_H__
#define __DATA_STRUCTURES_H__
# include <unordered_map>
# include <list>

typedef std::unordered_map<unsigned int, char> PageMap;
typedef std::list<unsigned int> LinkedList;
typedef std::list<unsigned int>::iterator FramePointer;
typedef std::unordered_map<unsigned int, FramePointer> IterMap;

#endif