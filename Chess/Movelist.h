#pragma once

#include "board.h"
#include "macros.h"
#include "move.h"

namespace Hopper
{
	class MoveList
	{
	public:
		MoveList(Board* bd, Move pv = NULLMOVE, Move hash = NULLMOVE, Move primary = NULLMOVE, Move secondary = NULLMOVE);
		void moveOrder(int genState);
		bool staticExchange(Move myMove, int threshold);
		void removeDuplicate(int gs);
		void staticSort();
		bool movesLeft()const { return index[generationState] < limit[generationState]; }
		bool noMoves()const;
		Move getCurrMove()const { return sortedMoves[generationState][index[generationState]]; }
		void increment() { ++index[generationState]; }
	private:
		Board* myBoard;
		Move sortedMoves[GENEND][SPACES];
		int index[GENEND];
		int limit[GENEND];
		int generationState;
	};
}