#include <cstring>
#include <algorithm>
#include "Killers.h"

namespace Hopper
{

	inline static bool killerSort(trackedMove const& lhs, trackedMove const& rhs) {
		return lhs.appearances > rhs.appearances;
	}

	void Killers::cutoff(Move cm, unsigned ply)
	{
		unsigned i = 0;
		while(i < killerIndex[ply]) {
			if (trackedKillers[ply][i].myMove == cm) {
				++trackedKillers[ply][i].appearances;
				std::sort(trackedKillers[ply], trackedKillers[ply] + killerIndex[ply], killerSort);
				return;
			}
			++i;
		}
		if (i < MEMORY) {
			trackedKillers[ply][i].myMove = cm;
			++trackedKillers[ply][i].appearances;
			++killerIndex[ply];
		}
	}

	void Killers::chrono()
	{
		for (unsigned i = 0; i < MAXDEPTH - 2; ++i) {
			killerIndex[i] = killerIndex[i + 2];
			for (unsigned j = 0; j < MEMORY; ++j) {
				trackedKillers[i][j] = trackedKillers[i + 2][j];
			}
		}
		killerIndex[MAXDEPTH - 1] = 0;
		killerIndex[MAXDEPTH - 2] = 0;
		memset(trackedKillers[MAXDEPTH - 1], 0, sizeof(trackedKillers[MAXDEPTH - 1]));
		memset(trackedKillers[MAXDEPTH - 2], 0, sizeof(trackedKillers[MAXDEPTH - 2]));
	}
}