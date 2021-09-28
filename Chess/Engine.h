#pragma once

#include "Board.h"
#include "Hashtable.h"
#include "Pawnhash.h"
#include "Killers.h"
#include "Movelist.h"

namespace Hopper 
{
	typedef struct limits 
	{
		int time[2] = { 300000, 300000 };
		int inc[2] = { 10000, 10000 };
		int movetime = 0;
		int movesleft = 40;
		int depth = MAXDEPTH;
		int nodes = 0;
		int perft = 0;
		int mate = 0;
		bool infinite = false;
	} limits;

	class Engine 
	{
	public:
		Engine(Board* bd);
		void makeMove();
		int alphaBeta(int depth, int ply, int alpha, int beta, line* pline, bool isNull);
		limits myLimits;
	private:
		static int hypotenuse(const int a, const int b);
		int perft(int depth);
		int quiescentSearch(int alpha, int beta);
		int negaEval();
		int pawnEval();
		Board* myBoard;
		Killers myKillers;
		HashTable myHashTable;
		Pawnhash myPawnHashTable;
		int nodes = 0;
	};
}