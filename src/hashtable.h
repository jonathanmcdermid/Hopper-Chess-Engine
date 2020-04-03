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
            for (int i = 0; i < HASHSIZE; ++i) { table[i] = hashentry(); } 
            master = true;
        }
        void clean() {
            for (int i = (master) ? 1 : 0; i < HASHSIZE; i += 2) { table[i] = hashentry(); }
            master = (master) ? false : true;
        }
        void newEntry(int index, hashentry h) { table[index] = h; }
        U64 getZobrist(int index)const { return table[index].getZobrist(); }
        move getMove(int index)const { return table[index].getMove(); }
        int getDepth(int index)const { return table[index].getDepth(); }
    private:
        hashentry table[HASHSIZE];
        bool master;
    };
}

#endif