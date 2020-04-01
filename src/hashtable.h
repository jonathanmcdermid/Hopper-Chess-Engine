#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "move.h"

namespace Chess {
    class hashtable {
    public:
        hashtable() {
            zob = 0;  
            depth = 0; 
            bmove = move();
        }
        hashtable(unsigned long long z, int d, move b) { 
            zob = z; 
            depth = d; 
            bmove = b; 
        }
        unsigned long long getZobrist() const { return zob; }
        int getDepth() const { return depth; }
        move getBmove() const { return bmove; }
    private:
        unsigned long long zob;
        int depth;
        move bmove;
    };
}

#endif