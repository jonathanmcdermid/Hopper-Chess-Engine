#include "Killers.h"

namespace Hopper 
{
	Killers::Killers() 
	{
		max = 0;
		for (int i = 0; i < MAXDEPTH * MEMORY; ++i) 
		{
			m[i] = NULLMOVE;
			s[i] = 0;
		}
	}

	void Killers::cutoff(Move cm, int ply) 
	{
		int i = 0;
		do 
		{
			if (m[ply + i * MAXDEPTH] == cm)
			{
				++s[ply + i * MAXDEPTH];
				if (s[ply + primaryindex[ply]] < s[ply + i * MAXDEPTH])
					primaryindex[ply] = i;
				return;
			}
			else if (m[ply + i * MAXDEPTH] == NULLMOVE)
			{
				m[ply + i * MAXDEPTH] = cm;
				++s[ply + i * MAXDEPTH];
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
				m[i + j * MAXDEPTH] = m[i + 2 + j * MAXDEPTH];
				s[i + j * MAXDEPTH] = s[i + 2 + j * MAXDEPTH];
			}
		}
		for (int i = MAXDEPTH - 2; i < MAXDEPTH; ++i) 
		{
			primaryindex[i] = 0;
			for (int j = 0; j < MEMORY; ++j) 
			{
				m[i + j * MAXDEPTH] = NULLMOVE;
				s[i + j * MAXDEPTH] = 0;
			}
		}
	}
}