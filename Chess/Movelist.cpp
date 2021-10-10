#include <algorithm>
#include <cstring>
#include "Movelist.h"
#include "Board.h"

namespace Hopper
{
	template <typename Iter>
	unsigned index_of(Iter first, Iter last, typename std::iterator_traits<Iter>::value_type& x)
	{
		unsigned i = 0;
		while (first != last && *first != x)
			++first, ++i;
		return i;
	}

	MoveList::MoveList(Board* bd, Move pv, Move hash, Move primary, Move secondary)
	{
		generationState = GENPV;
		myBoard = bd;
		memset(index, 0, sizeof(index));
		memset(limit, 0, sizeof(limit));
		memset(sortedMoves, 0, sizeof(sortedMoves));
		sortedMoves[GENPV][0] = pv;
		if(hash != pv)
			sortedMoves[GENHASH][0] = hash;
		if (primary != hash && primary != pv)
			sortedMoves[GENKILLPRIMARY][0] = primary;
		if (secondary != primary && secondary != hash && secondary != pv)
			sortedMoves[GENKILLSECONDARY][0] = secondary;
	}

	bool MoveList::noMoves()const
	{
		for (unsigned i = 0; i < GENEND; ++i) {
			if (limit[i])
				return false;
		}
		return true;
	}

	inline static bool moveFlagSort(Move const& lhs, Move const& rhs) {
		return lhs.getFlags() > rhs.getFlags();
	}

	void MoveList::removeDuplicate(unsigned gs) {
		if (limit[gs]) {
			unsigned i = index_of(sortedMoves[generationState], sortedMoves[generationState] + limit[generationState], sortedMoves[gs][0]);
			if (i != limit[generationState])
				sortedMoves[generationState][i] = sortedMoves[generationState][--limit[generationState]];
		}
	}

	void MoveList::staticSort() {
		for (unsigned i = 0; i < limit[GENWINCAPS]; ++i) {
			if (!staticExchange(sortedMoves[GENWINCAPS][i], -30)) {
				sortedMoves[GENLOSECAPS][limit[GENLOSECAPS]++] = sortedMoves[GENWINCAPS][i];
				sortedMoves[GENWINCAPS][i--] = sortedMoves[GENWINCAPS][--limit[GENWINCAPS]];
			}
		}
	}

	void MoveList::moveOrder(unsigned gs)
	{
		generationState = gs;
		switch (gs) {
		case GENPV:
		case GENHASH:
		case GENKILLPRIMARY:
		case GENKILLSECONDARY:
			if (sortedMoves[generationState][0] != NULLMOVE && myBoard->validateMove(sortedMoves[generationState][0]))
				++limit[generationState];
			break;
		case GENWINCAPS:
			limit[GENWINCAPS] = myBoard->genAllCapMoves(sortedMoves[GENWINCAPS]);
			removeDuplicate(GENPV);
			removeDuplicate(GENHASH);
			staticSort();
			break;
		case GENNONCAPS:
			limit[GENNONCAPS] = myBoard->genAllNonCapMoves(sortedMoves[GENNONCAPS]);
			removeDuplicate(GENPV);
			removeDuplicate(GENHASH);
			removeDuplicate(GENKILLPRIMARY);
			removeDuplicate(GENKILLSECONDARY);
			std::sort(sortedMoves[GENNONCAPS], sortedMoves[GENNONCAPS] + limit[GENNONCAPS], moveFlagSort);
			break;
		case GENLOSECAPS:
			break;
		}
	}

	bool MoveList::staticExchange(Move nextMove, int threshold)
	{
		bool tomove = myBoard->getTurn();
		unsigned to = nextMove.getTo(), from = nextMove.getFrom();
		int attackers[WIDTH * 2];
		unsigned total[2];
		int see = abs(myBoard->getGridAt(to));
		int trophy = abs(myBoard->getGridAt(from));
		unsigned smallestindex;
		for (unsigned i = 0; i < 2; ++i) {
			total[i] = myBoard->getThreatenedAt(i, to);
			for (unsigned j = 0; j < total[i]; ++j)
				attackers[i * WIDTH + j] = myBoard->getAttackersAt(i, j, to);
		}
		for (unsigned i = 0; i < total[tomove]; ++i)
			if (attackers[tomove * WIDTH + i] == from)
				attackers[tomove * WIDTH + i] = attackers[tomove * WIDTH + --total[tomove]];
		while (total[!tomove]) {
			tomove = !tomove;
			smallestindex = 0;
			for (unsigned i = 1; i < total[tomove]; ++i)
				if (abs(myBoard->getGridAt(attackers[tomove * WIDTH + i])) < abs(myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex])))
					smallestindex = i;
			see -= trophy;
			trophy = abs(myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]));
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + total[tomove] - 1];
			--total[tomove];
			tomove = !tomove;
			if (see > threshold)
				return true;
			else if (!total[tomove])
				return false;
			smallestindex = 0;
			for (unsigned i = 1; i < total[tomove]; ++i)
				if (abs(myBoard->getGridAt(attackers[tomove * WIDTH + i])) < abs(myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex])))
					smallestindex = i;
			see += trophy;
			trophy = abs(myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]));
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + --total[tomove]];
		}
		return see > threshold;
	}
}