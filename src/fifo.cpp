# include "functions.hpp"
# include "policies.hpp"

void Fifo(const char * filename, int numFrames, std::string mode){
    // read, dirty write and event counters
    int rCount = 0; int wCount = 0; int traceCount = 0;
    // declaring the page table and the fifo policy data structure
    PageMap table; LinkedList queue;
    unsigned int addr; char rw; // used to store values read from the tracefile
    
    FILE *traceFile = OpenFile(filename);
    // loop until there are no more entries in the tracefile
    while (!feof(traceFile) && fscanf(traceFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++; 
        addr /= PAGE_SIZE; // page is 4KB, so we need to divide it by 4096 (the value of PAGE_SIZE)
        // checking if page exists in the page table
        if(!Hit(addr,rw,table,rCount)){ 
            // No hit, page fault
            if (mode == "debug"){ // debug statements
                std::cout << "Page fault!" << std::endl;
                std::cout << "Trace: " << traceCount << "\nAddress: " << addr << std::endl;
            }
            if (table.size() == numFrames){ // checking if the page table is at capacity
                // need to make space for incoming pag
                if (mode == "debug") std::cout << "At capacity! Removing an item" << std::endl;
                // since we add to the back, we pop from the front - that would be the oldest page in FIFO
                unsigned int frame = queue.front(); // accessing oldest page
                // checking for dirty write and updating the dirty write counter if necessary
                if (table[frame] == 'W'){
                    wCount++;
                    if (mode == "debug") std::cout << "Dirty write" << std::endl;
                }
                // evicting page
                PopFifo(queue, table, frame);
            }
            if (mode == "debug") std::cout << "Added to fifo!" << std::endl;
            // adding newest page to the back of the FIFO 
            PushFifo(addr, rw, table, queue);
        }
    }
    fclose(traceFile);
    // printing relevant information for the quiet mode
    if (mode == "quiet"){
        std::cout <<"Total memory frames: " << numFrames << "\nEvents in trace: " << traceCount << 
        "\nTotal disk reads: " << rCount << "\nTotal disk writes: " << wCount << std::endl;
    }
}