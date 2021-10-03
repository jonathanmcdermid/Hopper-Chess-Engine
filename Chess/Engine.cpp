#include <chrono>
#include <iostream>
#include "Engine.h"

namespace Hopper
{
	Engine::Engine(Board* bd)
	{
		myBoard = bd;
		myHashTable.setSize(myLimits.hashbytes);
	}

	void Engine::makeMove()
	{//calls minimax and controls depth, alpha beta windows, and time
		auto startTime = std::chrono::high_resolution_clock::now();
		int timeallotted = (myLimits.time[myBoard->getTurn()] + myLimits.inc[myBoard->getTurn()]) / (myLimits.movesleft);
		int window = 45;
		int alpha = LOWERLIMIT, beta = UPPERLIMIT;
		int score;
		line principalVariation;
		nodes = 0;
		/*for (int depth = 1; depth < myLimits.depth; ++depth)
		{
			int n = perft(depth);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			std::cout << "info depth " << depth << " nodes " << n << " time " << (int)duration.count() <<"\n";
		}*/
		for (int depth = 1; depth < myLimits.depth; ++depth) {
			score = alphaBeta(depth, 0, alpha, beta, &principalVariation, false);
			myHashTable.extractPV(myBoard, &principalVariation);
			std::string message;
			std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " pv ";
			for (int i = 0; i < principalVariation.moveCount; ++i) {
				message += { (char)(principalVariation.moveLink[i].getFrom() % WIDTH + 'a'),
					(char)(WIDTH - (int)(principalVariation.moveLink[i].getFrom() / WIDTH) + '0'),
					(char)(principalVariation.moveLink[i].getTo() % WIDTH + 'a'),
					(char)(WIDTH - (int)(principalVariation.moveLink[i].getTo() / WIDTH) + '0'),
					(char)' ' };
			}
			std::cout << message << "\n";
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - startTime);
			if (duration.count() > timeallotted) {
				std::cout << "time " << (int)duration.count() << "\n";
				break;
			}
			if (score <= alpha || score >= beta) {
				if (abs(score) >= MATE )
					break;
				alpha = LOWERLIMIT;
				beta = UPPERLIMIT;
				--depth;
			}
			else {
				if(score == CONTEMPT && principalVariation.moveCount == 1 && depth != 1)
					break;
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
		unsigned keyIndex = (unsigned) (myBoard->getCurrZ() % myHashTable.getSize());
		if (myHashTable.getZobrist(keyIndex) == myBoard->getCurrZ() && myHashTable.getDepth(keyIndex) >= depth) {
			if (myHashTable.getFlags(keyIndex) == HASHEXACT || (myHashTable.getFlags(keyIndex) == HASHBETA && myHashTable.getEval(keyIndex) >= beta) || (myHashTable.getFlags(keyIndex) == HASHALPHA && myHashTable.getEval(keyIndex) <= alpha)) {
				pline->moveLink[0] = myHashTable.getMove(keyIndex);
				pline->moveCount = 1;
				return myHashTable.getEval(keyIndex);
			}
		}
		line localLine;
		int score;
		if (!isNull && depth > 3 && !myBoard->isCheck() && !myBoard->isEndgame()) {
			myBoard->movePiece(NULLMOVE);
			score = -alphaBeta(depth / 2 - 2, ply + 1, -beta, -beta + 1, &localLine, true);
			myBoard->unmovePiece();
			if (score >= beta)
				return score;
		}
		MoveList localMoveList(myBoard, pline->moveLink[0], myHashTable.getMove(keyIndex), myKillers.getPrimary(ply), myKillers.getSecondary(ply));
		int evaltype = HASHALPHA;
		for (int genstate = GENPV; genstate != GENEND; ++genstate)
		{
			localMoveList.moveOrder(genstate);
			while (localMoveList.movesLeft()) {
				myBoard->movePiece(localMoveList.getCurrMove());
				++nodes;
				if (myBoard->isPseudoRepititionDraw() || myBoard->isMaterialDraw())
					score = CONTEMPT;
				else if (genstate > GENHASH) {
					score = (depth > 1) ?
						-alphaBeta(depth - 2, ply + 1, -alpha - 1, -alpha, &localLine, false) :
						-alphaBeta(depth - 1, ply + 1, -alpha - 1, -alpha, &localLine, false);
					if (score > alpha && score < beta)
						score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localLine, false);
				}
				else
					score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localLine, false);
				myBoard->unmovePiece();
				if (score > alpha) {
					pline->moveLink[0] = localMoveList.getCurrMove();
					if (score >= beta) {
						pline->moveCount = 1;
						if (!localMoveList.getCurrMove().isCap())
							myKillers.cutoff(localMoveList.getCurrMove(), ply);
						myHashTable.newEntry(keyIndex, myBoard->getCurrZ(), depth, score, HASHBETA, localMoveList.getCurrMove());
						return score;
					}
					for (int j = 1; j < depth; ++j)
						pline->moveLink[j] = localLine.moveLink[j - 1];
					pline->moveCount = localLine.moveCount + 1;
					evaltype = HASHEXACT;
					alpha = score;
				}
				localMoveList.increment();
			}
		}
		if (localMoveList.noMoves())
			return (myBoard->isCheck()) ? -MATE - depth : CONTEMPT;
		else if (myHashTable.getDepth(keyIndex) < depth)
			myHashTable.newEntry(keyIndex, myBoard->getCurrZ(), depth, alpha, evaltype, pline->moveLink[0]);
		return alpha;
	}

	int Engine::perft(int depth)
	{
		if (!depth)
			return 1;
		Move allMoves[MEMORY];
		int moveCount = myBoard->genAllMoves(allMoves), n = 0;
		for (int i = 0; i < moveCount; ++i) {
			myBoard->movePiece(allMoves[i]);
			n += perft(depth - 1);
			myBoard->unmovePiece();
		}
		return n;
	}

	int Engine::quiescentSearch(int alpha, int beta)
	{
		int score = negaEval();
		if (myHashTable.getPawnEntry(myBoard->getCurrP() % myHashTable.getPawnSize()) == myBoard->getCurrP())
			score += (myBoard->getTurn()) ? myHashTable.getPawnEntry(myBoard->getCurrP() % myHashTable.getPawnSize()) : -myHashTable.getPawnEntry(myBoard->getCurrP() % myHashTable.getPawnSize());
		else
			myHashTable.newPawnEntry(myBoard->getCurrP() % myHashTable.getPawnSize(), pawnEval());
		if (score >= beta)
			return score;
		if (score > alpha)
			alpha = score;
		MoveList localMoveList(myBoard);
		localMoveList.moveOrder(GENWINCAPS);
		if (!localMoveList.movesLeft())
			return (myBoard->isCheckMate()) ? (myBoard->isCheck()) ? -MATE : CONTEMPT : score;
		do {
			myBoard->movePiece(localMoveList.getCurrMove());
			score = -quiescentSearch(-beta, -alpha);
			myBoard->unmovePiece();
			if (score >= beta)
				return score;
			else if (score > alpha)
				alpha = score;
			localMoveList.increment();
		} while (localMoveList.movesLeft());
		return alpha;
	}
}