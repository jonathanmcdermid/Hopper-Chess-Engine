#ifndef BOT_H
#define BOT_H

#include "hashtable.h"
#include "pawnhash.h"
#include "killers.h"
#include "evaluate.h"

namespace Chess {
	class board;
	class interface;
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
		bot(interface* ifx, board* bd);
		void makeMove();
		int alphaBeta(int depth, int ply, int alpha, int beta, line* pline, bool notnull);
		int perft(int depth);
		int quiescentSearch(int alpha, int beta);
		limits lim;
	private:
		board* b;
		interface* fx;
		killers k;
		hashtable ht;
		pawnhash ph;
		evaluate e = evaluate(b);
		int nodes = 0;
	};
}
#endif
