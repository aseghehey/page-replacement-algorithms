# include <iostream>
# include <string>
# include <unordered_map>
# include <list>

static int PAGE_SIZE = 4096;
typedef std::unordered_map<unsigned int, char> page_map;
typedef std::list<unsigned int> linked_list;
typedef std::list<unsigned int>::iterator frame_pointer;
typedef std::unordered_map<unsigned int, frame_pointer> iter_map;

void put_fifo(unsigned int& address, char& r_w, page_map& pTable, linked_list& list){
    list.push_back(address);
    pTable[address] = r_w;
}
void delete_fifo(linked_list& list, page_map& pTable, int& counter){
    unsigned int deleteFrame = list.front();
    if (pTable[deleteFrame] == 'W') counter++;
    list.pop_front(); pTable.erase(deleteFrame);
}
bool hit(unsigned int& address, char& r_w, page_map& pTable, int& counter){
    if (pTable.find(address) == pTable.end()){
        counter++;
        return false;
    }
    if (pTable[address]=='R') pTable[address] = r_w;
    return true;
}
void updateLRU(linked_list& pFrame, iter_map& refFrame, unsigned int& address){
    pFrame.push_front(address);
    refFrame[address] = pFrame.begin();
}
void addLRU(page_map& table, linked_list& pFrame, iter_map& refFrame, unsigned int& address, int& counter){
    unsigned int lru = pFrame.back();
    if (table[lru] == 'W') counter++;
    pFrame.pop_back(); table.erase(lru); refFrame.erase(lru);
}

void FIFO(const char * filename, int nFrames, std::string mode){
    int countR = 0; int countW = 0; int traceCount = 0;
    page_map table; linked_list queue;
    unsigned int addr; char rw;
    
    FILE *traceFile;
    traceFile = fopen(filename, "r");

    while (!feof(traceFile) && fscanf(traceFile, "%x %c", &addr, &rw)!=EOF){
        addr /= PAGE_SIZE;
        traceCount++;
        if(!hit(addr,rw,table,countR)){
            if (table.size() == nFrames){
                // at capacity
                delete_fifo(queue, table, countW);
            }
            put_fifo(addr, rw, table, queue);
        }
    }
    fclose(traceFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << queue.size() << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << countR << "\nWrite Count: " << countW << std::endl;
    }
}

void LRU(const char * filename, int numFrames, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0;
    linked_list pageFrame; page_map table; iter_map frameRef;

    unsigned int addr; char rw;
    FILE * tFile; tFile = fopen(filename, "r");

    while (!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;

        if (!hit(addr, rw, table, rCount)){
            if (table.size() == numFrames){
                addLRU(table, pageFrame, frameRef, addr, wCount);
            }
            table[addr] = rw;
        }
        else{
            pageFrame.erase(frameRef[addr]);
        }
        updateLRU(pageFrame, frameRef, addr);
    }

    fclose(tFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << numFrames << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << rCount << "\nWrite Count: " << wCount << std::endl;
    }
}
/*
void VMS(const char * filename, int numFrames, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0;
    linked_list cache1; linked_list cache2;
    page_map table; iter_map refFrame;

    int p = numFrames / 2;
    int sizeC2 = (numFrames * p) / 100; 
    int sizeC1 = numFrames - sizeC2;

    unsigned int addr; char rw;   
    FILE * tFile;
    tFile = fopen(filename, "r");

    while(!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw) != EOF)
    {
        if (table.find(addr) == table.end()){

        }
        else {

        }
    }
    fclose(tFile);

}
*/
int main(){

    FIFO("bzip.trace", 64, "quiet");
    LRU("bzip.trace", 64, "quiet");
    return 0;
}