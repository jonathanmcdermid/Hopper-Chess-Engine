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
		auto start = std::chrono::high_resolution_clock::now();
		int timeallotted = (b.turn) ? lim.time[WHITE] / (lim.movesleft + 20) : lim.time[BLACK] / (lim.movesleft + 20);
		int window = opt.windowstart;
		int score = LOWERLIMIT;
		int alpha = LOWERLIMIT;
		int beta = UPPERLIMIT;
		nodes = 0;
		for (int i = 0; i < SPACES; ++i) {
			for (int j = 0; j < SPACES; ++j) {
				historyMoves[i][j]=0;
			}
		}
		line pv;
		//for (int depth = 1; depth < lim.depth; ++depth) {
		//	int n = perft(b, depth);
		//	auto stop = std::chrono::high_resolution_clock::now();
		//	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		//	std::cout << "info depth " << depth << " nodes " << n << " time " << (int)duration.count() <<"\n";
		//}
		for (int depth = 1; depth < lim.depth; ++depth) {
			score = miniMax(b, depth, 0, alpha, beta, &pv, false);
			std::string message;
			std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " pv ";
			for (int i = 0; i < pv.cmove; ++i) { message += { (char)(pv.movelink[i].getFrom() % WIDTH + 'a'), (char)(WIDTH - pv.movelink[i].getFrom() / WIDTH + '0'), (char)(pv.movelink[i].getTo() % WIDTH + 'a'), (char)(WIDTH - pv.movelink[i].getTo() / WIDTH + '0'), (char)' ' }; }
			std::cout << message << "\n";
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			if (duration.count() > timeallotted || score >= MATE) { std::cout << "time " << (int)duration.count() << "\n"; break; }
			else if (score <= alpha || score >= beta) {
				alpha = LOWERLIMIT;
				beta = UPPERLIMIT;
				--depth;
				//window += opt.windowstepup;
			}
			else {
				alpha = score - window;
				beta = score + window;
				//if (window > opt.windowfloor) { window -= opt.windowstepdown; }
			}
		}
		b.movePiece(pv.movelink[0]);
		ht.clean();
		k.chrono();
	}

	int bot::miniMax(board& b, int depth, int ply, int alpha, int beta, line* pline, bool notNull) {//negamax and move ordering, includes principle variations, nullmoves, and hash moves
		if (!depth) { return qSearch(b, alpha, beta); }
		int keyindex = b.currZ % HASHSIZE;
		move hashmove = ht.getMove(keyindex);
		if (ht.getZobrist(keyindex) == b.currZ && ht.getDepth(keyindex) >= depth) {
			if (ht.getFlags(keyindex) == HASHEXACT || (ht.getFlags(keyindex) == HASHBETA && ht.getEval(keyindex) >= beta) || (ht.getFlags(keyindex) == HASHALPHA && ht.getEval(keyindex) <= alpha)) {
				pline->movelink[0] = hashmove;
				pline->cmove = 1;
				return ht.getEval(keyindex);
			}
		}
		line localline;
		int score;
		if (notNull && depth > 3 && !b.checkTurn()) {
			b.movePiece(move(0, 0, NULLMOVE));
			score = -miniMax(b, depth / 2 - 2, ply + 1, -beta, -beta + 1, &localline, false);
			b.unmovePiece();
			if (score >= beta) { return score; }
		}
		move moves[MEMORY];
		int genstate = GENHASH;
		int evaltype = HASHALPHA;
		int cmove = 0;
		int index = 0;
		int goodindex;
		move killer;
		while (genstate != GENEND) {
			switch (genstate) {
			case GENHASH:
				if (b.validateMove(pline->movelink[0])) {
					moves[cmove] = pline->movelink[0];
					++cmove;
				}
				if (hashmove != moves[0] && b.validateMove(hashmove)) {
					moves[cmove] = hashmove;
					++cmove;
				}
				else { hashmove = move(); }
				break;
			case GENCAPS:
				cmove += b.genAllCaps(&moves[index]);
				if (hashmove != move() || pline->movelink[0] != move()) {
					for (int i = index; i < cmove; ++i) {
						if (moves[i] == hashmove || moves[i] == pline->movelink[0]) {
							--cmove;
							moves[i] = moves[cmove];
							--i;
						}
					}
				}
				goodindex = index;
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
				break;
			case GENKILLS:
				killer = k.getPrimary(ply);
				if (b.validateMove(killer)) {
					if (killer != pline->movelink[0] && killer != hashmove) {
						moves[cmove] = killer;
						++cmove;
					}
				}
				else { killer = move(); }
				break;
			case GENNONCAPS:
				cmove += b.genAllNonCaps(&moves[index]);
				if (hashmove != move() || pline->movelink[0] != move() || killer != move()) {
					for (int i = index; i < cmove; ++i) {
						if (moves[i] == hashmove || moves[i] == pline->movelink[0] || moves[i] == killer) {
							--cmove;
							moves[i] = moves[cmove];
							--i;
						}
					}
				}
				goodindex = index;
				for (int i = cmove - 1; i > goodindex; --i) {
					if (!b.threatened[!b.turn][moves[i].getTo()]) {
						move tempmove = moves[goodindex];
						moves[goodindex] = moves[i];
						moves[i] = tempmove;
						++goodindex;
					}
				}
				break;
			}
			while (index < cmove) {
				b.movePiece(moves[index]);
				++nodes;
				if (b.twofoldRep()) { score = CONTEMPT; }
				else { score = -miniMax(b, depth - 1, ply + 1, -beta, -alpha, &localline, true); }
				b.unmovePiece();
				if (score >= beta) {
					if (moves[index].getFlags() ^ 1 << 2) { k.cutoff(moves[index], ply); }
					ht.newEntry(keyindex, hashentry(b.currZ, depth, score, HASHBETA, moves[index]));
					return score;
				}
				else if (score > alpha) {
					evaltype = HASHEXACT;
					pline->movelink[0] = moves[index];
					for (int j = 1; j < depth; ++j) { pline->movelink[j] = localline.movelink[j - 1]; }
					pline->cmove = localline.cmove + 1;
					alpha = score;
				}
				++index;
			}
			++genstate;
		}
		if (!index) { return (b.checkTurn()) ? -MATE - depth : -CONTEMPT; }
		else if (ht.getDepth(keyindex) < depth) { ht.newEntry(keyindex, hashentry(b.currZ, depth, alpha, evaltype, pline->movelink[0])); }
		return alpha;
	}

	int bot::perft(board& b, int depth) {
		if (!depth) { return 1; }
		int nodes = 0;
		move moves[MEMORY];
		int cmove = b.genAll(moves);
		for (int i = 0; i < cmove; ++i) {
			//std::cout << moves[i].getFrom() << " " << moves[i].getTo() << "\n";
			b.movePiece(moves[i]);
			nodes += perft(b, depth - 1);
			b.unmovePiece();
		}
		return nodes;
	}

	int bot::qSearch(board& b, int alpha, int beta) {//quiescent search
		int score = negaEval(b);
		if (score >= beta) { return score; }
		if (score > alpha) { alpha = score; }
		move moves[MEMORY];
		int cmove = b.genAllCaps(moves);
		if (!cmove) {
			if (!b.genAllNonCaps(moves)) { return (b.checkTurn()) ? -MATE : -CONTEMPT; }
			return score;
		}
		int goodindex = 0;
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
			else if (score > alpha) { alpha = score; }
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
			case KING:		sum += (b.endgame) ? WENDKINGBIT[i] : WKINGBIT[i]; break;
			case -KING:		sum += (b.endgame) ? BENDKINGBIT[i] : BKINGBIT[i]; break;
			}
		}
		return (b.turn) ? b.currV + sum : -b.currV - sum;
	}

}