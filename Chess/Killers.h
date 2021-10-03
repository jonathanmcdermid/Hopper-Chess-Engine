#pragma once

#include "Move.h"

namespace Hopper
{
	struct trackedMove {
		unsigned appearances = 0;
		Move myMove = NULLMOVE;
		trackedMove(unsigned u = 0, Move k = NULLMOVE) {
			appearances = u;
			myMove = k;
		}
		void operator=(const trackedMove& rhs) {
			appearances = rhs.appearances;
			myMove = rhs.myMove;
		}
	};

	class Killers
	{
	public:
		Killers();
		Move getPrimary(int ply) const { return trackedKillers[ply][0].myMove; }
		Move getSecondary(int ply) const { return trackedKillers[ply][1].myMove; }
		void cutoff(Move cm, int ply);
		void chrono();
	private:
		trackedMove trackedKillers[MAXDEPTH][MEMORY];
		int index[MAXDEPTH];
	};
}