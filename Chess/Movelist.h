#pragma once

#include "macros.h"
#include "move.h"

namespace Hopper {
	class Board;
	class MoveList {
	public:
		MoveList(Board* bd, Move pv, Move hash, Move killer);
		MoveList(Board* bd);
		void moveOrder(int genstate);
		bool staticExchange(Move m, int threshold);
		bool movesLeft() { return index[state] < limit[state]; }
		bool noMoves();
		Move getCurrMove() { return moves[state][index[state]]; }
		void increment() { ++index[state]; }
	private:
		Board* b;
		Move moves[GENEND][SPACES] = { 0 };
		int index[GENEND] = { 0 };
		int limit[GENEND] = { 0 };
		int state;
	};
}