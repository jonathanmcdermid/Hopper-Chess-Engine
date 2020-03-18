#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "move.h"

namespace Chess {
    class hashtable {
    public:
        hashtable() {
            zob     = 0;  
            depth   = 0; 
            bmove   = move();
        }
        hashtable(uint64_t z, uint8_t d, move b) { 
            zob     = z; 
            depth   = d; 
            bmove   = b; 
        }
        uint64_t getZobrist()   { return zob;   }
        uint8_t  getDepth()     { return depth; }
        move     getBmove()     { return bmove; }
        void     chrono()       { if (depth) { depth--; } else { bmove = move(); } }
    private:
        uint64_t zob;
        uint8_t  depth;
        move     bmove;
    };
}

#endif