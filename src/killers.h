#ifndef KILLERS_H
#define KILLERS_H

#include "move.h"

namespace Chess {
	class killers {
	public:
		killers();
		move getPrimary(int ply) const { return m[ply][primaryindex[ply]]; }
		void cutoff(move cm, int ply);
		void chrono();
	private:
		move m[MAXDEPTH][MEMORY];
		int primaryindex[MAXDEPTH];
		int s[MAXDEPTH][MEMORY];
		int max;
	};
}
#endif