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
			if (m[ply + i * SPACES] == cm)
			{
				++s[ply + i * SPACES];
				if (s[ply + primaryindex[ply]] < s[ply + i * SPACES])
					primaryindex[ply] = i;
				return;
			}
			else if (m[ply + i * SPACES] == NULLMOVE)
			{
				m[ply + i * SPACES] = cm;
				++s[ply + i * SPACES];
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
				m[i + j * SPACES] = m[i + 2 + j * SPACES];
				s[i + j * SPACES] = s[i + 2 + j * SPACES];
			}
		}
		for (int i = MAXDEPTH - 2; i < MAXDEPTH; ++i) 
		{
			primaryindex[i] = 0;
			for (int j = 0; j < MEMORY; ++j) 
			{
				m[i + j * SPACES] = NULLMOVE;
				s[i + j * SPACES] = 0;
			}
		}
	}
}