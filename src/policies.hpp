#ifndef __POLICIES_H__
#define __POLICIES_H__
#include <string>
void LeastRecentlyUsed(const char * filename, int numFrames, std::string mode);
void Fifo(const char * filename, int nFrames, std::string mode);
void SegmentedFifo(const char* filename, int numFrames, int p, std::string mode); 
#endif