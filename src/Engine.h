#pragma once

#include "Board.h"
#include "Hashtable.h"
#include "Killers.h"
#include "Movelist.h"

namespace Hopper
{
	typedef struct limits
	{
		unsigned time[2] = { 300000, 300000 };
		unsigned inc[2] = { 10000, 10000 };
		unsigned movetime = 0;
		unsigned movesleft = 10;
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
		int alphaBeta(int depth, int ply, int alpha, int beta, line* pline, bool isNull);
		int alphaBetaRoot(int depth, int ply, int alpha, int beta, line* pline, bool isNull);
		void perftControl();
		limits myLimits;
	private:
		static int hypotenuse(const int a, const int b);
		unsigned perft(int depth);
		int quiescentSearch(int alpha, int beta);
		void initEvalTables();
		void initLMRTables();
		int negaEval();
		int pawnEval();
		Board* myBoard;
		Killers myKillers;
		HashTable myHashTable;
		unsigned nodes;
		int lastEval;
	};
}