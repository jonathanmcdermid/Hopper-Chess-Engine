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
		int timeallotted = (b.turn) ? lim.time[WHITE] / (lim.movesleft + 10) : lim.time[BLACK] / (lim.movesleft + 10), window = 45, alpha = LOWERLIMIT, beta = UPPERLIMIT, score;
		line pv;
		nodes = 0;
		//for (int depth = 1; depth < lim.depth; ++depth) {
		//	int n = perft(b, depth);
		//	auto stop = std::chrono::high_resolution_clock::now();
		//	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		//	std::cout << "info depth " << depth << " nodes " << n << " time " << (int)duration.count() <<"\n";
		//}
		for (int depth = 1; depth < lim.depth; ++depth) {
			score = miniMax(b, depth, 0, alpha, beta, &pv, false);
			ht.extractPV(b, &pv);
			std::string message;
			std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " pv ";
			for (int i = 0; i < pv.cmove; ++i) { message += { (char)(pv.movelink[i].getFrom() % WIDTH + 'a'), (char)(WIDTH - pv.movelink[i].getFrom() / WIDTH + '0'), (char)(pv.movelink[i].getTo() % WIDTH + 'a'), (char)(WIDTH - pv.movelink[i].getTo() / WIDTH + '0'), (char)' ' }; }
			std::cout << message << "\n";
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			if (duration.count() > timeallotted || score >= MATE) { std::cout << "time " << (int)duration.count() << "\n"; break; }
			if (score <= alpha || score >= beta) {
				alpha = LOWERLIMIT;
				beta = UPPERLIMIT;
				--depth;
			}
			else {
				alpha = score - window;
				beta = score + window;
			}
		}
		b.movePiece(pv.movelink[0]);
		ht.clean();
		k.chrono();
	}

	int bot::miniMax(board& b, int depth, int ply, int alpha, int beta, line* pline, bool notNull) {//negamax
		if (!depth) { return qSearch(b, alpha, beta); }
		int keyindex = b.currZ % HASHSIZE;
		if (ht.getZobrist(keyindex) == b.currZ && ht.getDepth(keyindex) >= depth) {
			if (ht.getFlags(keyindex) == HASHEXACT || (ht.getFlags(keyindex) == HASHBETA && ht.getEval(keyindex) >= beta) || (ht.getFlags(keyindex) == HASHALPHA && ht.getEval(keyindex) <= alpha)) {
				pline->movelink[0] = ht.getMove(keyindex);
				pline->cmove = 1;
				return ht.getEval(keyindex);
			}
		}
		line localline;
		int score;
		if (ply && notNull && depth > 3 && !b.checkTurn()) {
			b.movePiece(move(0, 0, NULLMOVE));
			score = -miniMax(b, depth / 2 - 2, ply + 1, -beta, -beta + 1, &localline, false);
			b.unmovePiece();
			if (score >= beta) { return score; }
		}
		int evaltype = HASHALPHA, cmove = 0, index = 0, badcapstart = 0, badcapsend = 0;
		move pvmove = pline->movelink[0], killermove = k.getPrimary(ply), hashmove = ht.getMove(keyindex), tempmove, moves[MEMORY];
		for (int genstate = GENPV; genstate != GENEND; ++genstate) {
			switch (genstate) {
			case GENPV:
				if (b.validateMove(pvmove)) {
					moves[cmove++] = pvmove;
					if (hashmove == pvmove) { hashmove = move(); }
					if (killermove == pvmove) { killermove = move(); }
					goto branch;
				}
				else { ++genstate; }
			case GENHASH:
				if (b.validateMove(hashmove)) {
					moves[cmove++] = hashmove;
					if (killermove == hashmove) { killermove = move(); }
					goto branch;
				}
				else { ++genstate; }
			case GENWINCAPS:
				cmove += b.genAllCaps(&moves[index]);
				badcapstart = index;
				badcapsend = cmove;
				while (badcapstart < badcapsend && see(b, moves[badcapstart], -30)) { ++badcapstart; }
				for (int i = cmove - 1; i > badcapstart; --i) {
					if (see(b, moves[i], -30)) {
						tempmove = moves[badcapstart];
						moves[badcapstart] = moves[i];
						moves[i] = tempmove;
						++badcapstart;
					}
				}
				cmove = badcapstart;
				break;
			case GENKILLS:
				cmove = badcapsend;
				index = badcapsend;
				if (b.validateMove(killermove)) {
					moves[cmove++] = killermove;
					goto branch;
				}
				else { ++genstate; }
			case GENNONCAPS:
				cmove += b.genAllNonCaps(&moves[index]);
				break;
			case GENLOSECAPS:
				if (badcapstart != badcapsend) {
					index = badcapstart;
					cmove = badcapsend;
				}
			}
			while (index < cmove) {
			branch:
				b.movePiece(moves[index]);
				++nodes;
				if (b.twofoldRep()) { score = CONTEMPT; }
				else if (genstate > GENHASH) {
					if (depth > 2) { score = -miniMax(b, depth - 2, ply + 1, -alpha - 1, -alpha, &localline, true); }
					else { score = -miniMax(b, depth - 1, ply + 1, -alpha - 1, -alpha, &localline, true); }
					if (score > alpha && score < beta) { score = -miniMax(b, depth - 1, ply + 1, -beta, -alpha, &localline, true); }
				}
				else { score = -miniMax(b, depth - 1, ply + 1, -beta, -alpha, &localline, true); }
				b.unmovePiece();
				if (score > alpha) {
					pline->movelink[0] = moves[index];
					if (score >= beta) {
						pline->cmove = 1;
						if (moves[index].getFlags() ^ 1 << 2) { k.cutoff(moves[index], ply); }
						ht.newEntry(keyindex, hashentry(b.currZ, depth, score, HASHBETA, moves[index]));
						return score;
					}
					for (int j = 1; j < depth; ++j) { pline->movelink[j] = localline.movelink[j - 1]; }
					pline->cmove = localline.cmove + 1;
					evaltype = HASHEXACT;
					alpha = score;
				}
				++index;
			}
		}
		if (!cmove) { return (b.checkTurn()) ? -MATE - depth : -CONTEMPT; }
		else if (ht.getDepth(keyindex) < depth) { ht.newEntry(keyindex, hashentry(b.currZ, depth, alpha, evaltype, pline->movelink[0])); }
		return alpha;
	}

	int bot::perft(board& b, int depth) {
		if (!depth) { return 1; }
		move moves[MEMORY];
		int cmove = b.genAll(moves), n = 0;
		for (int i = 0; i < cmove; ++i) {
			//std::cout << moves[i].getFrom() << " " << moves[i].getTo() << "\n";
			b.movePiece(moves[i]);
			n += perft(b, depth - 1);
			b.unmovePiece();
		}
		return n;
	}

	int bot::qSearch(board& b, int alpha, int beta) {//quiescent search
		int score = negaEval(b);
		if (score >= beta) { return score; }
		if (score > alpha) { alpha = score; }
		move moves[MEMORY];
		int cmove = b.genAllCaps(moves);
		if (!cmove) {
			return (!b.genAllNonCaps(moves)) ? (b.checkTurn()) ? -MATE : -CONTEMPT : score;
		}
		int badcapstart = 0;
		move tempmove;
		while (badcapstart < cmove && see(b, moves[badcapstart], 0)) { ++badcapstart; }
		for (int i = cmove - 1; i > badcapstart; --i) {
			if (see(b, moves[i], 0)) {
				tempmove = moves[badcapstart];
				moves[badcapstart] = moves[i];
				moves[i] = tempmove;
				++badcapstart;
			}
		}
		for (int i = 0; i < badcapstart; ++i) {
			b.movePiece(moves[i]);
			score = -qSearch(b, -beta, -alpha);
			b.unmovePiece();
			if (score >= beta) { return score; }
			else if (score > alpha) { alpha = score; }
		}
		return alpha;
	}

	bool bot::see(board& b, move m, int threshold) {
		if (m == move() || (b.threatened[b.turn][m.getTo()] == 1 && b.threatened[!b.turn][m.getTo()] && abs(b.grid[m.getTo()]) > abs(b.grid[m.getFrom()]))) { return false; }
		else if (!b.threatened[!b.turn][m.getTo()] || abs(b.grid[m.getTo()]) > abs(b.grid[m.getFrom()])) { return true; }
		bool tomove = b.turn;
		int to = m.getTo();
		int from = m.getFrom();
		int attackers[2][WIDTH];
		int values[2][WIDTH];
		int total[2] = { 0,0 };
		int see = abs(b.grid[to]);
		int trophy = abs(b.grid[from]);
		int smallestindex = 0;
		if ((to + SOUTHEAST) % WIDTH > to % WIDTH && to < 55) {
			if (b.grid[to + SOUTHEAST] == KING) { attackers[WHITE][total[WHITE]++] = to + SOUTHEAST; }
			else if (b.grid[to + SOUTHEAST] == -KING) { attackers[BLACK][total[BLACK]++] = to + SOUTHEAST; }
		}
		if ((to + EAST) % WIDTH > to % WIDTH) {
			if (b.grid[to + EAST] == KING) { attackers[WHITE][total[WHITE]++] = to + EAST; }
			else if (b.grid[to + EAST] == -KING) { attackers[BLACK][total[BLACK]++] = to + EAST; }
		}
		if ((to + NORTHWEST) % WIDTH < to % WIDTH && to > 8) {
			if (b.grid[to + NORTHWEST] == KING) { attackers[WHITE][total[WHITE]++] = to + NORTHWEST; }
			else if (b.grid[to + NORTHWEST] == -KING) { attackers[BLACK][total[BLACK]++] = to + NORTHWEST; }
		}
		if ((to + WEST) % WIDTH < to % WIDTH && to > 0) {
			if (b.grid[to + WEST] == KING) { attackers[WHITE][total[WHITE]++] = to + WEST; }
			else if (b.grid[to + WEST] == -KING) { attackers[BLACK][total[BLACK]++] = to + WEST; }
		}
		if ((to + SOUTHWEST) % WIDTH < to % WIDTH && to < 57) {
			if (b.grid[to + SOUTHWEST] == KING) { attackers[WHITE][total[WHITE]++] = to + SOUTHWEST; }
			else if (b.grid[to + SOUTHWEST] == -KING) { attackers[BLACK][total[BLACK]++] = to + SOUTHWEST; }
		}
		if (to < 56) {
			if (b.grid[to + SOUTH] == KING) { attackers[WHITE][total[WHITE]++] = to + SOUTH; }
			else if (b.grid[to + SOUTH] == -KING) { attackers[BLACK][total[BLACK]++] = to + SOUTH; }
		}
		if ((to + NORTHEAST) % WIDTH > to % WIDTH && to > 6) {
			if (b.grid[to + NORTHEAST] == KING) { attackers[WHITE][total[WHITE]++] = to + NORTHEAST; }
			else if (b.grid[to + NORTHEAST] == -KING) { attackers[BLACK][total[BLACK]++] = to + NORTHEAST; }
		}
		if (to > 7) {
			if (b.grid[to + NORTH] == KING) { attackers[WHITE][total[WHITE]++] = to + NORTH; }
			else if (b.grid[to + NORTH] == -KING) { attackers[BLACK][total[BLACK]++] = to + NORTH; }
		}
		if (to % WIDTH) {
			if (b.grid[to + SOUTHWEST] == PAWN) { attackers[WHITE][total[WHITE]++] = to + SOUTHWEST; }
			else if (b.grid[to + NORTHWEST] == -PAWN) { attackers[BLACK][total[BLACK]++] = to + NORTHWEST; }
		}
		if (to % WIDTH != 7) {
			if (b.grid[to + SOUTHEAST] == PAWN) { attackers[WHITE][total[WHITE]++] = to + SOUTHEAST; }
			else if (b.grid[to + NORTHEAST] == -PAWN) { attackers[BLACK][total[BLACK]++] = to + NORTHEAST; }
		}
		if ((to + 10) % WIDTH > to % WIDTH && to < 54) {
			if (b.grid[to + 10] == KNIGHT) { attackers[WHITE][total[WHITE]++] = to + 10; }
			else if (b.grid[to + 10] == -KNIGHT) { attackers[BLACK][total[BLACK]++] = to + 10; }
		}
		if ((to + 17) % WIDTH > to % WIDTH && to < 47) {
			if (b.grid[to + 17] == KNIGHT) { attackers[WHITE][total[WHITE]++] = to + 17; }
			else if (b.grid[to + 17] == -KNIGHT) { attackers[BLACK][total[BLACK]++] = to + 17; }
		}
		if ((to - 10) % WIDTH < to % WIDTH && to > 9) {
			if (b.grid[to - 10] == KNIGHT) { attackers[WHITE][total[WHITE]++] = to - 10; }
			else if (b.grid[to - 10] == -KNIGHT) { attackers[BLACK][total[BLACK]++] = to - 10; }
		}
		if ((to - 17) % WIDTH < to % WIDTH && to > 16) {
			if (b.grid[to - 17] == KNIGHT) { attackers[WHITE][total[WHITE]++] = to - 17; }
			else if (b.grid[to - 17] == -KNIGHT) { attackers[BLACK][total[BLACK]++] = to - 17; }
		}
		if ((to + 6) % WIDTH < to % WIDTH && to < 58) {
			if (b.grid[to + 6] == KNIGHT) { attackers[WHITE][total[WHITE]++] = to + 6; }
			else if (b.grid[to + 6] == -KNIGHT) { attackers[BLACK][total[BLACK]++] = to + 6; }
		}
		if ((to + 15) % WIDTH < to % WIDTH && to < 49) {
			if (b.grid[to + 15] == KNIGHT) { attackers[WHITE][total[WHITE]++] = to + 15; }
			else if (b.grid[to + 15] == -KNIGHT) { attackers[BLACK][total[BLACK]++] = to + 15; }
		}
		if ((to - 6) % WIDTH > to % WIDTH && to > 5) {
			if (b.grid[to - 6] == KNIGHT) { attackers[WHITE][total[WHITE]++] = to - 6; }
			else if (b.grid[to - 6] == -KNIGHT) { attackers[BLACK][total[BLACK]++] = to - 6; }
		}
		if ((to - 15) % WIDTH > to % WIDTH && to > 14) {
			if (b.grid[to - 15] == KNIGHT) { attackers[WHITE][total[WHITE]++] = to - 15; }
			else if (b.grid[to - 15] == -KNIGHT) { attackers[BLACK][total[BLACK]++] = to - 15; }
		}
		for (int i = to + NORTH; i >= 0; i += NORTH) {
			if (b.grid[i]) {
				if (b.grid[i] == QUEEN || b.grid[i] == ROOK) { attackers[WHITE][total[WHITE]++] = i; }
				else if (b.grid[i] == -QUEEN || b.grid[i] == -ROOK) { attackers[BLACK][total[BLACK]++] = i; }
				else { break; }
			}
		}
		for (int i = to + SOUTH; i < SPACES; i += SOUTH) {
			if (b.grid[i]) {
				if (b.grid[i] == QUEEN || b.grid[i] == ROOK) { attackers[WHITE][total[WHITE]++] = i; }
				else if (b.grid[i] == -QUEEN || b.grid[i] == -ROOK) { attackers[BLACK][total[BLACK]++] = i; }
				else { break; }
			}
		}
		for (int i = to + EAST; i % WIDTH; i += EAST) {
			if (b.grid[i]) {
				if (b.grid[i] == QUEEN || b.grid[i] == ROOK) { attackers[WHITE][total[WHITE]++] = i; }
				else if (b.grid[i] == -QUEEN || b.grid[i] == -ROOK) { attackers[BLACK][total[BLACK]++] = i; }
				else { break; }
			}
		}
		for (int i = to + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
			if (b.grid[i]) {
				if (b.grid[i] == QUEEN || b.grid[i] == ROOK) { attackers[WHITE][total[WHITE]++] = i; }
				else if (b.grid[i] == -QUEEN || b.grid[i] == -ROOK) { attackers[BLACK][total[BLACK]++] = i; }
				else { break; }
			}
		}
		for (int i = to + NORTHEAST; i % WIDTH > to % WIDTH && i >= 0; i += NORTHEAST) {
			if (b.grid[i]) {
				if (b.grid[i] == QUEEN || b.grid[i] == BISHOP) { attackers[WHITE][total[WHITE]++] = i; }
				else if (b.grid[i] == -QUEEN || b.grid[i] == -BISHOP) { attackers[BLACK][total[BLACK]++] = i; }
				else if (b.grid[i] != -PAWN) { break; }
			}
		}
		for (int i = to + NORTHWEST; i % WIDTH < to % WIDTH && i >= 0; i += NORTHWEST) {
			if (b.grid[i]) {
				if (b.grid[i] == QUEEN || b.grid[i] == BISHOP) { attackers[WHITE][total[WHITE]++] = i; }
				else if (b.grid[i] == -QUEEN || b.grid[i] == -BISHOP) { attackers[BLACK][total[BLACK]++] = i; }
				else if (b.grid[i] != -PAWN) { break; }
			}
		}
		for (int i = to + SOUTHEAST; i % WIDTH > to % WIDTH && i < SPACES; i += SOUTHEAST) {
			if (b.grid[i]) {
				if (b.grid[i] == QUEEN || b.grid[i] == BISHOP) { attackers[WHITE][total[WHITE]++] = i; }
				else if (b.grid[i] == -QUEEN || b.grid[i] == -BISHOP) { attackers[BLACK][total[BLACK]++] = i; }
				else if(b.grid[i] != PAWN){ break; }
			}
		}
		for (int i = to + SOUTHWEST; i % WIDTH < to % WIDTH && i < SPACES; i += SOUTHWEST) {
			if (b.grid[i]) {
				if (b.grid[i] == QUEEN || b.grid[i] == BISHOP) { attackers[WHITE][total[WHITE]++] = i; }
				else if (b.grid[i] == -QUEEN || b.grid[i] == -BISHOP) { attackers[BLACK][total[BLACK]++] = i; }
				else if (b.grid[i] != PAWN) { break; }
			}
		}
		for (int i = 0; i < total[tomove]; ++i) {
			if (attackers[tomove][i] == from) { attackers[tomove][i] = attackers[tomove][--total[tomove]]; }
		}
		for (int i = 0; i < b.cpins; ++i) {
			for (int j = 0; j < total[tomove]; ++j) {
				if (attackers[tomove][j] == b.pins[i][0]) {
					switch (b.pins[i][1]) {
					case SOUTH:
					case NORTH:
						if (!(NSslide(from, attackers[tomove][j]))) { attackers[tomove][j] = attackers[tomove][--total[tomove]]; }
						else { continue; }
					case WEST:
					case EAST:
						if (!(EWslide(from, attackers[tomove][j]))) { attackers[tomove][j] = attackers[tomove][--total[tomove]]; }
						else { continue; }
					case NORTHEAST:
					case SOUTHWEST:
						if (!(NESWslide(from, attackers[tomove][j]))) { attackers[tomove][j] = attackers[tomove][--total[tomove]]; }
						else { continue; }
					case NORTHWEST:
					case SOUTHEAST:
						if (!(NWSEslide(from, attackers[tomove][j]))) { attackers[tomove][j] = attackers[tomove][--total[tomove]]; }
						else { continue; }
					}
				}
			}
		}
		for (int i = 0; i < total[WHITE]; ++i) {
			values[WHITE][i] = b.grid[attackers[WHITE][i]];
		}
		for (int i = 0; i < total[BLACK]; ++i) {
			values[BLACK][i] = -b.grid[attackers[BLACK][i]];
		}
		while (total[!tomove]) {
			tomove = !tomove;
			for (int i = 1; i < total[tomove]; ++i) {
				if (values[tomove][i] < values[tomove][smallestindex]) { smallestindex = i; }
			}
			see -= trophy;
			if (see >= threshold) { return true; }
			trophy = values[tomove][smallestindex];
			values[tomove][smallestindex] = values[tomove][--total[tomove]];
			tomove = !tomove;
			if (see + trophy < threshold || (!total[tomove] && see < threshold)) { return false; }
			smallestindex = 0;
			for (int i = 1; i < total[tomove]; ++i) {
				if (values[tomove][i] < values[tomove][smallestindex]) { smallestindex = i; }
			}
			see += trophy;
			trophy = values[tomove][smallestindex];
			values[tomove][smallestindex] = values[tomove][--total[tomove]];
			if (see - trophy >= threshold) { return true; }
			if (see < trophy) { return false; }
		}
		return see >= threshold;
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