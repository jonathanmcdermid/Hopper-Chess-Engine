#include "Killers.h"
#include <string>

namespace Hopper
{
	Killers::Killers()
	{
		memset(primaryindex, 0, sizeof(primaryindex));
		memset(killerMoveScores, 0, sizeof(killerMoveScores));
	}

	void Killers::cutoff(Move cm, int ply)
	{
		int i = 0;
		do {
			if (killerMoves[ply][i] == cm) {
				++killerMoveScores[ply][i];
				if (killerMoveScores[ply][primaryindex[ply]] < killerMoveScores[ply][i])
					primaryindex[ply] = i;
				return;
			}
			else if (killerMoves[ply][i] == NULLMOVE) {
				killerMoves[ply][i] = cm;
				++killerMoveScores[ply][i];
				return;
			}
		} while (++i < MEMORY);
	}

	void Killers::chrono()
	{
		for (int i = 0; i < MAXDEPTH - 2; ++i) {
			primaryindex[i] = primaryindex[i + 2];
			for (int j = 0; j < MEMORY; ++j) {
				killerMoves[i][j] = killerMoves[i + 2][j];
				killerMoveScores[i][j] = killerMoveScores[i + 2][j];
			}
		}
		primaryindex[MAXDEPTH - 1] = 0;
		primaryindex[MAXDEPTH - 2] = 0;
		memset(killerMoveScores[MAXDEPTH - 1], 0, sizeof(killerMoveScores[MAXDEPTH - 1]));
		memset(killerMoveScores[MAXDEPTH - 2], 0, sizeof(killerMoveScores[MAXDEPTH - 2]));
		for (int j = 0; j < MEMORY; ++j) {
			killerMoves[MAXDEPTH - 1][j] = NULLMOVE;
			killerMoves[MAXDEPTH - 2][j] = NULLMOVE;
		}
	}
}