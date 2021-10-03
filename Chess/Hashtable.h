#pragma once

#include "Hashentry.h"
#include "Board.h"
#include <vector>

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
        void setSize(unsigned bytes) {
            unsigned i;
            for (i = 31; i > 0; --i) {
                if (bytes & 1 << i)
                    break;
            }
            i = 1 << i;
            myHashTable.resize((unsigned) (i / 32));
            myPawnHashTable.resize((unsigned) (i / 32));
        }
        size_t getSize() { return myHashTable.size(); }
        size_t getPawnSize() { return myPawnHashTable.size(); }
        void clean();
        void extractPV(Board* b, line* l);
        void newEntry(int index, Hashentry& h) { myHashTable[index] = h; }
        void newEntry(int index, U64 myZobrist, int d, int e, int f, Move b) { myHashTable[index] = Hashentry(myZobrist, d, e, f, b); }
        Hashentry getEntry(int index) const { return myHashTable[index]; }
        void newPawnEntry(int index, int hashEval) { myPawnHashTable[index] = hashEval; }
        int getPawnEntry(int index) const { return myPawnHashTable[index]; }
        U64 getZobrist(int index) const { return myHashTable[index].getZobrist(); }
        Move getMove(int index) const { return myHashTable[index].getMove(); }
        int getDepth(int index) const { return myHashTable[index].getDepth(); }
        int getEval(int index) const { return myHashTable[index].getEval(); }
        int getFlags(int index) const { return myHashTable[index].getFlags(); }
    private:
        std::vector<Hashentry> myHashTable;
        std::vector<int> myPawnHashTable;
        bool master;
    };
}