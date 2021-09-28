#include "Movelist.h"
#include "Board.h"
#include "Move.h"

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
		if(pv != NULLMOVE)
			sortedMoves[GENPV][0] = pv;
		if (pv != hash && hash != NULLMOVE)
			sortedMoves[GENHASH][0] = hash;
		if (pv != killer && hash != killer && killer != NULLMOVE)
			sortedMoves[GENKILLS][0] = killer;
	}

	bool MoveList::noMoves()const
	{
		for (int i = 0; i < GENEND; ++i)
			if (limit[i])
				return false;
		return true;
	}

	bool moveFlagSort(Move const& lhs, Move const& rhs) {
		return lhs.getFlags() > rhs.getFlags();
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
			for (int i = 0; i < SPACES; ++i)
				sortedMoves[generationState][i] = NULLMOVE;
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
		case GENNONCAPS:
			limit[GENNONCAPS] = myBoard->genAllNonCapMoves(sortedMoves[GENNONCAPS]);
			if (limit[GENPV] && !sortedMoves[GENPV][0].isCap())
			{
				for (i = 0; i < limit[GENNONCAPS]; ++i)
				{
					if (sortedMoves[GENNONCAPS][i] == sortedMoves[GENPV][0])
					{
						sortedMoves[GENNONCAPS][i] = sortedMoves[GENNONCAPS][--limit[GENNONCAPS]];
						break;
					}
				}
			}
			if (limit[GENHASH] && !sortedMoves[GENHASH][0].isCap())
			{
				for (i = 0; i < limit[GENNONCAPS]; ++i)
				{
					if (sortedMoves[GENNONCAPS][i] == sortedMoves[GENHASH][0])
					{
						sortedMoves[GENNONCAPS][i] = sortedMoves[GENNONCAPS][--limit[GENNONCAPS]];
						break;
					}
				}
			}
			if (limit[GENKILLS] && !sortedMoves[GENKILLS][0].isCap())
			{
				for (i = 0; i < limit[GENNONCAPS]; ++i)
				{
					if (sortedMoves[GENNONCAPS][i] == sortedMoves[GENKILLS][0])
					{
						sortedMoves[GENNONCAPS][i] = sortedMoves[GENNONCAPS][--limit[GENNONCAPS]];
						break;
					}
				}
			}
			std::sort(sortedMoves[GENNONCAPS], sortedMoves[GENNONCAPS] + limit[GENNONCAPS], moveFlagSort);
			break;
		case GENLOSECAPS:
			break;
		}
	}

	bool MoveList::staticExchange(Move nextMove, int threshold)
	{
		bool tomove = myBoard->getTurn();
		int to = nextMove.getTo(), from = nextMove.getFrom();
		int attackers[WIDTH * 2];
		int total[2];
		int see = abs(myBoard->getGridAt(to));
		int trophy = abs(myBoard->getGridAt(from));
		int smallestindex;
		for (int i = 0; i < 2; ++i)
		{
			total[i] = myBoard->getThreatenedAt(i, to);
			for (int j = 0; j < total[i]; ++j)
				attackers[i * WIDTH + j] = myBoard->getAttackersAt(i, j, to);
		}
		for (int i = 0; i < total[tomove]; ++i)
			if (attackers[tomove * WIDTH + i] == from)
				attackers[tomove * WIDTH + i] = attackers[tomove * WIDTH + --total[tomove]];
		while (total[!tomove])
		{
			tomove = !tomove;
			smallestindex = 0;
			for (int i = 1; i < total[tomove]; ++i)
				if (abs(myBoard->getGridAt(attackers[tomove * WIDTH + i])) < abs(myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex])))
					smallestindex = i;
			see -= trophy;
			trophy = abs(myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]));
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + --total[tomove]];
			tomove = !tomove;
			if (see > threshold)
				return true;
			else if (!total[tomove])
				return false;
			smallestindex = 0;
			for (int i = 1; i < total[tomove]; ++i)
				if (abs(myBoard->getGridAt(attackers[tomove * WIDTH + i])) < abs(myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex])))
					smallestindex = i;
			see += trophy;
			trophy = abs(myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]));
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + --total[tomove]];
		}
		return see > threshold;
	}
}