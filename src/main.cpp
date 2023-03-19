# include <iostream>
# include "datastructures.hpp"
# include "functions.hpp"
# include "policies.hpp"

void PushFifo(unsigned int address, char r_w, PageMap& pTable, LinkedList& list);
void PopFifo(LinkedList& list, PageMap& pTable, unsigned int frameToDelete);
bool Hit(unsigned int address, char& r_w, PageMap& pTable, int& read);
void UpdateLRU(LinkedList& pFrame, IterMap& refFrame, unsigned int address);
void PushLRU(LinkedList& pFrame, char r_w, PageMap& pTable, IterMap& refFrame, unsigned int address);
void PopLRU(PageMap& pTable, LinkedList& pFrame, IterMap& refFrame, unsigned int frameToDelete);
FILE* OpenFile(const char * filename);

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
void Fifo(const char * filename, int nFrames, std::string mode){
    int countR = 0; int countW = 0; int traceCount = 0;
    PageMap table; LinkedList queue;
    unsigned int addr; char rw;
    
    FILE *traceFile = OpenFile(filename);
    while (!feof(traceFile) && fscanf(traceFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;
        if(!Hit(addr,rw,table,countR)){
            if (table.size() == nFrames){
                unsigned int frame = queue.front();
                if (table[frame] == 'W') countW++;
                PopFifo(queue, table, frame);
            }
            PushFifo(addr, rw, table, queue);
        }
    }
    fclose(traceFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << queue.size() << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << countR << "\nWrite Count: " << countW << std::endl;
    }
}

void LeastRecentlyUsed(const char * filename, int numFrames, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0;
    LinkedList pageFrame; PageMap table; IterMap frameRef;
    unsigned int addr; char rw;
    FILE * tFile = OpenFile(filename);

    while (!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;

        if (!Hit(addr, rw, table, rCount)){
            if (table.size() == numFrames){
                unsigned int frame = pageFrame.back();
                if (table[frame] == 'W') wCount++;
                PopLRU(table, pageFrame, frameRef, frame);
            }
            table[addr] = rw;
        }
        else{
            pageFrame.erase(frameRef[addr]);
        }
        UpdateLRU(pageFrame, frameRef, addr);
    }

    fclose(tFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << numFrames << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << rCount << "\nWrite Count: " << wCount << std::endl;
    }
}

void SegmentedFifo(const char* filename, int numFrames, int p, std::string mode){
    int  rCount = 0; int wCount = 0; int traceCount = 0;
    LinkedList fifo; LinkedList lru;
    IterMap referenceLRU;
    PageMap fifoTable; PageMap lruTable;
    int lengthLRU = (numFrames * p) / 100;
    int lengthFifo = numFrames - lengthLRU;
    FILE* tFile = OpenFile(filename);
    unsigned int addr; char rw;

    if (lengthLRU == 0){
        Fifo(filename, numFrames, mode);
        return;
    }
    if (lengthFifo == 0){
        LeastRecentlyUsed(filename, numFrames, mode);
        return;
    }
    
    while(!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;

        if(fifoTable.find(addr) == fifoTable.end()){ // no hit fifo
            if (lruTable.find(addr) != lruTable.end()){ // Page is in LRU and FIFO is full
                // Update W bit
                if (lruTable[addr] == 'R') lruTable[addr] = rw;
                rw = lruTable[addr];
                // Delete page from LRU
                // PopLRU(lruTable, lru, referenceLRU, addr);
                lru.erase(referenceLRU[addr]);
                lruTable.erase(addr);
                referenceLRU.erase(addr);
                // Get what goes in the lru from Fifo
                unsigned int oldestFifoPage = fifo.front(); 
                char oldestFifoPageRW = fifoTable[oldestFifoPage];
                // Remove old page from fifo
                PopFifo(fifo, fifoTable, oldestFifoPage);
                // Add old page to lru
                PushLRU(lru, oldestFifoPageRW, lruTable, referenceLRU, oldestFifoPage);
                // Add Hit page to fifo
                PushFifo(addr, rw, fifoTable, fifo);
            }
            else {
                if (fifo.size() == lengthFifo){
                    // delete oldest page in fifo, and place it in lru
                    unsigned int oldestFifoPage = fifo.front(); 
                    char oldestFifoPageRW = fifoTable[oldestFifoPage];
                    
                    // Check if LRU is full
                    if (lru.size() == lengthLRU){
                        unsigned int toDel = lru.back();
                        char _cur_rw = lruTable[toDel]; 
                        if (_cur_rw == 'W') wCount++;
                        PopLRU(lruTable, lru, referenceLRU, toDel);
                    }
                    PushLRU(lru, oldestFifoPageRW, lruTable, referenceLRU, oldestFifoPage);
                    // Pop oldest page
                    PopFifo(fifo, fifoTable, oldestFifoPage);
                }
                PushFifo(addr, rw, fifoTable, fifo);
                rCount++;
            }
        }
        else { // Case 1: Page is in FIFO cache/page table
            if (fifoTable[addr] == 'R') fifoTable[addr] = rw; // Update W bit
        }
    }
    fclose(tFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << numFrames << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << rCount << "\nWrite Count: " << wCount << std::endl;
    }

}

// testing
/*
int main(){
    Fifo("traces/bzip.trace", 64, "quiet");
    return 0;
}
*/