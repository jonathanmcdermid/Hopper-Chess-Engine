#include <chrono>
#include <iostream>
#include <math.h>
#include <cstring>
#include "Engine.h"

namespace Hopper
{
	int LMRTable[MAXDEPTH + 1][64];

	Engine::Engine(Board* bd)
	{
		myBoard = bd;
		myHashTable.setSize(myLimits.hashbytes);
		initEvalTables();
		initLMRTables();
		memset(hh, 0, sizeof(hh));
		lastEval = 0;
	}

	void Engine::initLMRTables() 
	{
		for (unsigned depth = 1; depth <= MAXDEPTH; ++depth)
			for (unsigned ordered = 1; ordered < 64; ++ordered)
				LMRTable[depth][ordered] = (int) (0.75 + log(depth) * log(ordered) / 2.25);
	}

	void Engine::makeMove()
	{//calls minimax and controls depth, alpha beta windows, and time
		auto startTime = std::chrono::high_resolution_clock::now();
		auto now = std::chrono::high_resolution_clock::now();
		unsigned window = 25;
		int alpha = LOWERLIMIT, beta = UPPERLIMIT;
		int score;
		line principalVariation;
		nodes = 0;
		scoredMove rootMoves[MEMORY];
		unsigned nMoves = myBoard->genAllMoves(rootMoves);
		if (nMoves == 1) {
			principalVariation.moveLink[0] = rootMoves[0].myMove;
			principalVariation.moveCount = 1;
		}
		else {
			bool panic = false;
			bool consensus = true;
			Move moveHistory[MAXDEPTH];
			unsigned timeallotted = (myLimits.time[myBoard->getTurn()] + myLimits.inc[myBoard->getTurn()]) / (myLimits.movesleft + 1);
			for (int depth = 1; depth < myLimits.depth; ++depth) {
				score = alphaBeta(depth, 0, alpha, beta, &principalVariation, false, false);
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
				if (score <= alpha || score >= beta) {
					beta = UPPERLIMIT;
					alpha = LOWERLIMIT;
					--depth;
				}
				//else if (score >= beta) {
				//	beta = UPPERLIMIT;
				//	--depth;
				//}
				else {
					alpha	= depth > 5 ? score - window : LOWERLIMIT;
					beta	= depth > 5 ? score + window : UPPERLIMIT;
					consensus = true;
					moveHistory[depth - 1] = principalVariation.moveLink[0];
					for (int i = (depth > CONSENSUS_THRESHOLD) ? depth - CONSENSUS_THRESHOLD : 1; i < depth; ++i) {
						if (moveHistory[i - 1] != moveHistory[i]) {
							consensus = false;
							break;
						}
					}
				}
				now = std::chrono::high_resolution_clock::now();
				// if PV mate is found, no point in continuing search
				if (score >= MATE || score <= -MATE ||
					// if we are over time limit, need to stop search unless we are significantly lower than previous eval
					std::chrono::duration_cast<std::chrono::milliseconds>
					(now - startTime).count() > timeallotted ||
					// if the past few iterative searches have yielded the same move, we will stop early
					(consensus == true && panic == false && 
					std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() > timeallotted / CONSENSUS_REDUCTION_FACTOR) ||
					// if engine was in panic state but has now found a move, we can stop
					(panic == true && score >= lastEval - PANIC_THRESHOLD)) {
					// if we are over our time limit but we cant find a good move, take more time and panic
					if (score < lastEval - PANIC_THRESHOLD && panic == false) {
						timeallotted *= 1 + 
							(lastEval - score) / PANIC_THRESHOLD < PANIC_EXTENSION_LIMIT ? 
							(lastEval - score) / PANIC_THRESHOLD : PANIC_EXTENSION_LIMIT;
						panic = true;
					}
					else {
						lastEval = score;
						break;
					}
				}
			}
		}
		std::cout << "time " << (int) std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() << "\n";
		myBoard->movePiece(principalVariation.moveLink[0]);
		myHashTable.clean();
		myKillers.chrono();
	}

	/*
	int Engine::alphaBetaRoot(int depth, int ply, int alpha, int beta, bool isNull, bool cutNode)
	{
		line localLine;
		int R;
		bool isCheck = myBoard->isCheck();
		std::sort(rootMoves, rootMoves + nRootMoves, smScoreComp);
		for (int i = 0; i < nRootMoves; ++i) {
			++nodes;
			myBoard->movePiece(rootMoves[i].myMove);
			if (depth > 2 && i != 0) {
				R = 1 + LMRTable[depth][i % 64];
				if (isCheck == false)
					rootMoves[i].score = -alphaBeta(depth - R, ply + 1, -alpha - 1, -alpha, &localLine, false, true);
				if (isCheck == true || rootMoves[i].score > alpha)
					rootMoves[i].score = -alphaBeta(depth - 1, ply + 1, -alpha - 1, -alpha, &localLine, false, !cutNode);
				if (rootMoves[i].score > alpha)
					rootMoves[i].score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localLine, false, false);
			}
			else
				rootMoves[i].score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localLine, false, false);
			myBoard->unmovePiece();
			if (rootMoves[i].score > alpha) {
				principalVariation.moveLink[0] = rootMoves[i].myMove;
				if (rootMoves[i].score >= beta) {
					principalVariation.moveCount = 1;
					if (rootMoves[i].myMove.isCap() == false)
						myKillers.cutoff(rootMoves[i].myMove, ply);
					return beta;
				}
				memcpy(principalVariation.moveLink + 1, localLine.moveLink, sizeof(int) * localLine.moveCount);
				principalVariation.moveCount = localLine.moveCount + 1;
				alpha = rootMoves[i].score;
			}
		}
		return alpha;
	}
	*/

	int Engine::alphaBeta(int depth, int ply, int alpha, int beta, line* pline, bool isNull, bool cutNode)
	{
		if (depth <= 0)
			return quiescentSearch(alpha, beta);

		bool pvNode = (beta - alpha != 1);
		// probe transposition table for cutoffs
		bool ttHit;
		hashEntry* TTentry = myHashTable.probe(myBoard->getCurrZ(), ttHit);
		if (ttHit) {
				pline->moveLink[0] = TTentry->hashMove;
				pline->moveCount = 1;
				if	(pvNode == false && 
					TTentry->hashDepth >= depth &&
					(TTentry->hashFlags == HASHEXACT ||
					(TTentry->hashFlags == HASHBETA && TTentry->hashEval >= beta) ||
					(TTentry->hashFlags == HASHALPHA && TTentry->hashEval <= alpha))) {
					return TTentry->hashEval;

				}
		}

		int score;
		bool inCheck = myBoard->isCheck();

		if (inCheck)
			goto movesLoop;

		if (pvNode == false && depth <= BETA_PRUNING_DEPTH) {
			score = (ttHit) ? TTentry->hashEval : negaEval();
			// beta pruning
			if (score - BETA_MARGIN * depth > beta)
				return score;
			// alpha pruning
			if (depth <= ALPHA_PRUNING_DEPTH && score + ALPHA_MARGIN <= alpha)
				return score;
		}
		// null move pruning
		if (isNull == false && myBoard->getGamePhase() >= NULLMOVE_THRESHOLD) {
			myBoard->movePiece(NULLMOVE);
			score = -alphaBeta(0, ply + 1, -beta, -beta + 1, nullptr, true, !cutNode);
			myBoard->unmovePiece();
			if (score >= beta)
				return beta;
		}

		if (pvNode && depth >= 6 && ttHit == false) 
			depth -= 2;
		
		if (cutNode && depth >= 9 && ttHit == false)
			--depth;

	movesLoop:

		MoveList localMoveList(myBoard, this, pline->moveLink[0], myKillers.getPrimary(ply), myKillers.getSecondary(ply));
		line localLine;
		unsigned evaltype = HASHALPHA;
		unsigned movesPlayed = 0;
		int R;
		int* hhp = NULL;
		for (unsigned genstate = GENPV; genstate < GENLOSECAPS; ++genstate)
		{
			localMoveList.moveOrder(genstate);
			while (localMoveList.movesLeft()) {
				++nodes;
				if(localMoveList.getCurrMove().isCap() == false)
					hhp = &hh[myBoard->getTurn()][localMoveList.getCurrMove().getFrom()][localMoveList.getCurrMove().getTo()];
				myBoard->movePiece(localMoveList.getCurrMove());
				if (myBoard->isPseudoRepititionDraw() || myBoard->isMaterialDraw())
					score = CONTEMPT;
				else {
					if (localMoveList.getCurrMove().isCap() == false && depth > 2 && movesPlayed != 0) {
						// initialize with LMR table
						R = LMRTable[depth][movesPlayed % 64];
						// increase for non pv nodes
						R += pvNode == false;
						// increase for check evasions with king
						R += inCheck && myBoard->getGridAt(localMoveList.getCurrMove().getTo()) / 2 == KING;
						// reduce for killers
						R -= genstate < GENQUIETS;
						// reduce based on history heuristic
						R -= MAX(-2, MIN(2, *hhp / 5000));
						R = MIN(depth - 1, MAX(R, 1));
						score = -alphaBeta(depth - R, ply + 1, -alpha - 1, -alpha, &localLine, false, true);
						if (score > alpha)
							score = -alphaBeta(depth - 1, ply + 1, -alpha - 1, -alpha, &localLine, false, !cutNode);
						if (pvNode && score > alpha)
							score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localLine, false, false);
					}
					else 
						score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localLine, false, false);
				}
				myBoard->unmovePiece();
				if (score > alpha) {
					pline->moveLink[0] = localMoveList.getCurrMove();
					memcpy(pline->moveLink + 1, localLine.moveLink, sizeof(int) * localLine.moveCount);
					pline->moveCount = localLine.moveCount + 1;
					if (score >= beta) {
						if (localMoveList.getCurrMove().isCap() == false) {
							myKillers.cutoff(localMoveList.getCurrMove(), ply);
							*hhp += 32 * MIN(depth * depth, 400) - * hhp * abs(MIN(depth * depth, 400)) / 512;
							Move m;
							while (localMoveList.rememberQuiets(m)) {
								hhp = &hh[myBoard->getTurn()][m.getFrom()][m.getTo()];
								*hhp -= 32 * MIN(depth * depth, 400) - *hhp * abs(MIN(depth * depth, 400)) / 512;
								if (*hhp < -33000)
									score = score;
							}
						}
						*TTentry = hashEntry(myBoard->getCurrZ(), depth, score, HASHBETA, localMoveList.getCurrMove());
						return beta;
					}
					evaltype = HASHEXACT;
					alpha = score;
				}
				++movesPlayed;
				localMoveList.increment();
			}

		}
		if (movesPlayed == 0)
			return (inCheck) ? -MATE + ply : CONTEMPT;
		else if (TTentry->hashDepth < depth)
			*TTentry = hashEntry(myBoard->getCurrZ(), depth, alpha, evaltype, pline->moveLink[0]);
		return alpha;
	}

	int Engine::quiescentSearch(int alpha, int beta)
	{
		bool ttHit;
		hashEntry* TTentry = myHashTable.probe(myBoard->getCurrZ(), ttHit);
		if (ttHit &&
			(TTentry->hashFlags == HASHEXACT ||
			(TTentry->hashFlags == HASHBETA && TTentry->hashEval >= beta) ||
			(TTentry->hashFlags == HASHALPHA && TTentry->hashEval <= alpha))) {
				return TTentry->hashEval;
		}
		int score = negaEval();
		if (myHashTable.getPawnZobrist(myBoard->getCurrP()) != myBoard->getCurrP())
			myHashTable.newPawnEntry(myBoard->getCurrP(), pawnEval());
		score += (myBoard->getTurn() == BLACK) ? -myHashTable.getPawnEval(myBoard->getCurrP()) : myHashTable.getPawnEval(myBoard->getCurrP());
		if (score >= beta) 
			return beta;
		else if (score > alpha)
			alpha = score;
		MoveList localMoveList(myBoard, this);
		localMoveList.moveOrder(GENWINCAPS);
		while (localMoveList.movesLeft()) {
			myBoard->movePiece(localMoveList.getCurrMove());
			score = -quiescentSearch(-beta, -alpha);
			myBoard->unmovePiece();
			if (score >= beta) {
				if (ttHit == false)
					*TTentry = hashEntry(myBoard->getCurrZ(), 0, score, HASHBETA, localMoveList.getCurrMove());
				return beta;
			}
			else if (score > alpha)
				alpha = score;
			localMoveList.increment();
		}
		return alpha;
	}

	void Engine::perftControl() {
		auto startTime = std::chrono::high_resolution_clock::now();
		for (int depth = 1; depth < myLimits.depth; ++depth) {
			unsigned n = perft(depth);
			auto stopTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime);
			std::cout << "info depth " << depth << " nodes " << n << " time " << (int)duration.count() << "\n";
		}
	}

	unsigned Engine::perft(int depth)
	{
		if (depth == 0)
			return 1;
		scoredMove allMoves[MEMORY];
		unsigned moveCount = myBoard->genAllMoves(allMoves);
		scoredMove allNonCapMoves[MEMORY];
		unsigned nonCapCount = myBoard->genAllNonCapMoves(allNonCapMoves);
		scoredMove allCapMoves[MEMORY];
		unsigned capCount = myBoard->genAllCapMoves(allCapMoves);
		if (moveCount != capCount + nonCapCount) {
			myBoard->drawBoard();
			std::string message;
			message = "All Moves: ";
			for (unsigned i = 0; i < moveCount; ++i) {
				message += {
					(char)(allMoves[i].myMove.getFrom() % WIDTH + 'a'),
						(char)((WIDTH - (int)allMoves[i].myMove.getFrom() / WIDTH) + '0'),
						(char)(allMoves[i].myMove.getTo() % WIDTH + 'a'),
						(char)(WIDTH - (int)(allMoves[i].myMove.getTo() / WIDTH) + '0'),
						(char)' ' };
			}
			std::cout << message << "\n";
			message = "Caps: ";
			for (unsigned i = 0; i < capCount; ++i) {
				message += {
					(char)(allCapMoves[i].myMove.getFrom() % WIDTH + 'a'),
						(char)((WIDTH - (int)allCapMoves[i].myMove.getFrom() / WIDTH) + '0'),
						(char)(allCapMoves[i].myMove.getTo() % WIDTH + 'a'),
						(char)(WIDTH - (int)(allCapMoves[i].myMove.getTo() / WIDTH) + '0'),
						(char)' ' };
			}
			std::cout << message << "\n";
			message = "NonCaps: ";
			for (unsigned i = 0; i < nonCapCount; ++i) {
				message += {
					(char)(allNonCapMoves[i].myMove.getFrom() % WIDTH + 'a'),
						(char)((WIDTH - (int)allNonCapMoves[i].myMove.getFrom() / WIDTH) + '0'),
						(char)(allNonCapMoves[i].myMove.getTo() % WIDTH + 'a'),
						(char)(WIDTH - (int)(allNonCapMoves[i].myMove.getTo() / WIDTH) + '0'),
						(char)' ' };
			}
			std::cout << message << "\n";
			moveCount = myBoard->genAllCapMoves(allMoves);
		}
		unsigned n = 0;
		for (unsigned i = 0; i < moveCount; ++i) {
			myBoard->movePiece(allMoves[i].myMove);
			n += perft(depth - 1);
			myBoard->unmovePiece();
		}
		return n;
	}
}