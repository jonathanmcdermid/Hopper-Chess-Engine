#include "Movelist.h"
#include "Board.h"
#include "Move.h"
#include <cmath>

namespace Hopper 
{
	MoveList::MoveList(Board* bd, Move pv, Move hash, Move killer) 
	{
		generationState = GENPV;
		myBoard = bd;
		for (int i = 0; i < GENEND; ++i) 
		{
			index[i] = 0;
			limit[i] = 0;
			for (int j = 0; j < SPACES; ++j) 
				sortedMoves[i][j] = NULLMOVE;
		}
		sortedMoves[GENPV][0] = pv;
		if (pv != hash) 
			sortedMoves[GENHASH][0] = hash;
		if (pv != killer && hash != killer) 
			sortedMoves[GENKILLS][0] = killer;
	}

	MoveList::MoveList(Board* bd) 
	{ 
		generationState = GENPV;
		myBoard = bd;
		for (int i = 0; i < GENEND; ++i) 
		{
			index[i] = 0;
			limit[i] = 0;
			for (int j = 0; j < SPACES; ++j) 
				sortedMoves[i][j] = NULLMOVE;
		}
	}


	const bool MoveList::noMoves() 
	{ 
		for (int i = 0; i < GENEND; ++i)
			if (limit[i]) 
				return false;
		return true; 
	}

	void MoveList::moveOrder(int genstate) 
	{
		generationState = genstate;
		int i;
		switch (genstate) 
		{
		case GENPV:
		case GENHASH:
		case GENKILLS:
			if (myBoard->validateMove(sortedMoves[generationState][0]))
				++limit[generationState];
			break;
		case GENWINCAPS:
			for (int j = 0; j < SPACES; ++j)
				sortedMoves[generationState][j] = NULLMOVE;
			limit[GENWINCAPS] = myBoard->genAllCapMoves(sortedMoves[GENWINCAPS]);
			if (limit[GENPV] && sortedMoves[GENPV][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINCAPS]; ++i) 
				{
					if (sortedMoves[GENWINCAPS][i] == sortedMoves[GENPV][0]) 
					{
						sortedMoves[GENWINCAPS][i] = sortedMoves[GENWINCAPS][--limit[GENWINCAPS]];
						break;
					}
				}
			}
			if (limit[GENHASH] && sortedMoves[GENHASH][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINCAPS]; ++i) 
				{
					if (sortedMoves[GENWINCAPS][i] == sortedMoves[GENHASH][0]) 
					{
						sortedMoves[GENWINCAPS][i] = sortedMoves[GENWINCAPS][--limit[GENWINCAPS]];
						break;
					}
				}
			}
			for (i = 0; i < limit[GENWINCAPS]; ++i) 
			{
				if (!staticExchange(sortedMoves[GENWINCAPS][i], -30)) 
				{
					sortedMoves[GENLOSECAPS][limit[GENLOSECAPS]++] = sortedMoves[GENWINCAPS][i];
					sortedMoves[GENWINCAPS][i--] = sortedMoves[GENWINCAPS][--limit[GENWINCAPS]];
				}
			}
			break;
		case GENWINNONCAPS:
			limit[GENWINNONCAPS] = myBoard->genAllNonCapMoves(sortedMoves[GENWINNONCAPS]);
			if (limit[GENPV] && !sortedMoves[GENPV][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINNONCAPS]; ++i) 
				{
					if (sortedMoves[GENWINNONCAPS][i] == sortedMoves[GENPV][0]) 
					{
						sortedMoves[GENWINNONCAPS][i] = sortedMoves[GENWINNONCAPS][--limit[GENWINNONCAPS]];
						break;
					}
				}
			}
			if (limit[GENHASH] && !sortedMoves[GENHASH][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINNONCAPS]; ++i) 
				{
					if (sortedMoves[GENWINNONCAPS][i] == sortedMoves[GENHASH][0]) 
					{
						sortedMoves[GENWINNONCAPS][i] = sortedMoves[GENWINNONCAPS][--limit[GENWINNONCAPS]];
						break;
					}
				}
			}
			if (limit[GENKILLS] && !sortedMoves[GENKILLS][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINNONCAPS]; ++i) 
				{
					if (sortedMoves[GENWINNONCAPS][i] == sortedMoves[GENKILLS][0]) 
					{
						sortedMoves[GENWINNONCAPS][i] = sortedMoves[GENWINNONCAPS][--limit[GENWINNONCAPS]];
						break;
					}
				}
			}
			for (i = 0; i < limit[GENWINNONCAPS]; ++i) 
			{
				if (myBoard->getThreatenedAt(!myBoard->getTurn(), sortedMoves[GENWINNONCAPS][i].getTo()) && sortedMoves[GENWINNONCAPS][i].getFlags() < NPROMOTE)
				{
					sortedMoves[GENLOSENONCAPS][limit[GENLOSENONCAPS]++] = sortedMoves[GENWINNONCAPS][i];
					sortedMoves[GENWINNONCAPS][i--] = sortedMoves[GENWINNONCAPS][--limit[GENWINNONCAPS]];
				}
			}
			break;
		case GENLOSENONCAPS:
			break;
		case GENLOSECAPS:
			break;
		}
	}

	bool MoveList::staticExchange(Move nextMove, int threshold)
	{
		if (!myBoard->getThreatenedAt(!myBoard->getTurn(), nextMove.getTo()) || abs(myBoard->getGridAt(nextMove.getTo())) >= abs(myBoard->getGridAt(nextMove.getFrom())))
			return true;
		if (abs(myBoard->getGridAt(nextMove.getTo())) <= abs(myBoard->getGridAt(nextMove.getFrom()) && myBoard->getThreatenedAt(!myBoard->getTurn(), nextMove.getTo()) > myBoard->getThreatenedAt(myBoard->getTurn(), nextMove.getTo())))
			return false;
		bool tomove = myBoard->getTurn();
		int to = nextMove.getTo(), from = nextMove.getFrom(), attackers[WIDTH * 2], total[2], see = abs(myBoard->getGridAt(to)), trophy = abs(myBoard->getGridAt(from)), smallestindex;
		for (int i = 0; i < 2; ++i)
		{
			total[i] = myBoard->getThreatenedAt(i, to);
			for (int j = 0; j < total[i]; ++j)
				attackers[i * WIDTH + j] = myBoard->getAttackersAt(i,j,to);
		}
		for (int i = 0; i < total[tomove]; ++i)
			if (attackers[tomove * WIDTH + i] == from)
				attackers[tomove * WIDTH + i] = attackers[tomove * WIDTH + --total[tomove]];
		while (total[!tomove])
		{
			tomove = !tomove;
			smallestindex = 0;
			for (int i = 1; i < total[tomove]; ++i)
				if (myBoard->getGridAt(attackers[tomove * WIDTH + i]) < myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]))
					smallestindex = i;
			see -= trophy;
			if (see >= threshold)
				return true;
			trophy = myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]);
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + --total[tomove]];
			tomove = !tomove;
			if (see + trophy < threshold || (!total[tomove] && see < threshold))
				return false;
			smallestindex = 0;
			for (int i = 1; i < total[tomove]; ++i)
				if (myBoard->getGridAt(attackers[tomove * WIDTH + i]) < myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]))
					smallestindex = i;
			see += trophy;
			trophy = myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]);
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + --total[tomove]];
			if (see - trophy >= threshold)
				return true;
			if (see < trophy)
				return false;
		}
		return see >= threshold;
	}
}