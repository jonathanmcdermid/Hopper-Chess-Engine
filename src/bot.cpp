#include <chrono>
#include "bot.h"
#include "board.h"

namespace Chess {

	static int WPAWNBIT[SPACES] = {
		 0,  0,  0,  0,  0,  0,  0,  0,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		 5,  5, 10, 25, 25, 10,  5,  5,
		 0,  0,  0, 20, 20,  0,  0,  0,
		 5, -5,-10,  0,  0,-10, -5,  5,
		 5, 10, 10,-20,-20, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	static int BPAWNBIT[SPACES] = {
		 0,  0,  0,  0,  0,  0,  0,  0,
		 5, 10, 10,-20,-20, 10, 10,  5,
		 5, -5,-10,  0,  0,-10, -5,  5,
		 0,  0,  0, 20, 20,  0,  0,  0,
		 5,  5, 10, 25, 25, 10,  5,  5,
		10, 10, 20, 30, 30, 20, 10, 10,
		50, 50, 50, 50, 50, 50, 50, 50,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	static int WKNIGHTBIT[SPACES] = {
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	};

	static int BKNIGHTBIT[SPACES] = {
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	};

	static int WBISHOPBIT[SPACES] = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20,
	};

	static int BBISHOPBIT[SPACES] = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	};

	static int WROOKBIT[SPACES] = {
		 0,  0,  0,  0,  0,  0,  0,  0,
		 5, 10, 10, 10, 10, 10, 10,  5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		 0,  0,  0,  5,  5,  0,  0,  0
	};

	static int BROOKBIT[SPACES] = {
		 0,  0,  0,  5,  5,  0,  0,  0,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		 5, 10, 10, 10, 10, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	static int WQUEENBIT[SPACES] = {
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};

	static int BQUEENBIT[SPACES] = {
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-10,  5,  5,  5,  5,  5,  0,-10,
		  0,  0,  5,  5,  5,  5,  0, -5,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};

	static int WKINGBIT[SPACES] = {
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		 20, 20,  0,  0,  0,  0, 20, 20,
		 20, 30, 10,  0,  0, 10, 30, 20
	};

	static int BKINGBIT[SPACES] = {
		 20, 30, 10,  0,  0, 10, 30, 20,
		 20, 20,  0,  0,  0,  0, 20, 20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30
	};

	static int WENDKINGBIT[SPACES] = {
		-50,-40,-30,-20,-20,-30,-40,-50,
		-30,-20,-10,  0,  0,-10,-20,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-30,  0,  0,  0,  0,-30,-30,
		-50,-30,-30,-30,-30,-30,-30,-50
	};

	static int BENDKINGBIT[SPACES] = {
		-50,-30,-30,-30,-30,-30,-30,-50,
		-30,-30,  0,  0,  0,  0,-30,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-20,-10,  0,  0,-10,-20,-30,
		-50,-40,-30,-20,-20,-30,-40,-50
	};

	void bot::makeMove(board& b) {//calls minimax and controls depth, alpha beta windows, and time
		nodes = 0;
		int timeallotted = (b.turn) ? lim.time[WHITE] / (lim.movesleft + 20): lim.time[BLACK] / (lim.movesleft + 20);
		auto start = std::chrono::high_resolution_clock::now();
		int window = opt.windowstart;
		int score = LOWERLIMIT;
		int alpha = LOWERLIMIT;
		int beta = UPPERLIMIT;
		line pv;
		for (int depth = 1; depth < lim.depth; ++depth) {
			score = miniMax(b, depth, alpha, beta, &pv, false);
			std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " pv ";
			for (int i = 0; i < pv.cmove; ++i) {
				std::string message = { (char)(pv.movelink[i].getFrom() % WIDTH + 'a'), (char)(WIDTH - pv.movelink[i].getFrom() / WIDTH + '0'),' ',(char)(pv.movelink[i].getTo() % WIDTH + 'a'),(char)(WIDTH - pv.movelink[i].getTo() / WIDTH + '0') };
				std::cout << message << " ";
			}
			std::cout << "\n";
			if (score <= alpha || score >= beta) {
				alpha = LOWERLIMIT;
				beta = UPPERLIMIT;
				--depth;
				window += opt.windowstepup;
			}
			else {
				auto stop = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
				if (duration.count() > timeallotted) { std::cout << "time " << (int)duration.count()<<"\n"; break; }
				alpha = score - window;
				beta = score + window;
				if (window > opt.windowfloor) { window -= opt.windowstepdown; }
			}
		}
		b.movePiece(pv.movelink[0]);
		ht.clean();
	}

	int bot::miniMax(board& b, int depth, int alpha, int beta, line* pline, bool notNull) {//negamax and move ordering, includes principle variations, nullmoves, and hash moves
		if (!depth) { return qSearch(b, alpha, beta);}
		line localline;
		int score;
		int cmove = b.cmove;
		if (!cmove) { return (b.checkTurn()) ? -MATE - depth : 0; }
		if (notNull && depth > 3 && !b.checkTurn()) {
			b.movePiece(move(0, 0, NULLMOVE));
			score = -miniMax(b, depth / 2 - 2, -beta, -beta + 1, &localline, false);
			b.unmovePiece();
			if (score >= beta) { return score; }
		}
		int keyindex = b.currZ % HASHSIZE;
		move hashmove = ht.getMove(keyindex);
		if (ht.getZobrist(keyindex) == b.currZ && ht.getDepth(keyindex) >= depth) {
			int hasheval = ht.getEval(keyindex);
			int hashflags = ht.getFlags(keyindex);
			if (hashflags == HASHEXACT || (hashflags == HASHBETA && hasheval >= beta) || (hashflags == HASHALPHA && hasheval <= alpha)) {
				pline->movelink[0] = hashmove;
				pline->cmove = 1;
				return hasheval;
			}
		}
		int goodindex = 0;
		move moves[MEMORY];
		for (int i = 0; i < cmove; ++i) { moves[i] = b.possiblemoves[i]; }
		if (pline->movelink[0].getFlags() != FAIL) {
			for (int i = 0; i < cmove; ++i) {
				if (moves[i] == pline->movelink[0]) {
					if (!i) { break; }
					move temp = moves[0];
					moves[0] = pline->movelink[0];
					moves[i] = temp;
					++goodindex;
					break;
				}
			}
		}
		if (hashmove.getFlags() != FAIL) {
			for (int i = 0; i < cmove; ++i) {
				if (moves[i] == hashmove) {
					if (!i) { break; }
					move temp = moves[goodindex];
					moves[goodindex] = hashmove;
					moves[i] = temp;
					++goodindex;
					break;
				}
			}
		}
		for (int i = cmove - 1; i > goodindex; --i) {
			if (moves[i].getFlags() & (1 << 2)) {
				move temp = moves[goodindex];
				moves[goodindex] = moves[i];
				moves[i] = temp;
				++goodindex;
			}
		}
		int evaltype = NONE;
		for (int i = 0; i < cmove; ++i) {
			b.movePiece(moves[i]);
			++nodes;
			if (b.twofoldRep()) { score = CONTEMPT; }
			else { score = -miniMax(b, depth - 1, -beta, -alpha, &localline, true); }
			b.unmovePiece();
			if (score >= beta) {
				ht.newEntry(keyindex, hashentry(b.currZ, depth, score, HASHBETA, moves[i]));
				return score;
			}
			if (score > alpha) {
				evaltype = HASHEXACT;
				pline->movelink[0] = moves[i];
				for (int j = 1; j < depth; ++j) { pline->movelink[j] = localline.movelink[j - 1]; }
				pline->cmove = localline.cmove + 1;
				alpha = score;
			}
			else { evaltype = HASHALPHA; }
		}
		if (ht.getDepth(keyindex) < depth) { ht.newEntry(keyindex, hashentry(b.currZ, depth, alpha, evaltype, pline->movelink[0])); }
		return alpha;
	}

	int bot::qSearch(board& b, int alpha, int beta) {//quiescent search
		int score = negaEval(b);
		if (score >= beta) { return score; }
		if (score > alpha) { alpha = score; }
		if (!b.cmove) { return (b.checkTurn()) ? -MATE : 0; }
		move moves[SPACES];
		int cmove = 0;
		int goodindex = 0;
		for (int i = 0; i < b.cmove; ++i) {
			if (b.possiblemoves[i].getFlags() & (1 << 2)) { 
				moves[cmove] = b.possiblemoves[i]; 
				++cmove;
			}
		}
		for (int i = cmove - 1; i > goodindex; --i) {
			if (!b.threatened[!b.turn][moves[i].getTo()]) {
				move tempmove = moves[goodindex];
				moves[goodindex] = moves[i];
				moves[i] = tempmove;
				++goodindex;
			}
		}
		for (int i = cmove - 1; i > goodindex; --i) {
			if (abs(b.grid[moves[i].getFrom()]) < abs(b.grid[moves[i].getTo()])) {
				move tempmove = moves[goodindex];
				moves[goodindex] = moves[i];
				moves[i] = tempmove;
				++goodindex;
			}
		}
		for (int i = 0; i < cmove; ++i) {
			b.movePiece(moves[i]);
			score = -qSearch(b, -beta, -alpha);
			b.unmovePiece();
			if (score >= beta) { return score; }
			if (score > alpha) { alpha = score; }
		}
		return alpha;
	}

	int bot::negaEval(const board& b) {//negamax evaluation using material sum of pieces and bonus boards
		int sum = 0;
		for (int i = 0; i < SPACES; ++i) {
			switch (b.grid[i]) {
			case EMPTY:		break;
			case PAWN:		sum += WPAWNBIT[i];		break;
			case -PAWN:		sum -= BPAWNBIT[i];		break;
			case KNIGHT:	sum += WKNIGHTBIT[i];	break;
			case -KNIGHT:	sum -= BKNIGHTBIT[i];	break;
			case BISHOP:	sum += WBISHOPBIT[i];	break;
			case -BISHOP:	sum -= BBISHOPBIT[i];	break;
			case ROOK:		sum += WROOKBIT[i];		break;
			case -ROOK:		sum -= BROOKBIT[i];		break;
			case QUEEN:		sum += WQUEENBIT[i];	break;
			case -QUEEN:	sum -= BQUEENBIT[i];	break;
			case KING:		sum = (b.endgame) ? sum + WENDKINGBIT[i] : sum + WKINGBIT[i]; break;
			case -KING:		sum = (b.endgame) ? sum - BENDKINGBIT[i] : sum - BKINGBIT[i]; break;
			}
		}
		return (b.turn) ? b.currV + sum : -b.currV - sum;
	}

}