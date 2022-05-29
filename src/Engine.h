#pragma once

#include "Board.h"
#include "Hashtable.h"
#include "Killers.h"
#include "Movelist.h"
#include "Evaluate.h"

namespace Hopper
{

	typedef struct limits
	{
		unsigned time[2] = { 300000, 300000 };
		unsigned inc[2] = { 10000, 10000 };
		unsigned movetime = 0;
		unsigned movesleft = 20;
		int depth = MAXDEPTH;
		unsigned nodes = 0;
		unsigned perft = 0;
		unsigned mate = 0;
		unsigned hashbytes = 220000000;
		bool infinite = false;
	} limits;

	typedef struct Thread
	{
		Thread(Board* b = NULL, int n = 0) {
			nThreads = n;
			myBoard = *b;
			nodes = 0;
			depth = 0;
			memset(HHtable, 0, sizeof(HHtable));
		}
		Board myBoard;
		int nThreads = 0;
		int nodes = 0;
		int depth = 0;
		int HHtable[2][SPACES][SPACES];
		line principalVariation;
		Killers myKillers;
	} Thread;

	class Engine
	{
	public:
		Engine();
		Move getBestMove(Thread* myThreads);
		void setHashSize(unsigned s);
		void perftControl();
		void flushTT() { myHashTable.flush(); }
		limits myLimits;
	private:
		unsigned perft(int depth);
		int alphaBeta(Thread* myThread, int depth, int ply, int alpha, int beta, line* pline, bool cutNode);
		int quiescentSearch(Thread* myThread, int alpha, int beta);
		void initLMRTables();
		HashTable myHashTable;
		Evaluate myEvaluate;
		int lastEval;
		bool abort;
	};
}