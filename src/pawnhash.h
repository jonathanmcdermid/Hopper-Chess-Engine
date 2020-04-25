#ifndef PAWNHASH_H
#define PAWNHASH_H

#include "move.h"

namespace Chess {
	class pawnhash {
	public:
		pawnhash() { for (int i = 0; i < HASHSIZE; ++i) { hashtable[i] = 0; } }
		void recordPawnEval(long zobrist, int evalWhite, int evalBlack, int passers) {
			int hashkey = (zobrist % HASHSIZE) * slots;
			hashtable[hashkey] = 0 | (evalWhite + 0x3FFF) | ((evalBlack + 0x3FFF) << 16);
			hashtable[hashkey + 1] = passers;
			hashtable[hashkey + 2] = (int)(zobrist >> 32);
		}
		int probePawnEval(U64 zobrist) {
			int hashkey = (zobrist % HASHSIZE) * slots;
			if (hashtable[hashkey + 2] == ((int)(zobrist >> 32))) {
				//Evaluation.passers = hashtable[hashkey + 1];
				return (hashtable[hashkey]);
			}
			return EMPTY;
		}
	private:
		int hashtable[HASHSIZE];
		int slots = 3;
	};

	class evalhash {
	public:
		evalhash() { for (int i = 0; i < HASHSIZE; ++i) { hashtable[i] = 0; } }
		void recordEval(U64 zobrist, int eval) {
			int hashkey = (zobrist % HASHSIZE) * slots;
			hashtable[hashkey] = (eval + 0x1FFFF);
			hashtable[hashkey + 1] = (int)(zobrist >> 32);
		}
		int probeEval(long zobrist) {
			int hashkey = (zobrist % HASHSIZE) * slots;
			if (hashtable[hashkey + 1] == zobrist >> 32) { return (hashtable[hashkey] - 0x1FFFF); }
			return EMPTY;
		}
	private:
		int hashtable[HASHSIZE];
		int slots = 2;
	};
}
#endif