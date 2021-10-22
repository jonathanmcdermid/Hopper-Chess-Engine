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
		for (unsigned i = 0; i < MAXDEPTH - 2; ++i) {
			killerLimit[i] = killerLimit[i + 2];
			for (unsigned j = 0; j < MEMORY; ++j) {
				trackedKillers[i][j] = trackedKillers[i + 2][j];
			}
		}
		killerLimit[MAXDEPTH - 1] = 0;
		killerLimit[MAXDEPTH - 2] = 0;
		memset(trackedKillers[MAXDEPTH - 1], 0, sizeof(trackedKillers[MAXDEPTH - 1]));
		memset(trackedKillers[MAXDEPTH - 2], 0, sizeof(trackedKillers[MAXDEPTH - 2]));
	}
}