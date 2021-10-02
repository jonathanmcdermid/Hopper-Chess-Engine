#pragma once

#include "Hashentry.h"
#include "Board.h"

namespace Hopper
{
    typedef struct line {
        int moveCount = 0;
        Move moveLink[MAXDEPTH];
    }line;

    class HashTable
    {
    public:
        HashTable() { master = true; }
        void clean();
        void extractPV(Board* b, line* l);
        void newEntry(int index, Hashentry& h) { myHashTable[index] = h; }
        void newEntry(int index, U64 myZobrist, int d, int e, int f, Move b) { myHashTable[index] = Hashentry(myZobrist, d, e, f, b); }
        Hashentry getEntry(int index) const { return myHashTable[index]; }
        U64 getZobrist(int index) const { return myHashTable[index].getZobrist(); }
        Move getMove(int index) const { return myHashTable[index].getMove(); }
        int getDepth(int index) const { return myHashTable[index].getDepth(); }
        int getEval(int index) const { return myHashTable[index].getEval(); }
        int getFlags(int index) const { return myHashTable[index].getFlags(); }
    private:
        Hashentry myHashTable[HASHSIZE];
        bool master;
    };
}