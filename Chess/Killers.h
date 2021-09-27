#pragma once

#include "Move.h"

namespace Hopper 
{
	class Killers 
	{
	public:
		Killers();
		Move getPrimary(int ply) const { return killerMoves[ply + primaryindex[ply] * MAXDEPTH]; }
		void cutoff(Move cm, int ply);
		void chrono();
	private:
		Move killerMoves[MAXDEPTH * MEMORY];
		int primaryindex[MAXDEPTH];
		int killerMoveScores[MAXDEPTH * MEMORY];
	};
}