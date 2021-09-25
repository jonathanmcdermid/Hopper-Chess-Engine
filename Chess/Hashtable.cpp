#include "Hashtable.h"

namespace Hopper 
{
    Hashtable::Hashtable() 
    {
        for (int i = 0; i < HASHSIZE; ++i) 
            table[i] = Hashentry();
        master = true;
    }

    void Hashtable::clean() 
    {
        for (int i = (int) master; i < HASHSIZE; i += 2) 
            table[i].setDepth(0);
        master != master;
    }

    void Hashtable::extractPV(Board* b, line* l) 
    {
        Move m;
        int index = 0;
        int depth = getDepth(b->getCurrZ() % HASHSIZE);
        do 
        {
            m = getMove(b->getCurrZ() % HASHSIZE);
            if (b->getCurrZ() != getZobrist(b->getCurrZ() % HASHSIZE) || !b->validateMove(m)) 
                break;
            l->movelink[index++] = m;
            b->movePiece(m);
        } while (--depth);
        l->cmove = index;
        for (int i = 0; i < index; ++i) 
            b->unmovePiece();
    }
}