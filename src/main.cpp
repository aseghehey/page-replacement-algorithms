# include <iostream>
# include <string>
# include <unordered_map>
# include <list>

static int PAGE_SIZE = 4096;
typedef std::unordered_map<unsigned int, char> page_map;
typedef std::list<unsigned int> linked_list;
typedef std::list<unsigned int>::iterator list_pointer;

void FIFO(const char * filename, int nFrames, std::string mode){
    int countR = 0; int countW = 0;
    page_map table; linked_list queue;
    unsigned int addr; char rw;
    
    FILE *traceFile;
    traceFile = fopen(filename, "r");
    int traceCount = 0;

    while (!feof(traceFile) && fscanf(traceFile, "%x %c", &addr, &rw)!=EOF){
        addr /= PAGE_SIZE;
        traceCount++;

        if (table.find(addr) == table.end()){ // page fault
            countR++;
            if (table.size() == nFrames){
                unsigned int deleteFrame = queue.front();
                queue.pop_front();
                if (table[deleteFrame] == 'W') countW++;
                table.erase(deleteFrame);
            }
            queue.push_back(addr);
            table[addr] = rw;
        }
        else {
            if (table[addr] == 'R') table[addr] = rw;
        }
    }
    fclose(traceFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << queue.size() << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << countR << "\nWrite Count: " << countW << std::endl;
    }
}

void segmentedFifo(){

}

void LRU(const char * filename, int numFrames, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0;

    page_map table; 
    linked_list queue;
    std::unordered_map<unsigned int, list_pointer> referenceHash;

    unsigned int addr; char rw;
    FILE * tFile;
    tFile = fopen(filename, "r");

    while (!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        if (table.find(addr) == table.end()){ // page fault
            rCount++;
            if (table.size() == numFrames){ // at capacity
                unsigned int leastRecentlyUsed = queue.back();
                if (table[leastRecentlyUsed] == 'W') wCount++; // dirty page
                queue.pop_back();
                table.erase(leastRecentlyUsed); referenceHash.erase(leastRecentlyUsed);
            }
            table[addr] = rw;
        }
        else{
            queue.erase(referenceHash[addr]);
        }

        queue.push_front(addr);
        referenceHash[addr] = queue.begin();
    }
    fclose(tFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << queue.size() << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << rCount << "\nWrite Count: " << wCount << std::endl;
    }
}

int main(){
    // FIFO("bzip.trace", 64, "quiet");
    LRU("bzip.trace", 64, "quiet");
    return 0;
}