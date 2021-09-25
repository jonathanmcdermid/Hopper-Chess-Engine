#pragma once

#include "board.h"
#include "macros.h"
#include "move.h"

namespace Hopper 
{
	class MoveList 
	{
	public:
		MoveList(Board* bd, Move pv, Move hash, Move killer);
		MoveList(Board* bd);
		~MoveList() { ; }
		void moveOrder(int genstate);
		bool staticExchange(Move m, int threshold);
		bool movesLeft() const { return index[state] < limit[state]; }
		const bool noMoves();
		Move getCurrMove() const { return moves[state][index[state]]; }
		void increment() { ++index[state]; }
	private:
		Board* b;
		Move moves[GENEND][SPACES];
		int index[GENEND];
		int limit[GENEND];
		int state;
	};
}