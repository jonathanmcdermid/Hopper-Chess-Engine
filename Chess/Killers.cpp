#include "Killers.h"

namespace Hopper 
{
	Killers::Killers() 
	{
		for (int i = 0; i < MAXDEPTH * MEMORY; ++i) 
		{
			killerMoves[i] = NULLMOVE;
			killerMoveScores[i] = 0;
		}
	}

	void Killers::cutoff(Move cm, int ply) 
	{
		int i = 0;
		do 
		{
			if (killerMoves[ply + i * SPACES] == cm)
			{
				++killerMoveScores[ply + i * SPACES];
				if (killerMoveScores[ply + primaryindex[ply]] < killerMoveScores[ply + i * SPACES])
					primaryindex[ply] = i;
				return;
			}
			else if (killerMoves[ply + i * SPACES] == NULLMOVE)
			{
				killerMoves[ply + i * SPACES] = cm;
				++killerMoveScores[ply + i * SPACES];
				return;
			}
		} while (++i < MEMORY);
	}

	void Killers::chrono() 
	{
		for (int i = 0; i < MAXDEPTH - 2; ++i) 
		{
			primaryindex[i] = primaryindex[i + 2];
			for (int j = 0; j < MEMORY; ++j) 
			{
				killerMoves[i + j * SPACES] = killerMoves[i + 2 + j * SPACES];
				killerMoveScores[i + j * SPACES] = killerMoveScores[i + 2 + j * SPACES];
			}
		}
		for (int i = MAXDEPTH - 2; i < MAXDEPTH; ++i) 
		{
			primaryindex[i] = 0;
			for (int j = 0; j < MEMORY; ++j) 
			{
				killerMoves[i + j * SPACES] = NULLMOVE;
				killerMoveScores[i + j * SPACES] = 0;
			}
		}
	}
}