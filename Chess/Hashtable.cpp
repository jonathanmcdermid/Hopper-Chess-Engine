#include "Hashtable.h"

namespace Hopper
{
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
        i = 1 << (i - 5);
        myHashTable.resize(i);
        myPawnHashTable.resize(i);
    }

    void HashTable::extractPV(Board* b, line* l)
    {
        Move nextMove;
        unsigned index = 0;
        unsigned depth = getDepth(b->getCurrZ() % myHashTable.size());
        do {
            nextMove = getMove(b->getCurrZ() % myHashTable.size());
            if (b->getCurrZ() != getZobrist(b->getCurrZ() % myHashTable.size()) || !b->validateMove(nextMove))
                break;
            l->moveLink[index++] = nextMove;
            b->movePiece(nextMove);
        } while (--depth);
        l->moveCount = index;
        while (index--)
            b->unmovePiece();
    }
}