#ifndef PAWNHASH_H
#define PAWNHASH_H

#include "move.h"

namespace Hopper {
	class pawnhash {
	public:
		pawnhash() { for (int i = 0; i < HASHSIZE; ++i) { hashtable[i] = 0; } }
		void newEntry(int index, int eval) { hashtable[index] = eval; }
		int getEntry(int index) { return hashtable[index]; }
	private:
		int hashtable[HASHSIZE];
	};
}
#endif