#include <algorithm>
#include <cstring>
#include "Movelist.h"
#include "Board.h"

namespace Hopper
{
	static int piece_values[6] = { 94, 281, 297, 512,  936,  0 };

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
		index = 0;
		memset(limit, 0, sizeof(limit));
		storedMoves[GENPV][0].myMove = pv;
		if(hash != pv)
			storedMoves[GENHASH][0].myMove = hash;
		if (primary != hash && primary != pv)
			storedMoves[GENKILLPRIMARY][0].myMove = primary;
		if (secondary != primary && secondary != hash && secondary != pv)
			storedMoves[GENKILLSECONDARY][0].myMove = secondary;
	}

	bool MoveList::noMoves()const
	{
		for (unsigned i = 0; i < GENEND; ++i) {
			if (limit[i])
				return false;
		}
		return true;
	}

	void MoveList::MVVLVA() {
		for (unsigned i = 0; i < limit[GENWINCAPS]; ++i) {
			storedMoves[GENWINCAPS][i].score = 1 << (6 + myBoard->getGridAt(storedMoves[GENWINCAPS][i].myMove.getTo()) / 2) |
											   1 << (5 - myBoard->getGridAt(storedMoves[GENWINCAPS][i].myMove.getFrom()) / 2);
		}
		std::sort(storedMoves[GENWINCAPS], storedMoves[GENWINCAPS] + limit[GENWINCAPS], smScoreComp);
	}

	void MoveList::removeDuplicate(unsigned gs) {
		if (limit[gs]) {
			unsigned i = index_of(storedMoves[generationState], storedMoves[generationState] + limit[generationState], storedMoves[gs][0]);
			if (i != limit[generationState])
				storedMoves[generationState][i] = storedMoves[generationState][--limit[generationState]];
		}
	}

	void MoveList::increment() { 
		++index;
		//if (generationState != GENWINCAPS)
		//	return;
		//if (!staticExchange(storedMoves[GENWINCAPS][index[GENWINCAPS]].myMove)) {
		//	storedMoves[GENLOSECAPS][limit[GENLOSECAPS]] = storedMoves[GENWINCAPS][index[GENWINCAPS]];
		//	++limit[GENLOSECAPS];
		//	++index[GENWINCAPS];
		//}
	}

	void MoveList::moveOrder(unsigned gs)
	{
		index = 0;
		generationState = gs;
		switch (gs) {
		case GENPV:
		case GENHASH:
		case GENKILLPRIMARY:
		case GENKILLSECONDARY:
			if (storedMoves[generationState][0].myMove != NULLMOVE && myBoard->validateMove(storedMoves[generationState][0].myMove))
				++limit[generationState];
			break;
		case GENWINCAPS:
			limit[GENWINCAPS] = myBoard->genAllCapMoves(storedMoves[GENWINCAPS]);
			removeDuplicate(GENPV);
			removeDuplicate(GENHASH);
			MVVLVA();
			break;
		case GENNONCAPS:
			limit[GENNONCAPS] = myBoard->genAllNonCapMoves(storedMoves[GENNONCAPS]);
			removeDuplicate(GENPV);
			removeDuplicate(GENHASH);
			removeDuplicate(GENKILLPRIMARY);
			removeDuplicate(GENKILLSECONDARY);
			std::sort(storedMoves[GENNONCAPS], storedMoves[GENNONCAPS] + limit[GENNONCAPS], smFlagsComp);
			break;
		case GENLOSECAPS:
			break;
		}
	}

	bool MoveList::staticExchange(Move nextMove)
	{
		bool tomove = myBoard->getTurn();
		unsigned to = nextMove.getTo(), from = nextMove.getFrom();
		int attackers[WIDTH * 2];
		unsigned total[2];
		int see = piece_values[myBoard->getGridAt(to) / 2];
		int trophy = piece_values[myBoard->getGridAt(from) / 2];
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
				if (piece_values[myBoard->getGridAt(attackers[tomove * WIDTH + i]) / 2] < 
					piece_values[myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]) / 2])
					smallestindex = i;
			see -= trophy;
			trophy = piece_values[myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]) / 2];
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + total[tomove] - 1];
			--total[tomove];
			tomove = !tomove;
			if (see > SEE_THRESHOLD)
				return true;
			else if (total[tomove] == 0)
				return false;
			smallestindex = 0;
			for (unsigned i = 1; i < total[tomove]; ++i)
				if (piece_values[myBoard->getGridAt(attackers[tomove * WIDTH + i]) / 2] <
					piece_values[myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]) / 2])
					smallestindex = i;
			see += trophy;
			trophy = piece_values[myBoard->getGridAt(attackers[tomove * WIDTH + smallestindex]) / 2];
			attackers[tomove * WIDTH + smallestindex] = attackers[tomove * WIDTH + --total[tomove]];
		}
		return see > SEE_THRESHOLD;
	}
}