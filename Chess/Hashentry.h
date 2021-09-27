#pragma once

#include "Move.h"

namespace Hopper 
{
    class Hashentry 
    {
    public:
        Hashentry() { refresh(); }
        void refresh() { hashZobristKey = 0; hashDepth = 0; hashEval = 0; hashFlags = 0; hashentryMove = NULLMOVE; }
        Hashentry(U64 myZobrist, int d, int e, int f, Move b) { hashZobristKey = myZobrist; hashDepth = d; hashEval = e; hashFlags = f; hashentryMove = b; }
        void operator=(Hashentry a) { hashZobristKey = a.hashZobristKey; hashDepth = a.hashDepth; hashEval = a.hashEval; hashFlags = a.hashFlags; hashentryMove = a.hashentryMove; }
        U64 getZobrist() const { return hashZobristKey; }
        int getDepth() const { return hashDepth; }
        int getEval() const { return hashEval; }
        int getFlags() const { return hashFlags; }
        Move getMove() const { return hashentryMove; }
        void setDepth(int d) { hashDepth = d; }
    private:
        U64 hashZobristKey;
        int hashDepth;
        int hashEval;
        int hashFlags;
        Move hashentryMove;
    };
}