#ifndef BOT_H
#define BOT_H

#include "hashtable.h"

namespace Chess {
	class board;
	typedef struct limits {
		int time[2] = { 300000, 300000 };
		int inc[2] = { 100000, 100000 };
		int movetime = 0;
		int movesleft = 0;
		int depth = MAXDEPTH;
		int nodes = 0;
		int perft = 0;
		int mate = 0;
		bool infinite = false;
	}limits;
	typedef struct options {
		int windowstart = 100;
		int windowstepup = 200;
		int windowstepdown = 50;
		int windowfloor = 100;
		int hashsize = HASHSIZE;
		int timefactor = 40;
	}options;
	class bot{
	public:
		void makeMove(board& b);
		int miniMax(board& b, int depth, int alpha, int beta, line* pline, bool notNull);
		int qSearch(board& b, int alpha, int beta);
		int negaEval(const board& b);
		limits lim;
		options opt;
	private:
		int nodes;
		hashtable ht;
	};
}

#endif
