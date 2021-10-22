#pragma once

#include "Board.h"

namespace Hopper
{
	class MoveList
	{
	public:
		MoveList(Board* bd, Move pv = NULLMOVE, Move hash = NULLMOVE, Move primary = NULLMOVE, Move secondary = NULLMOVE);
		void moveOrder(unsigned genState);
		bool staticExchange(Move myMove);
		void MVVLVA();
		void removeDuplicate(unsigned gs);
		bool movesLeft()const { return index < limit[generationState]; }
		bool noMoves()const;
		Move getCurrMove()const { return storedMoves[generationState][index].myMove; }
		void increment();
	private:
		Board* myBoard;
		scoredMove storedMoves[GENEND][SPACES];
		unsigned index;
		unsigned limit[GENEND];
		unsigned generationState;
	};
}