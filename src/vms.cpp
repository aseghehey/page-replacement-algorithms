# include "functions.hpp"
# include "policies.hpp"

void SegmentedFifo(const char* filename, int numFrames, int p, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0;
    LinkedList fifo; LinkedList lru;
    IterMap referenceLRU;
    PageMap fifoTable; PageMap lruTable;
    // calculating lengths based on desired percentage
    int lengthLRU = (numFrames * p) / 100; int lengthFifo = numFrames - lengthLRU;
    FILE* tFile = OpenFile(filename);
    unsigned int addr; char rw;

    // handling edge cases
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
        std::cout <<"Total memory frames: " << numFrames << "\nEvents in trace: " << traceCount << 
        "\nTotal disk reads: " << rCount << "\nTotal disk writes: " << wCount << std::endl;
    }

}
