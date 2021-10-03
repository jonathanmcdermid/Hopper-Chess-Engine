#include <string>
#include <algorithm>
#include "Killers.h"

namespace Hopper
{
	Killers::Killers()
	{
		memset(index, 0, sizeof(index));
	}

	inline static bool killerSort(trackedMove const& lhs, trackedMove const& rhs) {
		return lhs.appearances > rhs.appearances;
	}

	void Killers::cutoff(Move cm, int ply)
	{
		int i = 0;
		while(i < index[ply]) {
			if (trackedKillers[ply][i].myMove == cm) {
				++trackedKillers[ply][i].appearances;
				std::sort(trackedKillers[ply], trackedKillers[ply] + index[ply], killerSort);
				return;
			}
			++i;
		}
		if (i < MEMORY) {
			trackedKillers[ply][i].myMove = cm;
			++trackedKillers[ply][i].appearances;
			++index[ply];
		}
	}

	void Killers::chrono()
	{
		for (int i = 0; i < MAXDEPTH - 2; ++i) {
			index[i] = index[i + 2];
			for (int j = 0; j < MEMORY; ++j) {
				trackedKillers[i][j] = trackedKillers[i + 2][j];
			}
		}
		index[MAXDEPTH - 1] = 0;
		index[MAXDEPTH - 2] = 0;
		memset(trackedKillers[MAXDEPTH - 1], 0, sizeof(trackedKillers[MAXDEPTH - 1]));
		memset(trackedKillers[MAXDEPTH - 2], 0, sizeof(trackedKillers[MAXDEPTH - 2]));
	}
}