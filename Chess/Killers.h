#pragma once

#include "Move.h"

namespace Hopper 
{
	class Killers 
	{
	public:
		Killers();
		Move getPrimary(int ply) const { return m[ply + primaryindex[ply] * MAXDEPTH]; }
		void cutoff(Move cm, int ply);
		void chrono();
	private:
		Move m[MAXDEPTH * MEMORY];
		int primaryindex[MAXDEPTH];
		int s[MAXDEPTH * MEMORY];
		int max;
	};
}