# include "functions.hpp"
# include "policies.hpp"

void LeastRecentlyUsed(const char * filename, int numFrames, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0; // counters
    LinkedList pageFrame; PageMap table; IterMap frameRef; // data structures 
    unsigned int addr; char rw;
    FILE * tFile = OpenFile(filename);

    while (!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;

        if (!Hit(addr, rw, table, rCount)){
            if (mode == "debug"){
                std::cout << "Trace: " << traceCount << "\nAddress: " << addr << std::endl;
            }
            if (table.size() == numFrames){ // at capacity, pop least recently used
                if (mode == "debug") std::cout << "LRU is full! Removing an item" << std::endl;
                unsigned int frame = pageFrame.back();
                if (table[frame] == 'W'){
                    wCount++;
                    if (mode == "debug") std::cout << "Dirty write" << std::endl;
                }
                PopLRU(table, pageFrame, frameRef, frame);
            }
            table[addr] = rw; // adding new page to page table
        }
        else{
            pageFrame.erase(frameRef[addr]); // if hit, we need to update, so it goes through updateLRU right after
        }
        /* in each case we update the page in the LRU, and that could mean adding it as well. 
        But we only ever add to the page table once, hence why I have the case where we add in line 27, and do not call PushLRU(), as
        that function adds a frame to the page table as well
        */
        UpdateLRU(pageFrame, frameRef, addr); 
    }

    fclose(tFile);
    if (mode == "quiet"){
        std::cout <<"Total memory frames: " << numFrames << "\nEvents in trace: " << traceCount << 
        "\nTotal disk reads: " << rCount << "\nTotal disk writes: " << wCount << std::endl;
    }
}

