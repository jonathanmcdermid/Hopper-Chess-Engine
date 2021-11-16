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

	class Engine
	{
	public:
		Engine(Board* bd);
		void makeMove();
		void perftControl();
		limits myLimits;
		int HHtable[2][SPACES][SPACES];
	private:
		unsigned perft(int depth);
		int alphaBeta(int depth, int ply, int alpha, int beta, line* pline, bool cutNode);
		int quiescentSearch(int alpha, int beta);
		void initLMRTables();
		Board* myBoard;
		Killers myKillers;
		HashTable myHashTable;
		Evaluate myEvaluate;
		unsigned nodes;
		int lastEval;
	};
}