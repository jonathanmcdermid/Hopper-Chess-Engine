#pragma once

#include <vector>
#include "Board.h"

namespace Hopper
{
    typedef struct line {
        unsigned moveCount = 0;
        Move moveLink[MAXDEPTH];
    }line;

    typedef struct hashEntry {
        hashEntry(U64 k = 0, unsigned d = 0, int e = 0, unsigned f = 0, Move m = NULLMOVE) {
            hashZobristKey = k;
            hashDepth = d;
            hashEval = e;
            hashFlags = f;
            hashMove = m;
        }
        void operator=(const hashEntry& rhs) {
            hashZobristKey = rhs.hashZobristKey;
            hashDepth = rhs.hashDepth;
            hashEval = rhs.hashEval;
            hashFlags = rhs.hashFlags;
            hashMove = rhs.hashMove;
        }
        U64 hashZobristKey;
        unsigned hashDepth;
        unsigned hashFlags;
        int hashEval;
        Move hashMove;
    }hashEntry;

    class HashTable
    {
    public:
        HashTable() { master = true; }
        void setSize(unsigned bytes);
        void clean();
        void extractPV(Board* b, line* l);
        void newEntry(U64 key, hashEntry& h) { myHashTable[key % myHashTable.size()] = h; }
        void newEntry(U64 key, U64 myZobrist, int d, int e, int f, Move b) { myHashTable[key % myHashTable.size()] = hashEntry(myZobrist, d, e, f, b); }
        hashEntry getEntry(U64 key) const { return myHashTable[key % myHashTable.size()]; }
        void newPawnEntry(U64 key, int hashEval) { myPawnHashTable[key % myHashTable.size()] = hashEval; }
        int getPawnEntry(U64 key) const { return myPawnHashTable[key % myPawnHashTable.size()]; }
        U64 getZobrist(U64 key) const { return myHashTable[key % myPawnHashTable.size()].hashZobristKey; }
        Move getMove(U64 key) const { return myHashTable[key % myPawnHashTable.size()].hashMove; }
        unsigned getDepth(U64 key) const { return myHashTable[key % myPawnHashTable.size()].hashDepth; }
        int getEval(U64 key) const { return myHashTable[key % myPawnHashTable.size()].hashEval; }
        unsigned getFlags(U64 key) const { return myHashTable[key % myPawnHashTable.size()].hashFlags; }
    private:
        std::vector<hashEntry> myHashTable;
        std::vector<int> myPawnHashTable;
        bool master;
    };
}