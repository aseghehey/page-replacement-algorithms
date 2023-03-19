# include "functions.hpp"
# include "policies.hpp"

void Fifo(const char * filename, int numFrames, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0;
    PageMap table; LinkedList queue;
    unsigned int addr; char rw;
    
    FILE *traceFile = OpenFile(filename);
    while (!feof(traceFile) && fscanf(traceFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;
        if(!Hit(addr,rw,table,rCount)){
            if (mode == "debug"){
                std::cout << "Page fault!" << std::endl;
                std::cout << "Trace: " << traceCount << "\nAddress: " << addr << std::endl;
            }
            if (table.size() == numFrames){
                if (mode == "debug") std::cout << "At capacity! Removing an item" << std::endl;
                unsigned int frame = queue.front();
                if (table[frame] == 'W'){
                    wCount++;
                    if (mode == "debug") std::cout << "Dirty write" << std::endl;
                }
                PopFifo(queue, table, frame);
            }
            if (mode == "debug") std::cout << "Added to fifo!" << std::endl;
            PushFifo(addr, rw, table, queue);
        }
    }
    fclose(traceFile);
    if (mode == "quiet"){
        std::cout <<"Total memory frames: " << numFrames << "\nEvents in trace: " << traceCount << 
        "\nTotal disk reads: " << rCount << "\nTotal disk writes: " << wCount << std::endl;
    }
}