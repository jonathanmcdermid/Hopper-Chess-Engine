#include <algorithm>
#include <cstring>
#include "Movelist.h"
#include "Board.h"
#include "Engine.h"

namespace Hopper
{
	static int piece_values[6] = { 94, 281, 297, 512,  936,  25000 };

	template <typename Iter>
	unsigned index_of(Iter first, Iter last, typename std::iterator_traits<Iter>::value_type& x)
	{
		unsigned i = 0;
		while (first != last && *first != x)
			++first, ++i;
		return i;
	}

	MoveList::MoveList(Board* bd, Engine* e, Move pv, Move primary, Move secondary)
	{
		generationState = GENPV;
		myBoard = bd;
		myEngine = e;
		index = 0;
		memoryIndex = 0;
		limit = 0;
		pvMove.myMove = pv;
		if (primary != pv)
			primaryMove.myMove = primary;
		if (secondary != primary && secondary != pv)
			secondaryMove.myMove = secondary;
		playSpecial = false;
	}

	void MoveList::MVVLVA() 
	{
		for (unsigned i = 0; i < limit; ++i) {
			if (storedMoves[i].myMove.isCap()) {
				storedMoves[i].score = (storedMoves[i].myMove.getFlags() / NPROMOTE) & 1 << 15 |
					1 << (6 + myBoard->getGridAt(storedMoves[i].myMove.getTo()) / 2) |
					1 << (5 - myBoard->getGridAt(storedMoves[i].myMove.getFrom()) / 2);
			}
		}
		std::sort(storedMoves, storedMoves + limit, smScoreComp);
	}

	void MoveList::scoreQuiets() 
	{
		for (unsigned i = 0; i < limit; ++i) {
			if (storedMoves[i].myMove.isCap() == false) {
				storedMoves[i].score = (storedMoves[i].myMove.getFlags() / NPROMOTE) & 1 << 15 |
					1 << ((myBoard->getGridAt(storedMoves[i].myMove.getFrom()) / 2) % KING);
				storedMoves[i].score += myEngine->hh[myBoard->getTurn()][storedMoves[i].myMove.getFrom()][storedMoves[i].myMove.getTo()];
			}
		}
		std::sort(storedMoves, storedMoves + limit, smScoreComp);
	}

	void MoveList::removeDuplicate(scoredMove m) 
	{
		unsigned i = index_of(storedMoves, storedMoves + limit, m);
		if (i != limit)
			storedMoves[i] = storedMoves[--limit];
	}

	void MoveList::increment()
	{
		switch (generationState) {
		case GENPV:
		case GENKILLPRIMARY:
		case GENKILLSECONDARY:
			playSpecial = false;
			break;
		case GENWINCAPS:
		case GENLOSECAPS:
			do { ++index; } while (index < limit && storedMoves[index].myMove.isCap() == false);
			break;
		case GENQUIETS:
			do { ++index; } while (index < limit && storedMoves[index].myMove.isCap() == true);
			break;
		}
	}

	bool MoveList::rememberQuiets(Move& m)
	{
		while(memoryIndex < index && storedMoves[memoryIndex].myMove.isCap() == true){
			++memoryIndex;
		}
		if (memoryIndex == index) {
			return false;
		}
		else {
			m = storedMoves[memoryIndex++].myMove;
			return true;
		}
	}

	bool MoveList::movesLeft() 
	{ 
		switch (generationState) {
		case GENPV:
		case GENKILLPRIMARY:
		case GENKILLSECONDARY:
			return playSpecial;
		default:
			return index < limit;
		}
	}

	Move MoveList::getCurrMove() 
	{ 
		switch (generationState) {
		case GENPV:
			return pvMove.myMove;
		case GENKILLPRIMARY:
			return primaryMove.myMove;
		case GENKILLSECONDARY:
			return secondaryMove.myMove;
		default:
			return storedMoves[index].myMove;
		}
	}

	void MoveList::moveOrder(unsigned gs)
	{
		generationState = gs;
		playSpecial = false;
		memoryIndex = 0;
		switch (generationState) {
		case GENPV:
			if (myBoard->validateMove(pvMove.myMove)) {
				playSpecial = true;
			}
			break;
		case GENKILLPRIMARY:
			if (myBoard->validateMove(primaryMove.myMove)) {
				playSpecial = true;
			}
			break;
		case GENKILLSECONDARY:
			if (myBoard->validateMove(secondaryMove.myMove)) {
				playSpecial = true;
			}
			break;
		case GENWINCAPS:
			index = 0;
			limit += myBoard->genAllCapMoves(&storedMoves[limit]);
			if (pvMove.myMove.isCap())		removeDuplicate(pvMove);
			MVVLVA();
			break;
		case GENQUIETS:
			index = 0;
			limit += myBoard->genAllNonCapMoves(&storedMoves[limit]);
			if (!pvMove.myMove.isCap())		removeDuplicate(pvMove);
			removeDuplicate(primaryMove);
			removeDuplicate(secondaryMove);
			scoreQuiets();
			while (index < limit && storedMoves[index].myMove.isCap() == true) ++index;
			break;
		case GENLOSECAPS:
			index = limit;
			break;
		}
	}
}