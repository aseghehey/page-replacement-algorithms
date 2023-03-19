# include "functions.hpp"
# include "policies.hpp"

void LeastRecentlyUsed(const char * filename, int numFrames, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0;
    LinkedList pageFrame; PageMap table; IterMap frameRef;
    unsigned int addr; char rw;
    FILE * tFile = OpenFile(filename);

    while (!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;

        if (!Hit(addr, rw, table, rCount)){
            if (mode == "debug"){
                std::cout << "Trace: " << traceCount << "\nAddress: " << addr << std::endl;
            }
            if (table.size() == numFrames){
                if (mode == "debug") std::cout << "LRU is full! Removing an item" << std::endl;
                unsigned int frame = pageFrame.back();
                if (table[frame] == 'W'){
                    wCount++;
                    if (mode == "debug") std::cout << "Dirty write" << std::endl;
                }
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
        std::cout <<"Total memory frames: " << numFrames << "\nEvents in trace: " << traceCount << 
        "\nTotal disk reads: " << rCount << "\nTotal disk writes: " << wCount << std::endl;
    }
}

