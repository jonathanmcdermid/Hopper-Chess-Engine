#ifndef KILLERS_H
#define KILLERS_H

#include "move.h"

namespace Chess {
	class killers {
	public:
		killers() {
			for (int i = 0; i < MAXDEPTH; ++i) {
				primaryindex[i] = 0;
				for (int j = 0; j < MEMORY; ++j) {
					m[i][j] = move();
					s[i][j] = 0;
				}
			}
		}
		move getPrimary(int ply) const{ return m[ply][primaryindex[ply]]; }
		void cutoff(move cm, int ply){
			int i = 0;
			do {
				if (m[ply][i] == cm) {
					++s[ply][i];
					if (s[ply][primaryindex[ply]] < s[ply][i]) { primaryindex[ply] = i; }
					return;
				}
				else if(m[ply][i] == move()){
					m[ply][i] = cm;
					++s[ply][i];
					return;
				}
				++i;
			} while (i < MEMORY);
		}
		void chrono() {
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
					m[i][j] = move();
					s[i][j] = 0;
				}
			}
		}
	private:
		int primaryindex[MAXDEPTH] = { 0 };
		move m[MAXDEPTH][MEMORY];
		int s[MAXDEPTH][MEMORY];
		int max = 0;
	};
}
#endif