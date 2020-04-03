#ifndef HASHENTRY_H
#define HASHENTRY_H

#include "move.h"

namespace Chess {
    class hashentry {
    public:
        hashentry() {
            zob = 0;
            depth = 0;
            m = move();
        }
        hashentry(U64 z, int d, move b) {
            zob = z;
            depth = d;
            m = b;
        }
        void operator=(hashentry a) { 
            zob = a.zob; 
            depth = a.depth;
            m = a.m;
        }
        U64 getZobrist() const { return zob; }
        int getDepth() const { return depth; }
        move getMove() const { return m; }
    private:
        U64 zob;
        int depth;
        move m;
    };
}

#endif