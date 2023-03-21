# include <iostream>
# include "datastructures.hpp"

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

/* Function declaration to make it easier for the policies to refer to*/

static int PAGE_SIZE = 4096; // global constant that will be used to divide the addresses by
void PushFifo(unsigned int address, char r_w, PageMap& pTable, LinkedList& list);
void PopFifo(LinkedList& list, PageMap& pTable, unsigned int frameToDelete);
bool Hit(unsigned int address, char& r_w, PageMap& pTable, int& read);
void UpdateLRU(LinkedList& pFrame, IterMap& refFrame, unsigned int address);
void PushLRU(LinkedList& pFrame, char r_w, PageMap& pTable, IterMap& refFrame, unsigned int address);
void PopLRU(PageMap& pTable, LinkedList& pFrame, IterMap& refFrame, unsigned int frameToDelete);
FILE* OpenFile(const char * filename);
#endif