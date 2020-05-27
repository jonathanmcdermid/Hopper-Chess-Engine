#pragma once

#include "Hashentry.h"
#include "Board.h"

namespace Hopper {
    typedef struct line {
        int cmove = 0;
        Move movelink[MAXDEPTH];
    }line;
    class Hashtable {
    public:
        Hashtable();
        void clean();
        void extractPV(Board* b, line* l);
        void newEntry(int index, Hashentry h) { table[index] = h; }
        U64 getZobrist(int index) const { return table[index].getZobrist(); }
        Hashentry getEntry(int index) const { return table[index]; }
        Move getMove(int index) const { return table[index].getMove(); }
        int getDepth(int index) const { return table[index].getDepth(); }
        int getEval(int index) const { return table[index].getEval(); }
        int getFlags(int index) const { return table[index].getFlags(); }
    private:
        Hashentry table[HASHSIZE];
        bool master;
    };
}