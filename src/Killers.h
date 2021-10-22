#pragma once

#include "Move.h"

namespace Hopper
{
	class Killers
	{
	public:
		Killers();
		Move getPrimary(unsigned ply) const { return trackedKillers[ply][0].myMove; }
		Move getSecondary(unsigned ply) const { return trackedKillers[ply][1].myMove; }
		void cutoff(Move cm, unsigned ply);
		void chrono();
	private:
		scoredMove trackedKillers[MAXDEPTH][MEMORY];
		unsigned killerLimit[MAXDEPTH];
	};
}