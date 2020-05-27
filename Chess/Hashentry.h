#pragma once

#include "Move.h"

namespace Hopper {
    class Hashentry {
    public:
        Hashentry() { zob = 0; depth = 0; eval = 0; flags = 0; m = NULLMOVE; }
        Hashentry(U64 z, int d, int e, int f, Move b) { zob = z; depth = d; eval = e; flags = f; m = b; }
        void operator=(Hashentry a) { zob = a.zob; depth = a.depth; eval = a.eval; flags = a.flags; m = a.m; }
        U64 getZobrist() const { return zob; }
        int getDepth() const { return depth; }
        int getEval() const { return eval; }
        int getFlags() const { return flags; }
        Move getMove() const { return m; }
        void setDepth(int d) { depth = d; }
    private:
        U64 zob;
        int depth;
        int eval;
        int flags;
        Move m;
    };
}