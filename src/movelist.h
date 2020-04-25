#ifndef MOVELIST_H
#define MOVELIST_H

#include "move.h"

namespace Chess {
	class board;
	class movelist {
	public:
		movelist(board* bd, move pv, move hash, move killer);
		movelist(board* bd);
		void moveOrder(int genstate);
		bool staticExchange(move m, int threshold);
		bool movesLeft() { return index[state] < limit[state]; }
		bool noMoves();
		move getCurrMove() { return moves[state][index[state]]; }
		void increment() { ++index[state]; }
	private:
		board* b;
		move moves[GENEND][SPACES];
		int index[GENEND];
		int limit[GENEND];
		int state;
	};
}
#endif