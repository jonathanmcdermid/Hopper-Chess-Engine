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
        hashEntry(U64 k = 0, int d = 0, int e = 0, int f = 0, Move m = NULLMOVE) {
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
        int hashDepth;
        int hashFlags;
        int hashEval;
        Move hashMove;
    }hashEntry;

    typedef struct pawnHashEntry {
        pawnHashEntry(U64 k = 0, int e = 0) {
            pawnHashZobristKey = k;
            pawnHashEval = e;
        }
        void operator=(const pawnHashEntry& rhs) {
            pawnHashZobristKey = rhs.pawnHashZobristKey;
            pawnHashEval = rhs.pawnHashEval;
        }
        U64 pawnHashZobristKey;
        int pawnHashEval;
    }pawnHashEntry;

    class HashTable
    {
    public:
        HashTable() { master = true; }
        void setSize(unsigned bytes);
        void clean();
        void newEntry(U64 key, int d, int e, int f, Move b) { myHashTable[key % myHashTable.size()] = hashEntry(key, d, e, f, b); }
        void newEntry(U64 key, hashEntry& h) { myHashTable[key % myHashTable.size()] = h; }
        void newPawnEntry(U64 key, int hashEval){ myPawnHashTable[key % myPawnHashTable.size()] = pawnHashEntry(key, hashEval); }
        int getPawnEval(U64 key)    const { return myPawnHashTable[key % myPawnHashTable.size()].pawnHashEval; }
        U64 getPawnZobrist(U64 key) const { return myPawnHashTable[key % myPawnHashTable.size()].pawnHashZobristKey; }
        hashEntry getEntry(U64 key) const { return myHashTable[key % myHashTable.size()]; }
        U64 getZobrist(U64 key)     const { return myHashTable[key % myHashTable.size()].hashZobristKey; }
        Move getMove(U64 key)       const { return myHashTable[key % myHashTable.size()].hashMove; }
        int getDepth(U64 key)       const { return myHashTable[key % myHashTable.size()].hashDepth; }
        int getEval(U64 key)        const { return myHashTable[key % myHashTable.size()].hashEval; }
        int getFlags(U64 key)       const { return myHashTable[key % myHashTable.size()].hashFlags; }
    private:
        std::vector<hashEntry> myHashTable;
        std::vector<pawnHashEntry> myPawnHashTable;
        bool master;
    };
}