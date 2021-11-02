#include <cstring>
#include <algorithm>
#include "Killers.h"

namespace Hopper
{
	Killers::Killers() {
		memset(killerLimit, 0, sizeof(killerLimit));
	}

	void Killers::cutoff(Move cm, unsigned ply)
	{
		unsigned i = 0;
		while(i < killerLimit[ply]) {
			if (trackedKillers[ply][i].myMove == cm) {
				++trackedKillers[ply][i].score;
				std::sort(trackedKillers[ply], trackedKillers[ply] + killerLimit[ply], smScoreComp);
				return;
			}
			++i;
		}
		if (i < MEMORY) {
			trackedKillers[ply][i].myMove = cm;
			++trackedKillers[ply][i].score;
			++killerLimit[ply];
		}
	}

	void Killers::chrono()
	{
		memcpy(killerLimit, &killerLimit[2], sizeof(unsigned) * (MAXDEPTH - 2));
		memcpy(trackedKillers, &trackedKillers[2], sizeof(scoredMove) * MEMORY * (MAXDEPTH - 2));
		killerLimit[MAXDEPTH - 1] = 0;
		killerLimit[MAXDEPTH - 2] = 0;
		memset(trackedKillers[MAXDEPTH - 1], 0, sizeof(scoredMove) * MEMORY);
		memset(trackedKillers[MAXDEPTH - 2], 0, sizeof(scoredMove) * MEMORY);
	}
}