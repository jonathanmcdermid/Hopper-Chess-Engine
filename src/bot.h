#ifndef BOT_H
#define BOT_H

#include "hashtable.h"
#include "killers.h"

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
	} limits;
	class bot {
	public:
		void makeMove(board& b);
		int miniMax(board& b, int depth, int ply, int alpha, int beta, line* pline, bool notNull);
		int perft(board& b, int depth);
		int qSearch(board& b, int alpha, int beta);
		bool see(board& b, move m, int threshold);
		int negaEval(const board& b);
		limits lim;
	private:
		killers k;
		int nodes = 0;
		hashtable ht;
	};
}
#endif
