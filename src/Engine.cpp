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
		initLMRTables();
		myEvaluate = Evaluate();
		memset(HHtable, 0, sizeof(HHtable));
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
			bool fail = false;
			bool panic = false;
			bool consensus = true;
			Move moveHistory[MAXDEPTH];
			unsigned timeallotted = (myLimits.time[myBoard->getTurn()] + myLimits.inc[myBoard->getTurn()]) / (myLimits.movesleft + 1);
			for (int depth = 1; depth < myLimits.depth; ++depth) {
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
				if (score <= alpha || score >= beta) {
					fail = true;
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
					fail = false;
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
				if (score >= MATE - MAXDEPTH || score <= -MATE + MAXDEPTH ||
					// if PV is a draw in less moves than the depth searched, no point in continuing
					//(score == CONTEMPT && principalVariation.moveCount < depth && fail == false) ||
					// if we are over time limit, need to stop search unless we are significantly lower than previous eval
					std::chrono::duration_cast<std::chrono::milliseconds >
					(now - startTime).count() > timeallotted ||
					// if we are about to get flagged, end no matter what 
					std::chrono::duration_cast<std::chrono::milliseconds>
					(now - startTime).count() > myLimits.time[myBoard->getTurn()] / 3 ||
					// if the past few iterative searches have yielded the same move, we will stop early
					(consensus == true && panic == false && 
					std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() > timeallotted / CONSENSUS_REDUCTION_FACTOR) ||
					// if engine was in panic state but has now found a move, we can stop
					(panic == true && score >= lastEval - PANIC_THRESHOLD)) {
					// if we are over our time limit but we cant find a good move, take more time and panic
					if (score < lastEval - PANIC_THRESHOLD && panic == false) {
						timeallotted *= 1 + MIN((lastEval - score) / PANIC_THRESHOLD, PANIC_EXTENSION_LIMIT);
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

	int Engine::alphaBeta(int depth, int ply, int alpha, int beta, line* pline, bool cutNode)
	{
		if (depth <= 0)
			return quiescentSearch(alpha, beta);
		++nodes;
		bool PVnode = beta - alpha != 1;
		//bool RootNode = ply == 0;
		// probe transposition table for cutoffs
		bool TThit;
		hashEntry* TTentry = myHashTable.probe(myBoard->getCurrZ(), TThit);
		if (TThit) {
				pline->moveLink[0] = TTentry->hashMove;
				pline->moveCount = 1;
				if	(PVnode == false && 
					 TTentry->hashDepth >= depth &&
					(TTentry->hashFlags == HASHEXACT ||
					(TTentry->hashFlags == HASHBETA && TTentry->hashEval >= beta) ||
					(TTentry->hashFlags == HASHALPHA && TTentry->hashEval <= alpha))) {
					return TTentry->hashEval;
				}
		}

		int score;
		bool inCheck = myBoard->isCheck();
		line localLine;

		if (inCheck)
			goto movesLoop;

		if (TThit == false) {
			score = (myBoard->getTurn() == BLACK) ? - myEvaluate.eval(&myBoard->myPosition) : myEvaluate.eval(&myBoard->myPosition);
			if (myHashTable.getPawnZobrist(myBoard->getCurrP()) != myBoard->getCurrP())
				myHashTable.newPawnEntry(myBoard->getCurrP(), myEvaluate.pawnEval(&myBoard->myPosition));
			score += (myBoard->getTurn() == BLACK) ? - myHashTable.getPawnEval(myBoard->getCurrP()) : myHashTable.getPawnEval(myBoard->getCurrP());
			if (TTentry->hashDepth == 0)
				*TTentry = hashEntry(myBoard->getCurrZ(), 0, score, HASHNONE, NULLMOVE);
		}
		else
			score = TTentry->hashEval;

		if (PVnode == false && depth <= BETA_PRUNING_DEPTH) {
			// beta pruning
			if (score - BETA_MARGIN * depth > beta)
				return score;
			// alpha pruning
			if (depth <= ALPHA_PRUNING_DEPTH && score + ALPHA_MARGIN <= alpha)
				return score;
		}

		// null move pruning
		if (PVnode == false && 
			myBoard->getCurrM() != NULLMOVE && 
			depth >= NULLMOVE_PRUNING_DEPTH && 
			(TThit == false || (TTentry->hashFlags != HASHALPHA && TTentry->hashFlags != HASHNONE) || TTentry->hashEval >= beta)) {
			//&& myBoard->getGamePhase() >= NULLMOVE_THRESHOLD) {
			myBoard->movePiece(NULLMOVE);
			score = -alphaBeta(depth - 7 - depth / 6, ply + 1, -beta, -beta + 1, &localLine, !cutNode);
			myBoard->unmovePiece();
			if (score >= beta)
				return beta;
		}

		// internal iterative deepening
		if (TThit == false &&
			((PVnode && depth >= 6) || (PVnode == false && depth >= 8))) {
			int IIDdepth = PVnode ? depth - depth / 4 - 1 : (depth - 5) / 2;
			alphaBeta(IIDdepth, ply, alpha, beta, &localLine, cutNode);
			TTentry = myHashTable.probe(myBoard->getCurrZ(), TThit);
			if (TThit) {
				pline->moveLink[0] = TTentry->hashMove;
				pline->moveCount = 1;
			}
		}

		if (PVnode && depth >= 6 && TThit == false) 
			depth -= 2;
		
		if (cutNode && depth >= 9 && TThit == false)
			--depth;

	movesLoop:

		MoveList localMoveList(myBoard, this, pline->moveLink[0], myKillers.getPrimary(ply), myKillers.getSecondary(ply));
		unsigned evaltype = HASHALPHA;
		unsigned movesPlayed = 0;
		int R;
		int* HHentry = NULL;
		for (unsigned genstate = GENPV; genstate <= GENLOSECAPS; ++genstate)
		{
			localMoveList.moveOrder(genstate);
			while (localMoveList.movesLeft()) {
				if(localMoveList.getCurrMove().isCap() == false)
					HHentry = &HHtable[myBoard->getTurn()][localMoveList.getCurrMove().getFrom()][localMoveList.getCurrMove().getTo()];
				myBoard->movePiece(localMoveList.getCurrMove());
				if (myBoard->isPseudoRepititionDraw() || myBoard->isMaterialDraw())
					score = CONTEMPT;
				else {
					if (localMoveList.getCurrMove().isCap() == false && 
						depth > 2 && 
						movesPlayed != 0) {
						// initialize with LMR table
						R = LMRTable[depth][movesPlayed % 64];
						// increase for non pv nodes
						R += PVnode == false;
						// increase for check evasions with king
						R += inCheck && myBoard->getGridAt(localMoveList.getCurrMove().getTo()) / 2 == KING;
						// reduce for killers
						R -= genstate < GENQUIETS;
						// reduce based on history heuristic
						R -= MAX(-2, MIN(2, *HHentry / 5000));
						R = MIN(depth - 1, MAX(R, 1));
						score = -alphaBeta(depth - R, ply + 1, -alpha - 1, -alpha, &localLine, true);
						if (score > alpha)
							score = -alphaBeta(depth - 1, ply + 1, -alpha - 1, -alpha, &localLine, !cutNode);
						if (PVnode && score > alpha)
							score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localLine, false);
					}
					else 
						score = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &localLine, false);
				}
				myBoard->unmovePiece();
				if (score > alpha) {
					pline->moveLink[0] = localMoveList.getCurrMove();
					memcpy(pline->moveLink + 1, localLine.moveLink, sizeof(int) * localLine.moveCount);
					pline->moveCount = localLine.moveCount + 1;
					if (score >= beta) {
						if (localMoveList.getCurrMove().isCap() == false) {
							myKillers.cutoff(localMoveList.getCurrMove(), ply);
							int bonus = MIN(depth * depth, 400);
							*HHentry += 32 * bonus - * HHentry * bonus / 512; // abs(bonus) not needed
							Move m;
							while (localMoveList.rememberQuiets(m)) {
								HHentry = &HHtable[myBoard->getTurn()][m.getFrom()][m.getTo()];
								*HHentry += 32 * (-bonus) - *HHentry * bonus / 512;
							}
						}
						if (TTentry->hashDepth <= depth)
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
		else if (TTentry->hashDepth <= depth)
			*TTentry = hashEntry(myBoard->getCurrZ(), depth, alpha, evaltype, pline->moveLink[0]);
		return alpha;
	}

	int Engine::quiescentSearch(int alpha, int beta)
	{
		++nodes;
		bool TThit;
		hashEntry* TTentry = myHashTable.probe(myBoard->getCurrZ(), TThit);
		if (TThit) {
			if (TTentry->hashFlags == HASHEXACT ||
				(TTentry->hashFlags == HASHBETA && TTentry->hashEval >= beta) ||
				(TTentry->hashFlags == HASHALPHA && TTentry->hashEval <= alpha))
				return TTentry->hashEval;
		}
		int score = (myBoard->getTurn() == BLACK) ? - myEvaluate.eval(&myBoard->myPosition) : myEvaluate.eval(&myBoard->myPosition);
		if (myHashTable.getPawnZobrist(myBoard->getCurrP()) != myBoard->getCurrP())
			myHashTable.newPawnEntry(myBoard->getCurrP(), myEvaluate.pawnEval(&myBoard->myPosition));
		score += (myBoard->getTurn() == BLACK) ? - myHashTable.getPawnEval(myBoard->getCurrP()) : myHashTable.getPawnEval(myBoard->getCurrP());
		if (TThit == false)
			*TTentry = hashEntry(myBoard->getCurrZ(), 0, score, HASHNONE, NULLMOVE);
		if (score >= beta)
			return beta;
		int oldAlpha = alpha;
		if (score > alpha)
			alpha = score;
		MoveList localMoveList(myBoard, this, TTentry->hashMove);
		scoredMove best = scoredMove(LOWERLIMIT);
		for (unsigned genState = TTentry->hashMove.isCap() ? GENPV : GENWINCAPS; genState <= GENWINCAPS; ++genState) {
			localMoveList.moveOrder(genState);
			while (localMoveList.movesLeft()) {
				myBoard->movePiece(localMoveList.getCurrMove());
				score = -quiescentSearch(-beta, -alpha);
				myBoard->unmovePiece();
				if (score > best.score) {
					if (score >= beta) {
						if (TTentry->hashDepth == 0)
							*TTentry = hashEntry(myBoard->getCurrZ(), 0, score, HASHBETA, localMoveList.getCurrMove());
						return beta;
					}
					best.score = score;
					best.myMove = localMoveList.getCurrMove();
					if (score > alpha)
						alpha = score;
				}
				localMoveList.increment();
			}
		}
		if (best.score != LOWERLIMIT && TTentry->hashDepth == 0)
			*TTentry = hashEntry(myBoard->getCurrZ(), 0, best.score, best.score > oldAlpha ? HASHEXACT : HASHALPHA, best.myMove);
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