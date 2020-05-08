#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "hashentry.h"
#include "board.h"

namespace Hopper {
    typedef struct line {
        int cmove = 0;
        move movelink[MAXDEPTH];
    }line;
    class hashtable {
    public:
        hashtable();
        void clean();
        void extractPV(board* b, line* l);
        void newEntry(int index, hashentry h) { table[index] = h; }
        U64 getZobrist(int index) const { return table[index].getZobrist(); }
        hashentry getEntry(int index) const { return table[index]; }
        move getMove(int index) const { return table[index].getMove(); }
        int getDepth(int index) const { return table[index].getDepth(); }
        int getEval(int index) const { return table[index].getEval(); }
        int getFlags(int index) const { return table[index].getFlags(); }
    private:
        hashentry table[HASHSIZE];
        bool master;
    };
}

#endif