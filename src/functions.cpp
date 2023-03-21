# include "functions.hpp"
/* To avoid repetition and make the program modular, I chose to split the common operations into functions */

void PushFifo(unsigned int address, char r_w, PageMap& pTable, LinkedList& list){
    /* Given a page and its bit, add it to the back of the fifo DS, and to the page table*/
    list.push_back(address);
    pTable[address] = r_w;
}
void PopFifo(LinkedList& list, PageMap& pTable, unsigned int frameToDelete){
    /* Given a FIFO list, pop the oldest page (at the front). It is necessary to pass the frame to delete,
    to remove it from the page table */
    list.pop_front(); 
    pTable.erase(frameToDelete);
}
bool Hit(unsigned int address, char& r_w, PageMap& pTable, int& read){
    /* This function checks for a page hit. If true, update W bit and return true, else, increment the read counter and return false*/
    if (pTable.find(address) == pTable.end()){
        read++;
        return false;
    }
    if (pTable[address]=='R') pTable[address] = r_w; // update
    return true;
}
void UpdateLRU(LinkedList& pFrame, IterMap& refFrame, unsigned int address){
    /* This function updates a page to become the most recently used, should be called ONLY when that said page has a hit*/
    pFrame.push_front(address);
    refFrame[address] = pFrame.begin();
}
void PushLRU(LinkedList& pFrame, char r_w, PageMap& pTable, IterMap& refFrame, unsigned int address){
    /* Pushes a new page to become the most recently used */
    pTable[address] = r_w;
    pFrame.push_front(address);
    refFrame[address] = pFrame.begin();
}
void PopLRU(PageMap& pTable, LinkedList& pFrame, IterMap& refFrame, unsigned int frameToDelete){
    /* Removes a page from the LRU, and the page table*/
    pTable.erase(frameToDelete); 
    refFrame.erase(frameToDelete);
    pFrame.pop_back(); 
}
FILE* OpenFile(const char * filename){
    /* Given a filename, the function returns a FILE* object */
    FILE* file;
    file = fopen(filename, "r");
    return file;
}