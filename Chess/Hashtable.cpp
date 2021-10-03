#include "Hashtable.h"

namespace Hopper
{
    void HashTable::clean()
    {
        for (int i = (int)master; i < myHashTable.size(); i += 2)
            myHashTable[i].setDepth(0);
        master = !master;
    }

    void HashTable::extractPV(Board* b, line* l)
    {
        Move nextMove;
        int index = 0;
        int depth = getDepth(b->getCurrZ() % myHashTable.size());
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