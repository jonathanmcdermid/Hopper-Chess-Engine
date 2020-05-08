#include "killers.h"

namespace Hopper {
	killers::killers() {
		max = 0;
		for (int i = 0; i < MAXDEPTH; ++i) {
			primaryindex[i] = 0;
			for (int j = 0; j < MEMORY; ++j) {
				m[i][j] = NULLMOVE;
				s[i][j] = 0;
			}
		}
	}

	void killers::cutoff(move cm, int ply) {
		int i = 0;
		do {
			if (m[ply][i] == cm) {
				++s[ply][i];
				if (s[ply][primaryindex[ply]] < s[ply][i]) { primaryindex[ply] = i; }
				return;
			}
			else if (m[ply][i] == NULLMOVE) {
				m[ply][i] = cm;
				++s[ply][i];
				return;
			}
		} while (++i < MEMORY);
	}

	void killers::chrono() {
		for (int i = 0; i < MAXDEPTH - 2; ++i) {
			primaryindex[i] = primaryindex[i + 2];
			for (int j = 0; j < MEMORY; ++j) {
				m[i][j] = m[i + 2][j];
				s[i][j] = s[i + 2][j];
			}
		}
		for (int i = MAXDEPTH - 2; i < MAXDEPTH; ++i) {
			primaryindex[i] = 0;
			for (int j = 0; j < MEMORY; ++j) {
				m[i][j] = NULLMOVE;
				s[i][j] = 0;
			}
		}
	}
}