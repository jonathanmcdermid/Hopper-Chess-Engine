#ifndef BOT_H
#define BOT_H

#include "hashtable.h"

namespace Chess {
	class board;
	typedef struct line {
		uint8_t cmove;
		move movelink[SURFACE];
	}line;
	class bot{
	public:
		bot();
		move getMove(board* b);
		void setwTime(uint32_t i) { wtime = i; }
		void setbTime(uint32_t i) { btime = i; }
		int16_t miniMax(board* b, uint8_t depth, int16_t alpha, int16_t beta, line* pline, bool notNull);
		static int16_t qSearch(board* b, int16_t alpha, int16_t beta);
	private:
		hashtable table[HASHSIZE];
		uint32_t wtime;
		uint32_t btime;
	};
}

#endif
