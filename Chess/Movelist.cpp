#include "Movelist.h"
#include "Board.h"
#include "Move.h"
#include <cmath>

namespace Hopper 
{
	MoveList::MoveList(Board* bd, Move pv, Move hash, Move killer) 
	{
		b = bd;
		for (int i = 0; i < GENEND; ++i) 
		{
			index[i] = 0;
			limit[i] = 0;
			//for (int j = 0; j < SPACES; ++j) 
			//	moves[i][j] = NULLMOVE;
		}
		state = GENPV;
		moves[GENPV][0] = pv;
		if (pv != hash) 
			moves[GENHASH][0] = hash;
		if (pv != killer && hash != killer) 
			moves[GENKILLS][0] = killer;
	}

	MoveList::MoveList(Board* bd) 
	{ 
		b = bd;
		for (int i = 0; i < GENEND; ++i) 
		{
			index[i] = 0;
			limit[i] = 0;
			//for (int j = 0; j < SPACES; ++j) 
			//	moves[i][j] = NULLMOVE;
		}
		state = GENPV;
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
		int i;
		state = genstate;
		switch (genstate) 
		{
		case GENPV:
		case GENHASH:
		case GENKILLS:
			if (b->validateMove(moves[genstate][0]))
				++limit[genstate];
			return;
		case GENWINCAPS:
			for (int j = 0; j < SPACES; ++j)
				moves[genstate][j] = NULLMOVE;
			limit[GENWINCAPS] = b->genAllCaps(moves[GENWINCAPS]);
			if (limit[GENPV] && moves[GENPV][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINCAPS]; ++i) 
				{
					if (moves[GENWINCAPS][i] == moves[GENPV][0]) 
					{
						moves[GENWINCAPS][i] = moves[GENWINCAPS][--limit[GENWINCAPS]];
						break;
					}
				}
			}
			if (limit[GENHASH] && moves[GENHASH][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINCAPS]; ++i) 
				{
					if (moves[GENWINCAPS][i] == moves[GENHASH][0]) 
					{
						moves[GENWINCAPS][i] = moves[GENWINCAPS][--limit[GENWINCAPS]];
						break;
					}
				}
			}
			for (i = 0; i < limit[GENWINCAPS]; ++i) 
			{
				if (!staticExchange(moves[GENWINCAPS][i], -30)) 
				{
					moves[GENLOSECAPS][limit[GENLOSECAPS]++] = moves[GENWINCAPS][i];
					moves[GENWINCAPS][i--] = moves[GENWINCAPS][--limit[GENWINCAPS]];
				}
			}
			return;
		case GENWINNONCAPS:
			limit[GENWINNONCAPS] = b->genAllNonCaps(moves[GENWINNONCAPS]);
			if (limit[GENPV] && !moves[GENPV][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINNONCAPS]; ++i) 
				{
					if (moves[GENWINNONCAPS][i] == moves[GENPV][0]) 
					{
						moves[GENWINNONCAPS][i] = moves[GENWINNONCAPS][--limit[GENWINNONCAPS]];
						break;
					}
				}
			}
			if (limit[GENHASH] && !moves[GENHASH][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINNONCAPS]; ++i) 
				{
					if (moves[GENWINNONCAPS][i] == moves[GENHASH][0]) 
					{
						moves[GENWINNONCAPS][i] = moves[GENWINNONCAPS][--limit[GENWINNONCAPS]];
						break;
					}
				}
			}
			if (limit[GENKILLS] && !moves[GENKILLS][0].isCap()) 
			{
				for (i = 0; i < limit[GENWINNONCAPS]; ++i) 
				{
					if (moves[GENWINNONCAPS][i] == moves[GENKILLS][0]) 
					{
						moves[GENWINNONCAPS][i] = moves[GENWINNONCAPS][--limit[GENWINNONCAPS]];
						break;
					}
				}
			}
			for (i = 0; i < limit[GENWINNONCAPS]; ++i) 
			{
				if (b->threatenedAt(!b->getTurn(), moves[GENWINNONCAPS][i].getTo()) && moves[GENWINNONCAPS][i].getFlags() < NPROMOTE)
				{
					moves[GENLOSENONCAPS][limit[GENLOSENONCAPS]++] = moves[GENWINNONCAPS][i];
					moves[GENWINNONCAPS][i--] = moves[GENWINNONCAPS][--limit[GENWINNONCAPS]];
				}
			}
			return;
		case GENLOSENONCAPS:
			return;
		case GENLOSECAPS:
			return;
		}
	}

	bool MoveList::staticExchange(Move m, int threshold)
	{
		if (!b->threatenedAt(!b->getTurn(), m.getTo()) || abs(b->gridAt(m.getTo())) >= abs(b->gridAt(m.getFrom())))
			return true;
		if (abs(b->gridAt(m.getTo())) <= abs(b->gridAt(m.getFrom()) && b->threatenedAt(!b->getTurn(), m.getTo()) > b->threatenedAt(b->getTurn(), m.getTo())))
			return false;
		bool tomove = b->getTurn();
		int to = m.getTo(), from = m.getFrom(), attackers[WIDTH * 2], total[2], see = abs(b->gridAt(to)), trophy = abs(b->gridAt(from)), smallestindex;
		for (int i = 0; i < 2; ++i)
		{
			total[i] = b->threatenedAt(i, to);
			for (int j = 0; j < total[i]; ++j)
				attackers[i * WIDTH + j] = b->getAttackers(i,j,to);
		}
		for (int i = 0; i < total[tomove]; ++i)
			if (attackers[tomove * WIDTH + i] == from)
				attackers[tomove * WIDTH + i] = attackers[tomove * WIDTH + --total[tomove]];
		while (total[!tomove])
		{
			tomove = !tomove;
			smallestindex = 0;
			for (int i = 1; i < total[tomove]; ++i)
				if (b->gridAt(attackers[tomove * WIDTH + i]) < b->gridAt(attackers[tomove * WIDTH + smallestindex]))
					smallestindex = i;
			see -= trophy;
			if (see >= threshold)
				return true;
			trophy = b->gridAt(attackers[tomove * WIDTH + smallestindex]);
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + --total[tomove]];
			tomove = !tomove;
			if (see + trophy < threshold || (!total[tomove] && see < threshold))
				return false;
			smallestindex = 0;
			for (int i = 1; i < total[tomove]; ++i)
				if (b->gridAt(attackers[tomove * WIDTH + i]) < b->gridAt(attackers[tomove * WIDTH + smallestindex]))
					smallestindex = i;
			see += trophy;
			trophy = b->gridAt(attackers[tomove * WIDTH + smallestindex]);
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + --total[tomove]];
			if (see - trophy >= threshold)
				return true;
			if (see < trophy)
				return false;
		}
		return see >= threshold;
	}
}