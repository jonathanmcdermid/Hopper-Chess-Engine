#include <chrono>
#include <iostream>
#include "Engine.h"

namespace Hopper 
{
	Engine::Engine(Board* bd) 
	{
		myBoard = bd;
	}

	void Engine::makeMove() 
	{//calls minimax and controls depth, alpha beta windows, and time
		auto startTime = std::chrono::high_resolution_clock::now();
		int timeallotted = (myBoard->getTurn()) ? lim.time[WHITE] / (lim.movesleft + 10) : lim.time[BLACK] / (lim.movesleft + 10);
		int window = 45;
		int alpha = LOWERLIMIT, beta = UPPERLIMIT;
		int score;
		line principalVariation;
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
			score = alphaBeta(depth, 0, alpha, beta, &principalVariation, false);
			myHashTable.extractPV(myBoard, &principalVariation);
			std::string message;
			std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " pv ";
			for (int i = 0; i < principalVariation.moveCount; ++i) {
				message += { (char)(principalVariation.moveLink[i].getFrom() % WIDTH + 'a'),
					(char) (WIDTH - (int)(principalVariation.moveLink[i].getFrom() / WIDTH) + '0'),
					(char) (principalVariation.moveLink[i].getTo() % WIDTH + 'a'),
					(char) (WIDTH - (int)(principalVariation.moveLink[i].getTo() / WIDTH) + '0'),
					(char) ' ' };
			}
			std::cout << message << "\n";
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - startTime);
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
		myBoard->movePiece(principalVariation.moveLink[0]);
		myHashTable.clean();
		myKillers.chrono();
	}

	int Engine::alphaBeta(int depth, int ply, int alpha, int beta, line* pline, bool isNull) 
	{
		if (!depth) 
			return quiescentSearch(alpha, beta);
		int keyindex = myBoard->getCurrZ() % HASHSIZE;
		if (myHashTable.getZobrist(keyindex) == myBoard->getCurrZ() && myHashTable.getDepth(keyindex) >= depth) 
		{
			if (myHashTable.getFlags(keyindex) == HASHEXACT || (myHashTable.getFlags(keyindex) == HASHBETA && myHashTable.getEval(keyindex) >= beta) || (myHashTable.getFlags(keyindex) == HASHALPHA && myHashTable.getEval(keyindex) <= alpha)) 
			{
				pline->moveLink[0] = myHashTable.getMove(keyindex);
				pline->moveCount = 1;
				return myHashTable.getEval(keyindex);
			}
		}
		line localline;
		int score;
		if (!isNull && depth > 3 && !myBoard->isCheck() && !myBoard->isEndgame()) 
		{
			myBoard->movePiece(NULLMOVE);
			score = -alphaBeta((int) (depth >> 1) - 2, ply + 1, -beta, -beta + 1, &localline, true);
			myBoard->unmovePiece();
			if (score >= beta) 
				return score;
		}
		MoveList ml(myBoard, pline->moveLink[0], myHashTable.getMove(keyindex), myKillers.getPrimary(ply));
		int evaltype = HASHALPHA;
		for (int genstate = GENPV; genstate != GENEND; ++genstate) 
		{
			ml.moveOrder(genstate);
			while (ml.movesLeft()) 
			{
				myBoard->movePiece(ml.getCurrMove());
				++nodes;
				if (myBoard->isRepititionDraw() || myBoard->isMaterialDraw()) 
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
				myBoard->unmovePiece();
				if (score > alpha) 
				{
					pline->moveLink[0] = ml.getCurrMove();
					if (score >= beta) 
					{
						pline->moveCount = 1;
						if (!ml.getCurrMove().isCap()) 
							myKillers.cutoff(ml.getCurrMove(), ply);
						myHashTable.newEntry(keyindex, myBoard->getCurrZ(), depth, score, HASHBETA, ml.getCurrMove());
						return score;
					}
					for (int j = 1; j < depth; ++j) 
						pline->moveLink[j] = localline.moveLink[j - 1];
					pline->moveCount = localline.moveCount + 1;
					evaltype = HASHEXACT;
					alpha = score;
				}
				ml.increment();
			}
		}
		if (ml.noMoves()) 
			return (myBoard->isCheck()) ? -MATE - depth : -CONTEMPT;
		else if (myHashTable.getDepth(keyindex) < depth) 
			myHashTable.newEntry(keyindex, myBoard->getCurrZ(), depth, alpha, evaltype, pline->moveLink[0]);
		return alpha;
	}

	int Engine::perft(int depth) 
	{
		if (!depth) 
			return 1;
		Move moves[MEMORY];
		int moveCount = myBoard->genAllMoves(moves), n = 0;
		for (int i = 0; i < moveCount; ++i) 
		{
			myBoard->movePiece(moves[i]);
			n += perft(depth - 1);
			myBoard->unmovePiece();
		}
		return n;
	}

	int Engine::quiescentSearch(int alpha, int beta) 
	{
		int score = negaEval();
		if (myPawnHashTable.getEntry(myBoard->getCurrP() % HASHSIZE) == myBoard->getCurrP()) 
			score += (myBoard->getTurn())? myPawnHashTable.getEntry(myBoard->getCurrP() % HASHSIZE): -myPawnHashTable.getEntry(myBoard->getCurrP() % HASHSIZE);
		else 
			myPawnHashTable.newEntry(myBoard->getCurrP() % HASHSIZE, pawnEval());
		if (score >= beta) 
			return score;
		if (score > alpha) 
			alpha = score;
		MoveList ml(myBoard);
		ml.moveOrder(GENWINCAPS);
		if (!ml.movesLeft()) 
			return (myBoard->isCheckMate()) ? (myBoard->isCheck()) ? -MATE : -CONTEMPT : score;
		do {
			myBoard->movePiece(ml.getCurrMove());
			score = -quiescentSearch(-beta, -alpha);
			myBoard->unmovePiece();
			if (score >= beta) 
				return score;
			else if (score > alpha) 
				alpha = score;
			ml.increment();
		} while (ml.movesLeft());
		return alpha;
	}
}