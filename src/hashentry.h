#ifndef HASHENTRY_H
#define HASHENTRY_H

#include "move.h"

namespace Chess {
    class hashentry {
    public:
        hashentry() {
            zob = 0;
            depth = 0;
            eval = 0;
            flags = 0;
            m = move();
        }
        hashentry(U64 z, int d, int e, int f, move b) {
            zob = z;
            depth = d;
            eval = e;
            flags = f;
            m = b;
        }
        void operator=(hashentry a) { 
            zob = a.zob; 
            depth = a.depth;
            eval = a.eval;
            flags = a.flags;
            m = a.m;
        }
        U64 getZobrist() const { return zob; }
        int getDepth() const { return depth; }
        int getEval() const { return eval; }
        int getFlags() const { return flags; }
        move getMove() const { return m; }
        void setDepth(int d) { depth = d; }
    private:
        U64 zob;
        int depth;
        int eval;
        int flags;
        move m;
    };
}

#endif