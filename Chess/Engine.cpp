#include <chrono>
#include <iostream>
#include "Engine.h"

namespace Hopper
{
	Engine::Engine(Board* bd)
	{
		myBoard = bd;
		myHashTable.setSize(myLimits.hashbytes);
		init_tables();
	}

	void Engine::makeMove()
	{//calls minimax and controls depth, alpha beta windows, and time
		auto startTime = std::chrono::high_resolution_clock::now();
		std::chrono::steady_clock::time_point stop;
		std::chrono::milliseconds duration;
		unsigned window = 45;
		int alpha = LOWERLIMIT, beta = UPPERLIMIT;
		int score;
		line principalVariation;
		nodes = 0;
		Move rootMoves[128];
		unsigned nMoves = myBoard->genAllMoves(rootMoves);
		if (nMoves == 1) {
			principalVariation.moveLink[0] = rootMoves[0];
			principalVariation.moveCount = 1;
		}
		else {
			bool consensus = true;
			Move history[MAXDEPTH];
			unsigned timeallotted = (myLimits.time[myBoard->getTurn()] + myLimits.inc[myBoard->getTurn()]) / ((myBoard->getCurrF() == 5) ? 
				myLimits.movesleft / 2 + 1 : myLimits.movesleft + 1);
			for (unsigned depth = 1; depth < myLimits.depth; ++depth) {
				score = alphaBeta(depth, 0, alpha, beta, &principalVariation, false);
				std::string message;
				std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " pv ";
				for (unsigned i = 0; i < principalVariation.moveCount; ++i) {
					message += {
						(char)(principalVariation.moveLink[i].getFrom() % WIDTH + 'a'),
							(char)(WIDTH - (int)(principalVariation.moveLink[i].getFrom() / WIDTH) + '0'),
							(char)(principalVariation.moveLink[i].getTo() % WIDTH + 'a'),
							(char)(WIDTH - (int)(principalVariation.moveLink[i].getTo() / WIDTH) + '0'),
							(char)' ' };
				}
				std::cout << message << "\n";
				stop = std::chrono::high_resolution_clock::now();
				duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - startTime);
				if (duration.count() > timeallotted)
					break;
				if (score <= alpha || score >= beta) {
					alpha = LOWERLIMIT;
					beta = UPPERLIMIT;
					--depth;
				}
				else {
					alpha = score - window;
					beta = score + window;
					consensus = true;
					history[depth - 1] = principalVariation.moveLink[0];
					for (unsigned i = (depth > 4) ? depth - 4 : 1; i < depth; ++i) {
						if (history[i - 1] != history[i]) {
							consensus = false;
							break;
						}
					}
				}
				if (score >= MATE || score <= -MATE || (duration.count() > timeallotted / 2 && consensus))
					break;
			}
		}
		std::cout << "time " << (int)duration.count() << "\n";
		myBoard->movePiece(principalVariation.moveLink[0]);
		myHashTable.clean();
		myKillers.chrono();
	}

	int Engine::alphaBetaRoot(unsigned depth, unsigned ply, int alpha, int beta, line* pline, bool isNull) {
		return 0;
	} 

	int Engine::alphaBeta(unsigned depth, unsigned ply, int alpha, int beta, line* pline, bool isNull)
	{
		if (depth == 0)
			return quiescentSearch(alpha, beta);
		if (myHashTable.getZobrist(myBoard->getCurrZ()) == myBoard->getCurrZ() && myHashTable.getDepth(myBoard->getCurrZ()) >= depth) {
			if (myHashTable.getFlags(myBoard->getCurrZ()) == HASHEXACT
				|| (myHashTable.getFlags(myBoard->getCurrZ()) == HASHBETA && myHashTable.getEval(myBoard->getCurrZ()) >= beta)
				|| (myHashTable.getFlags(myBoard->getCurrZ()) == HASHALPHA && myHashTable.getEval(myBoard->getCurrZ()) <= alpha)) {
				pline->moveLink[0] = myHashTable.getMove(myBoard->getCurrZ());
				pline->moveCount = 1;
				return myHashTable.getEval(myBoard->getCurrZ());
			}
		}
		line localLine;
		int score;
		if (isNull == false && myBoard->isCheck() == false && myBoard->getGamePhase() >= NULLMOVE_GAMEPHASE_THRESHOLD) {
			myBoard->movePiece(NULLMOVE);
			score = -alphaBeta(0, ply + 1, -beta, -beta + 1, &localLine, true);
			myBoard->unmovePiece();
			if (score >= beta)
				return score;
		}
		MoveList localMoveList(myBoard, pline->moveLink[0], myHashTable.getMove(myBoard->getCurrZ()), myKillers.getPrimary(ply), myKillers.getSecondary(ply));
		unsigned evaltype = HASHALPHA;
		for (unsigned genstate = GENPV; genstate != GENEND; ++genstate)
		{
			localMoveList.moveOrder(genstate);
			while (localMoveList.movesLeft()) {
				myBoard->movePiece(localMoveList.getCurrMove());
				++nodes;
				if (myBoard->isPseudoRepititionDraw() || myBoard->isMaterialDraw())
					score = CONTEMPT;
				else if (genstate > GENHASH) {
					score = -alphaBeta((depth > 2) ? depth - 2 : 0, ply + 1, -alpha - 1, -alpha, &localLine, false);
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
						if (localMoveList.getCurrMove().isCap() == false)
							myKillers.cutoff(localMoveList.getCurrMove(), ply);
						myHashTable.newEntry(myBoard->getCurrZ(), depth, score, HASHBETA, localMoveList.getCurrMove());
						return score;
					}
					for (unsigned j = 1; j < depth; ++j)
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
		else if (myHashTable.getDepth(myBoard->getCurrZ()) < depth)
			myHashTable.newEntry(myBoard->getCurrZ(), depth, alpha, evaltype, pline->moveLink[0]);
		return alpha;
	}

	int Engine::quiescentSearch(int alpha, int beta)
	{
		int score = negaEval();
		if (myHashTable.getPawnZobrist(myBoard->getCurrP()) != myBoard->getCurrP())
			myHashTable.newPawnEntry(myBoard->getCurrP(), pawnEval());
		score += (myBoard->getTurn() == BLACK) ? -myHashTable.getPawnEval(myBoard->getCurrP()) : myHashTable.getPawnEval(myBoard->getCurrP());
		if (score >= beta)
			return score;
		else if (score > alpha)
			alpha = score;
		MoveList localMoveList(myBoard);
		localMoveList.moveOrder(GENWINCAPS);
		if (localMoveList.movesLeft() == false) 
			return score;
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

	void Engine::perftControl() {
		auto startTime = std::chrono::high_resolution_clock::now();
		for (unsigned depth = 1; depth < myLimits.depth; ++depth) {
			unsigned n = perft(depth);
			auto stopTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime);
			std::cout << "info depth " << depth << " nodes " << n << " time " << (int)duration.count() << "\n";
		}
	}

	unsigned Engine::perft(unsigned depth)
	{
		if (depth == 0)
			return 1;
		Move allMoves[MEMORY];
		unsigned moveCount = myBoard->genAllMoves(allMoves);
		Move allNonCapMoves[MEMORY];
		unsigned nonCapCount = myBoard->genAllNonCapMoves(allNonCapMoves);
		Move allCapMoves[MEMORY];
		unsigned capCount = myBoard->genAllCapMoves(allCapMoves);
		if (moveCount != capCount + nonCapCount) {
			myBoard->drawBoard();
			std::string message;
			message = "All Moves: ";
			for (unsigned i = 0; i < moveCount; ++i) {
				message += {
					(char)(allMoves[i].getFrom() % WIDTH + 'a'),
						(char)((WIDTH - (int)allMoves[i].getFrom() / WIDTH) + '0'),
						(char)(allMoves[i].getTo() % WIDTH + 'a'),
						(char)(WIDTH - (int)(allMoves[i].getTo() / WIDTH) + '0'),
						(char)' ' };
			}
			std::cout << message << "\n";
			message = "Caps: ";
			for (unsigned i = 0; i < capCount; ++i) {
				message += {
					(char)(allCapMoves[i].getFrom() % WIDTH + 'a'),
						(char)((WIDTH - (int)allCapMoves[i].getFrom() / WIDTH) + '0'),
						(char)(allCapMoves[i].getTo() % WIDTH + 'a'),
						(char)(WIDTH - (int)(allCapMoves[i].getTo() / WIDTH) + '0'),
						(char)' ' };
			}
			std::cout << message << "\n";
			message = "NonCaps: ";
			for (unsigned i = 0; i < nonCapCount; ++i) {
				message += {
					(char)(allNonCapMoves[i].getFrom() % WIDTH + 'a'),
						(char)((WIDTH - (int)allNonCapMoves[i].getFrom() / WIDTH) + '0'),
						(char)(allNonCapMoves[i].getTo() % WIDTH + 'a'),
						(char)(WIDTH - (int)(allNonCapMoves[i].getTo() / WIDTH) + '0'),
						(char)' ' };
			}
			std::cout << message << "\n";
			moveCount = myBoard->genAllCapMoves(allMoves);
		}
		unsigned n = 0;
		for (unsigned i = 0; i < moveCount; ++i) {
			myBoard->movePiece(allMoves[i]);
			n += perft(depth - 1);
			myBoard->unmovePiece();
		}
		return n;
	}
}