#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <math.h>
#include <cstring>
#include "Engine.h"

namespace Hopper
{
	int LMRTable[MAXDEPTH + 1][64];

	Engine::Engine()
	{
		myHashTable.setSize(myLimits.hashbytes);
		initLMRTables();
		myEvaluate = Evaluate();
		lastEval = 0;
		abort = false;
	}

	void Engine::initLMRTables() 
	{
		for (unsigned depth = 1; depth <= MAXDEPTH; ++depth)
			for (unsigned ordered = 1; ordered < 64; ++ordered)
				LMRTable[depth][ordered] = (int) (0.75 + log(depth) * log(ordered) / 2.25);
	}

	Move Engine::getBestMove(Thread* myThreads)
	{//calls minimax and controls depth, alpha beta windows, and time
		auto startTime = std::chrono::high_resolution_clock::now();
		auto now = std::chrono::high_resolution_clock::now();
		std::vector<std::thread> processes;
		processes.resize(myThreads[0].nThreads);
		unsigned window = 25;
		int alpha = LOWERLIMIT, beta = UPPERLIMIT;
		int nodes;
		int score;//FIX ME FOR THREADS
		line principalVariation[10];// FIX ME FOR THREADS
		scoredMove rootMoves[MEMORY];
		unsigned nMoves = myThreads->myBoard.genAllMoves(rootMoves);
		if (nMoves == 1) {
			principalVariation[0].moveLink[0] = rootMoves[0].myMove;
			principalVariation[0].moveCount = 1;
		}
		else {
			bool fail = false;
			bool panic = false;
			bool consensus = true;
			bool TThit;
			Move moveHistory[MAXDEPTH];
			unsigned timeallotted = (myLimits.time[myThreads[0].myBoard.getTurn()] + myLimits.inc[myThreads[0].myBoard.getTurn()]) / (myLimits.movesleft + 1);
			for (int depth = 1; depth < myLimits.depth; ++depth) {
				nodes = 0;
				abort = false;
				for (int i = 0; i < myThreads[0].nThreads; ++i) {
					processes[i] = std::thread([this](Thread* myThread, int depth, int ply, int alpha, int beta, line* pline, bool cutNode) {
						alphaBeta(myThread, depth, ply, alpha, beta, pline, cutNode); }, 
						&myThreads[i], depth + i, 0, alpha, beta, &principalVariation[i], false);
				}
				for (int i = 0; i < myThreads[0].nThreads; ++i) {
					if (depth == MAXDEPTH - 1)
						i=i;
					processes[i].join();
					abort = true;
					nodes += myThreads[i].nodes;
				}
				score = myHashTable.probe(myThreads[0].myBoard.getCurrZ(), TThit)->hashEval;
				std::string message;
				std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " pv ";
				for (unsigned i = 0; i < principalVariation[0].moveCount; ++i) {
					message += {
						(char)(principalVariation[0].moveLink[i].getFrom() % WIDTH + 'a'),
							(char)(WIDTH - (int)(principalVariation[0].moveLink[i].getFrom() / WIDTH) + '0'),
							(char)(principalVariation[0].moveLink[i].getTo() % WIDTH + 'a'),
							(char)(WIDTH - (int)(principalVariation[0].moveLink[i].getTo() / WIDTH) + '0'),
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
					moveHistory[depth - 1] = principalVariation[0].moveLink[0];
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
					(now - startTime).count() > myLimits.time[myThreads[0].myBoard.getTurn()] / 3 ||
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
		myHashTable.clean();
		for (int i = 0; i < myThreads[0].nThreads; ++i) {
			myThreads[i].myKillers.chrono();
			myThreads[i].principalVariation.moveCount = 0;
		}
		return principalVariation[0].moveLink[0];
	}

	int Engine::alphaBeta(Thread* myThread, int depth, int ply, int alpha, int beta, line* pline, bool cutNode)
	{
		if (depth <= 0)
			return quiescentSearch(myThread, alpha, beta);
		++myThread->nodes;
		bool PVnode = beta - alpha != 1;
		//bool RootNode = ply == 0;
		// probe transposition table for cutoffs
		bool TThit;
		hashEntry* TTentry = myHashTable.probe(myThread->myBoard.getCurrZ(), TThit);
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
		bool inCheck = myThread->myBoard.isCheck();
		line localLine;

		if (inCheck)
			goto movesLoop;

		if (TThit == false) {
			score = (myThread->myBoard.getTurn() == BLACK) ? - myEvaluate.eval(&myThread->myBoard.myPosition) : myEvaluate.eval(&myThread->myBoard.myPosition);
			if (myHashTable.getPawnZobrist(myThread->myBoard.getCurrP()) != myThread->myBoard.getCurrP())
				myHashTable.newPawnEntry(myThread->myBoard.getCurrP(), myEvaluate.pawnEval(&myThread->myBoard.myPosition));
			score += (myThread->myBoard.getTurn() == BLACK) ? - myHashTable.getPawnEval(myThread->myBoard.getCurrP()) : myHashTable.getPawnEval(myThread->myBoard.getCurrP());
			if (TTentry->hashDepth == 0)
				*TTentry = hashEntry(myThread->myBoard.getCurrZ(), 0, score, HASHNONE, NULLMOVE);
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
			myThread->myBoard.getCurrM() != NULLMOVE && 
			depth >= NULLMOVE_PRUNING_DEPTH && 
			(TThit == false || (TTentry->hashFlags != HASHALPHA && TTentry->hashFlags != HASHNONE) || TTentry->hashEval >= beta)) {
			//&& myThread->myBoard.getGamePhase() >= NULLMOVE_THRESHOLD) {
			myThread->myBoard.movePiece(NULLMOVE);
			score = -alphaBeta(myThread, depth - 7 - depth / 6, ply + 1, -beta, -beta + 1, &localLine, !cutNode);
			myThread->myBoard.unmovePiece();
			if (score >= beta)
				return beta;
		}

		// internal iterative deepening
		if (TThit == false &&
			((PVnode && depth >= 6) || (PVnode == false && depth >= 8))) {
			int IIDdepth = PVnode ? depth - depth / 4 - 1 : (depth - 5) / 2;
			alphaBeta(myThread, IIDdepth, ply, alpha, beta, &localLine, cutNode);
			TTentry = myHashTable.probe(myThread->myBoard.getCurrZ(), TThit);
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

		MoveList localMoveList(myThread, pline->moveLink[0], myThread->myKillers.getPrimary(ply), myThread->myKillers.getSecondary(ply));
		unsigned evaltype = HASHALPHA;
		unsigned movesPlayed = 0;
		int R;
		int* HHentry = NULL;
		for (unsigned genstate = GENPV; genstate <= GENLOSECAPS; ++genstate)
		{
			localMoveList.moveOrder(genstate);
			while (localMoveList.movesLeft()) {
				if(localMoveList.getCurrMove().isCap() == false)
					HHentry = &myThread->HHtable[myThread->myBoard.getTurn()][localMoveList.getCurrMove().getFrom()][localMoveList.getCurrMove().getTo()];
				myThread->myBoard.movePiece(localMoveList.getCurrMove());
				if (myThread->myBoard.isPseudoRepititionDraw() || myThread->myBoard.isMaterialDraw())
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
						R += inCheck && myThread->myBoard.getGridAt(localMoveList.getCurrMove().getTo()) / 2 == KING;
						// reduce for killers
						R -= genstate < GENQUIETS;
						// reduce based on history heuristic
						R -= MAX(-2, MIN(2, *HHentry / 5000));
						R = MIN(depth - 1, MAX(R, 1));
						score = -alphaBeta(myThread, depth - R, ply + 1, -alpha - 1, -alpha, &localLine, true);
						if (score > alpha)
							score = -alphaBeta(myThread, depth - 1, ply + 1, -alpha - 1, -alpha, &localLine, !cutNode);
						if (PVnode && score > alpha)
							score = -alphaBeta(myThread, depth - 1, ply + 1, -beta, -alpha, &localLine, false);
					}
					else 
						score = -alphaBeta(myThread, depth - 1, ply + 1, -beta, -alpha, &localLine, false);
				}
				myThread->myBoard.unmovePiece();
				if (score > alpha) {
					pline->moveLink[0] = localMoveList.getCurrMove();
					memcpy(pline->moveLink + 1, localLine.moveLink, sizeof(int) * localLine.moveCount);
					pline->moveCount = localLine.moveCount + 1;
					if (score >= beta) {
						if (localMoveList.getCurrMove().isCap() == false) {
							myThread->myKillers.cutoff(localMoveList.getCurrMove(), ply);
							int bonus = MIN(depth * depth, 400);
							*HHentry += 32 * bonus - * HHentry * bonus / 512; // abs(bonus) not needed
							Move m;
							while (localMoveList.rememberQuiets(m)) {
								HHentry = &myThread->HHtable[myThread->myBoard.getTurn()][m.getFrom()][m.getTo()];
								*HHentry += 32 * (-bonus) - *HHentry * bonus / 512;
							}
						}
						if (TTentry->hashDepth <= depth)
							*TTentry = hashEntry(myThread->myBoard.getCurrZ(), depth, score, HASHBETA, localMoveList.getCurrMove());
						return beta;
					}
					evaltype = HASHEXACT;
					alpha = score;
				}
				++movesPlayed;
				localMoveList.increment();
				if (abort)
					return alpha;
			}

		}
		if (movesPlayed == 0)
			return (inCheck) ? -MATE + ply : CONTEMPT;
		else if (TTentry->hashDepth <= depth)
			*TTentry = hashEntry(myThread->myBoard.getCurrZ(), depth, alpha, evaltype, pline->moveLink[0]);
		return alpha;
	}

	int Engine::quiescentSearch(Thread* myThread, int alpha, int beta)
	{
		++myThread->nodes;
		bool TThit;
		hashEntry* TTentry = myHashTable.probe(myThread->myBoard.getCurrZ(), TThit);
		if (TThit) {
			if (TTentry->hashFlags == HASHEXACT ||
				(TTentry->hashFlags == HASHBETA && TTentry->hashEval >= beta) ||
				(TTentry->hashFlags == HASHALPHA && TTentry->hashEval <= alpha))
				return TTentry->hashEval;
		}
		int score = (myThread->myBoard.getTurn() == BLACK) ? - myEvaluate.eval(&myThread->myBoard.myPosition) : myEvaluate.eval(&myThread->myBoard.myPosition);
		if (myHashTable.getPawnZobrist(myThread->myBoard.getCurrP()) != myThread->myBoard.getCurrP())
			myHashTable.newPawnEntry(myThread->myBoard.getCurrP(), myEvaluate.pawnEval(&myThread->myBoard.myPosition));
		score += (myThread->myBoard.getTurn() == BLACK) ? - myHashTable.getPawnEval(myThread->myBoard.getCurrP()) : myHashTable.getPawnEval(myThread->myBoard.getCurrP());
		if (TThit == false)
			*TTentry = hashEntry(myThread->myBoard.getCurrZ(), 0, score, HASHNONE, NULLMOVE);
		if (score >= beta)
			return beta;
		int oldAlpha = alpha;
		if (score > alpha)
			alpha = score;
		MoveList localMoveList(myThread, TTentry->hashMove);
		scoredMove best = scoredMove(LOWERLIMIT);
		for (unsigned genState = TTentry->hashMove.isCap() ? GENPV : GENWINCAPS; genState <= GENWINCAPS; ++genState) {
			localMoveList.moveOrder(genState);
			while (localMoveList.movesLeft()) {
				myThread->myBoard.movePiece(localMoveList.getCurrMove());
				score = -quiescentSearch(myThread, -beta, -alpha);
				myThread->myBoard.unmovePiece();
				if (score > best.score) {
					if (score >= beta) {
						if (TTentry->hashDepth == 0)
							*TTentry = hashEntry(myThread->myBoard.getCurrZ(), 0, score, HASHBETA, localMoveList.getCurrMove());
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
			*TTentry = hashEntry(myThread->myBoard.getCurrZ(), 0, best.score, best.score > oldAlpha ? HASHEXACT : HASHALPHA, best.myMove);
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
		/*
		if (depth == 0)
			return 1;
		scoredMove allMoves[MEMORY];
		unsigned moveCount = myThread->myBoard.genAllMoves(allMoves);
		scoredMove allNonCapMoves[MEMORY];
		unsigned nonCapCount = myThread->myBoard.genAllNonCapMoves(allNonCapMoves);
		scoredMove allCapMoves[MEMORY];
		unsigned capCount = myThread->myBoard.genAllCapMoves(allCapMoves);
		if (moveCount != capCount + nonCapCount) {
			myThread->myBoard.drawBoard();
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
			moveCount = myThread->myBoard.genAllCapMoves(allMoves);
		}
		unsigned n = 0;
		for (unsigned i = 0; i < moveCount; ++i) {
			myThread->myBoard.movePiece(allMoves[i].myMove);
			n += perft(depth - 1);
			myThread->myBoard.unmovePiece();
		}
		return n;
		*/
		return 0;
	}
}