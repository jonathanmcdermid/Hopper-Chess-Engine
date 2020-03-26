#include <chrono>
#include "bot.h"
#include "board.h"

namespace Chess {

	move bot::getMove(board* b) {//calls minimax and controls depth, alpha beta windows, and time
		int32_t timeallotted = (b->getTurn()) ? lim.time[WHITE] / opt.timefactor: lim.time[BLACK] / opt.timefactor;
		auto start = std::chrono::high_resolution_clock::now();
		int16_t score = LOWERLIMIT;
		uint16_t window = opt.windowstart;
		int16_t alpha = LOWERLIMIT;
		int16_t beta = UPPERLIMIT;
		line pv;
		for (pv.cmove = 1; pv.cmove < lim.depth; ++pv.cmove) {
			score = miniMax(b, pv.cmove, alpha, beta, &pv, false);
			if (score == -MATE) { break; }
			else if (score <= alpha || score >= beta) {
				alpha = LOWERLIMIT;
				beta = UPPERLIMIT;
				--pv.cmove;
				window += opt.windowstepup;
			}
			else {
				alpha = score - window;
				beta = score + window;
				if (window > opt.windowfloor) { window -= opt.windowstepdown; }
				auto stop = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
				if (duration.count() > timeallotted) { break; }
			}
		}
		std::cout << "info depth " << (int) pv.cmove << " score cp " << (int) score << "\n";
		return table[b->getzHist(0) % HASHSIZE].getBmove();
	}

	int16_t bot::miniMax(board* b, uint8_t depth, int16_t alpha, int16_t beta, line* pline, bool notNull) {//negamax and move ordering, includes principle variations, nullmoves, and hash moves
		if (!depth) { return qSearch(b, alpha, beta); }
		line localline;
		int16_t score;
		if (notNull && depth > 3 && !b->checkTeam(b->getTurn())) {
			uint8_t R = depth / 2 - 2;
			b->movePiece(move(0, 0, NULLMOVE));
			score = -miniMax(b, R, -beta, -beta + 1, &localline, false);
			b->unmovePiece();
			if (score >= beta) { return score; }
		}
		uint8_t flag = CAPTURE;
		bool stuck = true;
		uint8_t index2 = 0;
		if (pline->movelink[0].getFlags() != FAIL) {
			for (uint8_t i = 0; i < b->cmove; ++i) {
				if (b->possiblemoves[i] == pline->movelink[0]) {
					move tempMove = b->possiblemoves[0];
					b->possiblemoves[0] = pline->movelink[0];
					b->possiblemoves[i] = tempMove;
					index2++;
				}
			}
		}
		if (table[b->getzHist(0) % HASHSIZE].getBmove().getFlags() != FAIL) {
			move tempMove = table[b->getzHist(0) % HASHSIZE].getBmove();
			for (uint8_t index1 = 0; index1 < b->cmove; ++index1) {
				if (b->possiblemoves[index1] == tempMove) {
					move tempMove2 = b->possiblemoves[index2];
					b->possiblemoves[index2] = tempMove;
					b->possiblemoves[index1] = tempMove2;
					++index2;
					break;
				}
			}
		}
		while (flag != FAIL) {
			for (uint8_t index1 = b->cmove - 1; index1 > index2; --index1) {
				if (b->possiblemoves[index1].getFlags() == flag) {
					move tempMove = b->possiblemoves[index2];
					b->possiblemoves[index2] = b->possiblemoves[index1];
					b->possiblemoves[index1] = tempMove;
					++index2;
				}
			}
			switch (flag) {
			case QPROMOTEC: {flag = QPROMOTE; break; }
			case QPROMOTE: {flag = KCASTLE;  break; }
			case KCASTLE: {flag = QCASTLE;  break; }
			case QCASTLE: {flag = CAPTURE;  break; }
			case CAPTURE: {flag = FAIL;	  break; }
			}
		}
		for (uint8_t index1 = 0; index1 < b->cmove; ++index1) {
			if (b->movePiece(b->possiblemoves[index1])) {
				stuck = false;
				if (b->getzHist(0) == b->getzHist(4)) { score = CONTEMPT; }
				else { score = -miniMax(b, depth - 1, -beta, -alpha, &localline, true); }
				b->unmovePiece();
				if (score > alpha) {
					pline->movelink[0] = b->possiblemoves[index1];
					for (uint8_t index2 = 1; index2 < depth; ++index2) { pline->movelink[index2] = localline.movelink[index2 - 1]; }
					pline->cmove = localline.cmove + 1;
					alpha = score;
				}
				if (score >= beta) { return score; }
			}
		}
		if (stuck) { alpha = (b->checkTeam(b->getTurn())) ? MATE : 0; }
		else { table[b->getzHist(0) % HASHSIZE] = hashtable(b->getzHist(0), depth, pline->movelink[0]); }
		return alpha;
	}

	int16_t bot::qSearch(board* b, int16_t alpha, int16_t beta) {//quiescent search
		int16_t score = b->negaEval();
		if (score >= beta) { return score; }
		if (score > alpha) { alpha = score; }
		for (uint8_t index1 = 0; index1 < b->cmove; ++index1) {
			if (b->possiblemoves[index1].getFlags() >= CAPTURE) {
				if (b->movePiece(b->possiblemoves[index1])) {
					score = -qSearch(b, -beta, -alpha);
					b->unmovePiece();
					if (score >= beta) { return score; }
					if (score > alpha) { alpha = score; }
				}
			}
		}
		return alpha;
	}
}