# include <iostream>
# include <string>
# include <unordered_map>
# include <list>

static int PAGE_SIZE = 4096;
typedef std::unordered_map<unsigned int, char> page_map;
typedef std::list<unsigned int> linked_list;
typedef std::list<unsigned int>::iterator frame_pointer;
typedef std::unordered_map<unsigned int, frame_pointer> iter_map;

void push_fifo(unsigned int& address, char& r_w, page_map& pTable, linked_list& list){
    list.push_back(address);
    pTable[address] = r_w;
}
void pop_fifo(linked_list& list, page_map& pTable,unsigned int& frameToDelete){
    list.pop_front(); 
    pTable.erase(frameToDelete);
}
bool hit(unsigned int& address, char& r_w, page_map& pTable, int& read){
    if (pTable.find(address) == pTable.end()){
        read++;
        return false;
    }
    if (pTable[address]=='R') pTable[address] = r_w; // update
    return true;
}
void update_lru(linked_list& pFrame, iter_map& refFrame, unsigned int& address){
    pFrame.push_front(address);
    refFrame[address] = pFrame.begin();
}
void push_lru(linked_list& pFrame, char& r_w, page_map& pTable, iter_map& refFrame, unsigned int& address){
    pTable[address] = r_w;
    pFrame.push_front(address);
    refFrame[address] = pFrame.begin();
}
void pop_lru(page_map& pTable, linked_list& pFrame, iter_map& refFrame, unsigned int& frameToDelete){
    pFrame.pop_back(); 
    pTable.erase(frameToDelete); 
    refFrame.erase(frameToDelete);
}
FILE* openFile(const char * filename){
    FILE* file;
    file = fopen(filename, "r");
    return file;
}


void FIFO(const char * filename, int nFrames, std::string mode){
    int countR = 0; int countW = 0; int traceCount = 0;
    page_map table; linked_list queue;
    unsigned int addr; char rw;
    
    FILE *traceFile = openFile(filename);
    while (!feof(traceFile) && fscanf(traceFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;
        if(!hit(addr,rw,table,countR)){
            if (table.size() == nFrames){
                unsigned int frame = queue.front();
                if (table[frame] == 'W') countW++;
                pop_fifo(queue, table, frame);
            }
            push_fifo(addr, rw, table, queue);
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
    FILE * tFile = openFile(filename);

    while (!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;

        if (!hit(addr, rw, table, rCount)){
            if (table.size() == numFrames){
                unsigned int frame = pageFrame.back();
                if (table[frame] == 'W') wCount++;
                pop_lru(table, pageFrame, frameRef, frame);
            }
            table[addr] = rw;
        }
        else{
            pageFrame.erase(frameRef[addr]);
        }
        update_lru(pageFrame, frameRef, addr);
    }

    fclose(tFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << numFrames << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << rCount << "\nWrite Count: " << wCount << std::endl;
    }
}

void VMS(const char * filename, int numFrames, int p, std::string mode){
    int rCount = 0; int wCount = 0; int traceCount = 0;
    linked_list fifo; linked_list lru; iter_map refLRU; page_map table;
    int len2 = (numFrames * p) / 100;
    int len1 = numFrames - len2;
    unsigned int addr; char rw;
    FILE* tFile = openFile(filename);

    while (!feof(tFile) && fscanf(tFile, "%x %c", &addr, &rw)!=EOF){
        traceCount++;
        addr /= PAGE_SIZE;

        if (!hit(addr, rw, table, rCount)){ // miss #1
            if (fifo.size() == len1){
                unsigned int frame_addr = fifo.front();
                char rw_ = table[frame_addr];

                if (lru.size() == len2){ // both full
                    unsigned int frame_to_Remove = lru.back();
                    if (table[frame_to_Remove] == 'W') wCount++;
                    pop_lru(table, lru, refLRU, frame_to_Remove);
                    pop_fifo(fifo, table, frame_addr);
                    push_lru(lru, rw_, table, refLRU, frame_addr);
                }
                else { // lru not full
                    pop_fifo(fifo, table, frame_addr);
                    push_lru(lru, rw_, table, refLRU, frame_addr);
                }
            }
            push_fifo(addr, rw, table, fifo);
        }
        else {
            if (refLRU.find(addr) != refLRU.end()){
                unsigned int frame_addr = lru.back();
                char rw_ = table[frame_addr];
                if (fifo.size() < len1){
                    pop_lru(table, lru, refLRU, frame_addr);
                }
                else { // full
                    if (lru.size() < len2){
                        unsigned int fifo_frame_addr = fifo.front(); 
                        char fifo_frame_rw = table[fifo_frame_addr];
                        pop_fifo(fifo, table, fifo_frame_addr);
                        push_lru(lru, fifo_frame_rw, table, refLRU, fifo_frame_addr);
                    }
                }
                push_fifo(frame_addr, rw_, table, fifo);
            }
        }

    }
    fclose(tFile);
    if (mode == "quiet"){
        std::cout <<"Memory Frames: " << numFrames << "\nTrace Count: " << traceCount << 
        "\nRead Count: " << rCount << "\nWrite Count: " << wCount << std::endl;
    }
}

int main(){

    FIFO("bzip.trace", 64, "quiet");
    std::cout << std::endl;
    LRU("bzip.trace", 64, "quiet");
    std::cout << std::endl;
    VMS("bzip.trace", 64, 50, "quiet");
    return 0;
}