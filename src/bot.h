#ifndef BOT_H
#define BOT_H

#include "hashtable.h"

namespace Chess {
	class board;
	typedef struct line {
		uint8_t cmove;
		move movelink[MAXDEPTH];
	}line;
	typedef struct limits {
		 uint32_t time[2];
		 uint32_t inc[2];
		 uint8_t movesleft;
		 uint8_t depth;
		 uint8_t nodes;
	}limits;
	class bot{
	public:
		bot();
		move getMove(board* b);
		int16_t miniMax(board* b, uint8_t depth, int16_t alpha, int16_t beta, line* pline, bool notNull);
		static int16_t qSearch(board* b, int16_t alpha, int16_t beta);
		limits lim;
	private:
		hashtable table[HASHSIZE];
	};
}

#endif
