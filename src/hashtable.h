#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "hashentry.h"
#include "board.h"

namespace Chess {
    typedef struct line {
        int cmove = 0;
        move movelink[MAXDEPTH];
    }line;
    class hashtable {
    public:
        hashtable() {
            master = true;
        }
        void clean() {
            for (int i = master; i < HASHSIZE; i += 2) { table[i].setDepth(0); }
            master = !master;
        }
        void extractPV(board* b, line* l) {
            move m;
            int index = 0;
            for (int i = 0; i < MAXDEPTH; ++i) {
                m = getMove(b->currZ % HASHSIZE);
                if (!b->validateMove(m)) { break; }
                l->movelink[index++] = m;
                b->movePiece(m);
            }
            //l->cmove = index;
            for (int i = 0; i < index; ++i) { b->unmovePiece(); }
        }
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