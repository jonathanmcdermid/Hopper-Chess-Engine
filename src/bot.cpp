#include <chrono>
#include "bot.h"
#include "board.h"

namespace Chess {

	bot::bot() {
		for (uint16_t i = 0; i < HASHSIZE; i++) { table[i] = hashtable(); }
		lim.time[WHITE] = 500000;
		lim.time[BLACK] = 500000;
		lim.inc[WHITE] = 0;
		lim.inc[WHITE] = 0;
		lim.depth = MAXDEPTH;
	}

	move bot::getMove(board* b) {//calls minimax and controls depth, alpha beta windows, and time
		uint32_t timeallotted = (b->getTurn()) ? lim.time[WHITE] / 50 : lim.time[BLACK] / 50;
		auto start = std::chrono::high_resolution_clock::now();
		int16_t score;
		int16_t alpha = LOWERLIMIT;
		int16_t beta = UPPERLIMIT;
		uint16_t window = 400;
		line pv;
		pv.cmove = 0;
		pv.movelink[0] = move();
		for (pv.cmove = 1; pv.cmove < lim.depth; ++pv.cmove) {
			pv.movelink[pv.cmove] = move();
			score = miniMax(b, pv.cmove, alpha, beta, &pv, false);
			if (score == -MATE) { break; }
			else if (score <= alpha || score >= beta) {
				alpha = LOWERLIMIT;
				beta = UPPERLIMIT;
				pv.cmove--;
				window += 200;
			}
			else {
				alpha = score - window;
				beta = score + window;
				if (window > 100) { window -= 50; }
				auto stop = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
				if (duration.count() > timeallotted) { break; }
			}
		}
		for (uint16_t i = 0; i < HASHSIZE; ++i) { table[i].chrono(); }
		return pv.movelink[0];
	}

	int16_t bot::miniMax(board* b, uint8_t depth, int16_t alpha, int16_t beta, line* pline, bool notNull) {//negamax and move ordering, includes principle variations, nullmoves, and hash moves
		if (!depth) {
			pline->cmove = 0;
			return qSearch(b, alpha, beta);

		}
		line localline;
		int16_t score;
		if (notNull && depth > 3 && !b->getCheck()) {
			uint8_t R = depth / 2 - 2;
			b->movePiece(move(0, 0, NULLMOVE));
			score = -miniMax(b, R, -beta, -beta + 1, &localline, false);
			b->unmovePiece();
			if (score >= beta) { return score; }
		}
		bool stuck = true;
		uint8_t index1;
		uint8_t index2;
		uint8_t flag = CAPTURE;
		uint8_t cmove = 0;
		uint16_t keyindex = b->getKey() % HASHSIZE;
		move possiblemoves[MEMORY];
		for (index1 = 0; index1 < SPACES; ++index1) {
			if ((b->getGrid(index1) > 0 && b->getTurn()) || (b->getGrid(index1) < 0 && !b->getTurn())) {
				for (index2 = 0; index2 < SPACES; ++index2) {
					possiblemoves[cmove] = b->createMove(index1, index2);
					if (possiblemoves[cmove].getFlags() != FAIL) { cmove++; }
				}
			}
		}
		possiblemoves[cmove] = move();
		index2 = 0;
		while (pline->movelink[0].getFlags()!=FAIL) {
			if (pline->movelink[0] == possiblemoves[index2]) {
				possiblemoves[cmove + 1] = possiblemoves[0];
				possiblemoves[0] = possiblemoves[index2];
				possiblemoves[index2] = possiblemoves[cmove + 1];
				index2 = 1;
				break;
			}
			if (index2 == cmove) { 
				index2 = 0;
				break; 
			}
			index2++;
		}
		if (table[keyindex].getBmove().getFlags() != FAIL) {
			possiblemoves[cmove+1]=table[keyindex].getBmove();
			for (uint8_t index3 = index2; index3 < cmove; ++index3) {
				if (possiblemoves[index3] == possiblemoves[cmove + 1]) {
					possiblemoves[cmove + 1] = possiblemoves[index2];
					possiblemoves[index2] = possiblemoves[index3];
					possiblemoves[index3] = possiblemoves[cmove + 1];
					index2++;
					break;
				}
			}
		}
		while (flag != FAIL) {
			for (index1 = cmove - 1; index1 > index2; --index1) {
				if (possiblemoves[index1].getFlags() == flag) {
					possiblemoves[cmove + 1] = possiblemoves[index2];
					possiblemoves[index2] = possiblemoves[index1];
					possiblemoves[index1] = possiblemoves[cmove + 1];
					index2++;
				}
			}
			switch (flag) {
			case QPROMOTEC: {flag = QPROMOTE; break; }
			case QPROMOTE:	{flag = KCASTLE;  break; }
			case KCASTLE:	{flag = QCASTLE;  break; }
			case QCASTLE:	{flag = CAPTURE;  break; }
			case CAPTURE:	{flag = FAIL;	  break; }
			}
		}
		for (index1 = 0; index1 < cmove; ++index1) {
			if (b->movePiece(possiblemoves[index1])) {
				stuck = false;
				score = -miniMax(b, depth - 1, -beta, -alpha, &localline, true);
				b->unmovePiece();
				if (score > alpha) {
					pline->movelink[0] = possiblemoves[index1];
					for (uint8_t index2 = 1; index2 < depth; ++index2) { pline->movelink[index2] = localline.movelink[index2 - 1]; }
					pline->cmove = localline.cmove + 1;
					alpha = score;
				}
				if (score >= beta) { 
					table[keyindex] = hashtable(b->getKey(), depth, possiblemoves[index1]);
					return score; 
				}
			}
		}
		if (stuck) { alpha = (b->getCheck()) ? MATE : 0; }
		else if (table[keyindex].getDepth() < depth) { table[keyindex] = hashtable(b->getKey(), depth, pline->movelink[0]); }
		return alpha;
	}

	int16_t bot::qSearch(board* b, int16_t alpha, int16_t beta) {//quiescent search
		int16_t score = b->negaEval();
		if (score >= beta) { return score; }
		if (score > alpha) { alpha = score; }
		move possiblemoves[16];
		uint8_t cmove = 0;
		for (uint8_t from = 0; from < SPACES; ++from) {
			if ((b->getGrid(from) > 0 && b->getTurn()) || (b->getGrid(from) < 0 && !b->getTurn())) {
				for (uint8_t to = 0; to < SPACES; ++to) {
					if ((b->getGrid(to) < 0 && b->getTurn()) || (b->getGrid(to) > 0 && !b->getTurn())) {
						possiblemoves[cmove] = b->createMove(from, to);
						if (possiblemoves[cmove].getFlags() != FAIL) { cmove++; }
					}
				}
			}
		}
		possiblemoves[cmove] = move();
		for (uint8_t index1 = 0; index1 < cmove; ++index1) {
			if (b->movePiece(possiblemoves[index1])) {
				score = -qSearch(b, -beta, -alpha);
				b->unmovePiece();
				if (score >= beta) { return score; }
				if (score > alpha) { alpha = score; }
			}
		}
		return alpha;
	}
}