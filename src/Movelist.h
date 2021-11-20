#pragma once

#include "Board.h"

namespace Hopper
{
	struct Thread;

	class MoveList
	{
	public:
		MoveList(Thread* t, Move pv = NULLMOVE, Move primary = NULLMOVE, Move secondary = NULLMOVE);
		void moveOrder(unsigned genState);
		void MVVLVA();
		void scoreQuiets();
		void removeDuplicate(scoredMove m);
		bool movesLeft();
		Move getCurrMove();
		void increment();
		bool rememberQuiets(Move& m);
		bool SEEcontrol();
		void updateHiddenAttackers(int from, int to);
		void handleEnPassant(int from, int to);
	private:
		Thread* myThread;
		scoredMove storedMoves[MEMORY];
		scoredMove pvMove;
		scoredMove primaryMove;
		scoredMove secondaryMove;
		bool playSpecial;
		unsigned index;
		unsigned memoryIndex;
		unsigned limit;
		unsigned generationState;
		int attackers[WIDTH * 2];
		int attackerValues[WIDTH * 2];
		unsigned total[2];
	};
}