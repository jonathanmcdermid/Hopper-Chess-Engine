#include "Hashtable.h"

namespace Hopper 
{
    void HashTable::clean() 
    {
        for (int i = (int) master; i < HASHSIZE; i += 2) 
            myHashTable[i].setDepth(0);
        master = !master;
    }

    void HashTable::extractPV(Board* b, line* l) 
    {
        Move nextMove;
        int index = 0;
        int depth = getDepth(b->getCurrZ() % HASHSIZE);
        do 
        {
            nextMove = getMove(b->getCurrZ() % HASHSIZE);
            if (b->getCurrZ() != getZobrist(b->getCurrZ() % HASHSIZE) || !b->validateMove(nextMove)) 
                break;
            l->moveLink[index++] = nextMove;
            b->movePiece(nextMove);
        } while (--depth);
        l->moveCount = index;
        while(index--)
            b->unmovePiece();
    }
}