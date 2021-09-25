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
			if (m[ply + (int) (i << 6)] == cm)
			{
				++s[ply + (int) (i << 6)];
				if (s[ply + primaryindex[ply]] < s[ply + (int) (i << 6)])
					primaryindex[ply] = i;
				return;
			}
			else if (m[ply + (int) (i << 6)] == NULLMOVE)
			{
				m[ply + (int) (i << 6)] = cm;
				++s[ply + (int) (i << 6)];
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
				m[i + (int) (j << 6)] = m[i + 2 + (int) (j << 6)];
				s[i + (int) (j << 6)] = s[i + 2 + (int) (j << 6)];
			}
		}
		for (int i = MAXDEPTH - 2; i < MAXDEPTH; ++i) 
		{
			primaryindex[i] = 0;
			for (int j = 0; j < MEMORY; ++j) 
			{
				m[i + (int) (j << 6)] = NULLMOVE;
				s[i + (int) (j << 6)] = 0;
			}
		}
	}
}