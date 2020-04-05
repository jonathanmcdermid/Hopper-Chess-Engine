#ifndef BOT_H
#define BOT_H

#include "hashtable.h"

namespace Chess {
	class board;
	typedef struct limits {
		int time[2] = { 300000, 300000 };
		int inc[2] = { 100000, 100000 };
		int movetime = 0;
		int movesleft = 40;
		int depth = MAXDEPTH;
		int nodes = 0;
		int perft = 0;
		int mate = 0;
		bool infinite = false;
	}limits;
	typedef struct options {
		int windowstart = 225;
		int windowstepup = 50;
		int windowstepdown = 25;
		int windowfloor = 30;
		int hashsize = HASHSIZE;
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
		int nodes = 0;
		hashtable ht;
	};
}

#endif
