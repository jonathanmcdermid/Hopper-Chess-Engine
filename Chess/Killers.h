#pragma once

#include "Move.h"

namespace Hopper 
{
	class Killers 
	{
	public:
		Killers();
		~Killers() { ; }
		Move getPrimary(int ply) const { return m[ply + primaryindex[ply] * MAXDEPTH]; }
		void cutoff(Move cm, int ply);
		void chrono();
	private:
		Move m[MAXDEPTH * MEMORY];
		int primaryindex[MAXDEPTH] = { 0 };
		int s[MAXDEPTH * MEMORY] = { 0 };
		int max = 0;
	};
}