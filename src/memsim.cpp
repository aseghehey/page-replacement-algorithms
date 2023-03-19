#include "policies.hpp"
# include <iostream>
int main(int argc, char* argv[]){
    const char* file = argv[1];
    int nFrames = atoi(argv[2]);
    if (argc == 5){
        std::string mode = argv[4];
        std::string policy = argv[3];
        if (policy == "fifo"){
            Fifo(file, nFrames, mode);
        }
        else if (policy == "lru") {
            LeastRecentlyUsed(file, nFrames, mode);
        }
    }
    else {
        std::string mode = argv[5];
        int p = atoi(argv[4]);
        SegmentedFifo(file, nFrames, p, mode);
    }
    
    return 0;
}