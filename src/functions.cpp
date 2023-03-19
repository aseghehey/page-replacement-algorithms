# include "functions.hpp"

void PushFifo(unsigned int address, char r_w, PageMap& pTable, LinkedList& list){
    list.push_back(address);
    pTable[address] = r_w;
}
void PopFifo(LinkedList& list, PageMap& pTable, unsigned int frameToDelete){
    list.pop_front(); 
    pTable.erase(frameToDelete);
}
bool Hit(unsigned int address, char& r_w, PageMap& pTable, int& read){
    if (pTable.find(address) == pTable.end()){
        read++;
        return false;
    }
    if (pTable[address]=='R') pTable[address] = r_w; // update
    return true;
}
void UpdateLRU(LinkedList& pFrame, IterMap& refFrame, unsigned int address){
    pFrame.push_front(address);
    refFrame[address] = pFrame.begin();
}
void PushLRU(LinkedList& pFrame, char r_w, PageMap& pTable, IterMap& refFrame, unsigned int address){
    pTable[address] = r_w;
    pFrame.push_front(address);
    refFrame[address] = pFrame.begin();
}
void PopLRU(PageMap& pTable, LinkedList& pFrame, IterMap& refFrame, unsigned int frameToDelete){
    pTable.erase(frameToDelete); 
    refFrame.erase(frameToDelete);
    pFrame.pop_back(); 
}
FILE* OpenFile(const char * filename){
    FILE* file;
    file = fopen(filename, "r");
    return file;
}