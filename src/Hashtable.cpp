#include <cstring>
#include "Hashtable.h"

namespace Hopper
{
    void HashTable::flush()
    {
        for (unsigned i = 0; i < myHashTable.size(); i += 2)
            myHashTable[i] = hashEntry();
        for (unsigned i = 0; i < myPawnHashTable.size(); i += 2)
            myPawnHashTable[i] = pawnHashEntry();
    }

    void HashTable::clean()
    {
        for (unsigned i = master; i < myHashTable.size(); i += 2)
            myHashTable[i].hashDepth = 0;
        master = !master;
    }

    void HashTable::setSize(unsigned bytes) {
        unsigned i;
        for (i = 31; i > 0; --i) {
            if (bytes & 1 << i)
                break;
        }
        unsigned j = 1 << (i - 5);
        unsigned k = 1 << (i - 6);
        myHashTable.resize(j);
        myPawnHashTable.resize(k);
    }

    hashEntry* HashTable::probe(U64 key, bool& found) {
        found = myHashTable[key % myHashTable.size()].hashZobristKey == key;
        return &myHashTable[key % myHashTable.size()];
    }
}