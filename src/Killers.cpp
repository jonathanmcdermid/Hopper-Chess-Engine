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
		memset(trackedKillers, 0, sizeof(trackedKillers));
		memset(killerLimit, 0, sizeof(killerLimit));
	}
}