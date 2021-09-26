#include <chrono>
#include <iostream>
#include "Bot.h"

namespace Hopper 
{
	Bot::Bot(Board* bd) 
	{
		b = bd;
	}

	void Bot::makeMove() 
	{//calls minimax and controls depth, alpha beta windows, and time
		auto start = std::chrono::high_resolution_clock::now();
		int timeallotted = (b->turn) ? lim.time[WHITE] / (lim.movesleft + 10) : lim.time[BLACK] / (lim.movesleft + 10);
		int window = 45;
		int alpha = LOWERLIMIT, beta = UPPERLIMIT;
		int score;
		line pv;
		nodes = 0;
		/*for (int depth = 1; depth < lim.depth; ++depth)
		{
			int n = perft(depth);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			std::cout << "info depth " << depth << " nodes " << n << " time " << (int)duration.count() <<"\n";
		}*/
		for (int depth = 1; depth < lim.depth; ++depth) 
		{
			score = alphaBeta(depth, 0, alpha, beta, &pv, false);
			ht.extractPV(b, &pv);
			std::string message;
			std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " pv ";
			for (int i = 0; i < pv.cmove; ++i) {
				message += { (char)(pv.movelink[i].getFrom() % WIDTH + 'a'),
					(char) (WIDTH - (int)(pv.movelink[i].getFrom() / WIDTH) + '0'),
					(char) (pv.movelink[i].getTo() % WIDTH + 'a'),
					(char) (WIDTH - (int)(pv.movelink[i].getTo() / WIDTH) + '0'),
					(char) ' ' };
			}
			std::cout << message << "\n";
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			if (duration.count() > timeallotted)
			{
				std::cout << "time " << (int)duration.count() << "\n";
				break;
			}
			if (score <= alpha || score >= beta) 
			{
				alpha = LOWERLIMIT;
				beta = UPPERLIMIT;
				--depth;
			}
			else 
			{
				alpha = score - window;
				beta = score + window;
			}
		}
		b->movePiece(pv.movelink[0]);
		ht.clean();
		k.chrono();
	}

	int Bot::alphaBeta(int depth, int ply, int alpha, int beta, line* pline, bool isNull) 
	{
		if (!depth) 
			return quiescentSearch(alpha, beta);
		int keyindex = b->getCurrZ() % HASHSIZE;
		if (ht.getZobrist(keyindex) == b->getCurrZ() && ht.getDepth(keyindex) >= depth) 
		{
			if (ht.getFlags(keyindex) == HASHEXACT || (ht.getFlags(keyindex) == HASHBETA && ht.getEval(keyindex) >= beta) || (ht.getFlags(keyindex) == HASHALPHA && ht.getEval(keyindex) <= alpha)) 
			{
				pline->movelink[0] = ht.getMove(keyindex);
				pline->cmove = 1;
				return ht.getEval(keyindex);
			}
		}
		line localline;
		int score;
		if (!isNull && depth > 3 && !b->isCheck() && !b->isEndgame()) 
		{
			b->movePiece(NULLMOVE);
			score = -alphaBeta((int) (depth >> 1) - 2, ply + 1, -beta, -beta + 1, &localline, true);
			b->unmovePiece();
			if (score >= beta) 
				return score;
		}
		MoveList ml(b, pline->movelink[0], ht.getMove(keyindex), k.getPrimary(ply));
		int evaltype = HASHALPHA;
		for (int genstate = GENPV; genstate != GENEND; ++genstate) 
		{
			ml.moveOrder(genstate);
			while (ml.movesLeft()) 
			{
				b->movePiece(ml.getCurrMove());
				++nodes;
				if (b->isRepititionDraw() || b->isMaterialDraw()) 
					score = CONTEMPT;
				else if (genstate > GENHASH) 
				{
					score = (depth > 1) ? 
						-alphaBeta(depth - 2, ply + 1, -alpha - 1, -alpha, &localline, false) : 
						-alphaBeta(depth - 1, ply + 1, -alpha - 1, -alpha, &localline, false);
					if (score > alpha && score < beta) 
						score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localline, false);
				}
				else 
					score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localline, false);
				b->unmovePiece();
				if (score > alpha) 
				{
					pline->movelink[0] = ml.getCurrMove();
					if (score >= beta) 
					{
						pline->cmove = 1;
						if (!ml.getCurrMove().isCap()) 
							k.cutoff(ml.getCurrMove(), ply);
						ht.newEntry(keyindex, b->getCurrZ(), depth, score, HASHBETA, ml.getCurrMove());
						return score;
					}
					for (int j = 1; j < depth; ++j) 
						pline->movelink[j] = localline.movelink[j - 1];
					pline->cmove = localline.cmove + 1;
					evaltype = HASHEXACT;
					alpha = score;
				}
				ml.increment();
			}
		}
		if (ml.noMoves()) 
			return (b->isCheck()) ? -MATE - depth : -CONTEMPT;
		else if (ht.getDepth(keyindex) < depth) 
			ht.newEntry(keyindex, b->getCurrZ(), depth, alpha, evaltype, pline->movelink[0]);
		return alpha;
	}

	int Bot::perft(int depth) 
	{
		if (!depth) 
			return 1;
		Move moves[MEMORY];
		int cmove = b->genAll(moves), n = 0;
		for (int i = 0; i < cmove; ++i) 
		{
			b->movePiece(moves[i]);
			n += perft(depth - 1);
			b->unmovePiece();
		}
		return n;
	}

	int Bot::quiescentSearch(int alpha, int beta) 
	{
		int score = negaEval();
		if (ph.getEntry(b->getCurrP() % HASHSIZE) == b->getCurrP()) 
			score += (b->turn)? ph.getEntry(b->getCurrP() % HASHSIZE): -ph.getEntry(b->getCurrP() % HASHSIZE);
		else 
			ph.newEntry(b->getCurrP() % HASHSIZE, pawnEval()); 
		if (score >= beta) 
			return score;
		if (score > alpha) 
			alpha = score;
		MoveList ml(b);
		ml.moveOrder(GENWINCAPS);
		if (!ml.movesLeft()) 
			return (b->isCheckMate()) ? (b->isCheck()) ? -MATE : -CONTEMPT : score;
		do {
			b->movePiece(ml.getCurrMove());
			score = -quiescentSearch(-beta, -alpha);
			b->unmovePiece();
			if (score >= beta) 
				return score;
			else if (score > alpha) 
				alpha = score;
			ml.increment();
		} while (ml.movesLeft());
		return alpha;
	}
}