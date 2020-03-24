#ifndef BOT_H
#define BOT_H

#include "hashtable.h"

namespace Chess {
	class board;
	typedef struct line {
		uint8_t cmove = 0;
		move movelink[MAXDEPTH];
	}line;
	typedef struct limits {
		uint32_t time[2] = { 300000, 300000 };
		uint32_t inc[2] = { 100000, 100000 };
		uint32_t movetime = 0;
		uint8_t movesleft = 0;
		uint8_t depth = MAXDEPTH;
		uint8_t nodes = 0;
		uint8_t perft = 0;
		uint8_t mate = 0;
		bool infinite = false;
	}limits;
	typedef struct options {
		uint16_t windowstart = 400;
		uint16_t windowstepup = 200;
		uint16_t windowstepdown = 50;
		uint16_t windowfloor = 100;
		uint16_t hashsize = HASHSIZE;
		uint8_t timefactor = 40;
	}options;
	class bot{
	public:
		bot() { ; }
		move getMove(board* b);
		int16_t miniMax(board* b, uint8_t depth, int16_t alpha, int16_t beta, line* pline, bool notNull);
		static int16_t qSearch(board* b, int16_t alpha, int16_t beta);
		limits lim;
		options opt;
	private:
		hashtable table[HASHSIZE];
	};
}

#endif
