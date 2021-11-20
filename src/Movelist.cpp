#include <algorithm>
#include <cstring>
#include "Movelist.h"
#include "Board.h"
#include "Engine.h"

namespace Hopper
{
	static int see_piece_values[7] = { 100, 300, 315, 500,  900,  25000, 0 };

	template <typename Iter>
	unsigned index_of(Iter first, Iter last, typename std::iterator_traits<Iter>::value_type& x)
	{
		unsigned i = 0;
		while (first != last && *first != x)
			++first, ++i;
		return i;
	}

	MoveList::MoveList(Thread* t, Move pv, Move primary, Move secondary)
	{
		generationState = GENPV;
		myThread = t;
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
					1 << (6 + myThread->myBoard.getGridAt(storedMoves[i].myMove.getTo()) / 2) |
					1 << (5 - myThread->myBoard.getGridAt(storedMoves[i].myMove.getFrom()) / 2);
			}
		}
		std::sort(storedMoves, storedMoves + limit, smScoreComp);
	}

	void MoveList::scoreQuiets() 
	{
		for (unsigned i = 0; i < limit; ++i) {
			if (storedMoves[i].myMove.isCap() == false) {
				storedMoves[i].score = (storedMoves[i].myMove.getFlags() / NPROMOTE) & 1 << 15 |
					1 << ((myThread->myBoard.getGridAt(storedMoves[i].myMove.getFrom()) / 2) % KING);
				storedMoves[i].score += myThread->HHtable[myThread->myBoard.getTurn()][storedMoves[i].myMove.getFrom()][storedMoves[i].myMove.getTo()];
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
			while (true) {
				do { ++index; } while (index < limit && storedMoves[index].myMove.isCap() == false);
				if (index >= limit || SEEcontrol())
					break;
				else
					storedMoves[index].score = 0;
			}
			break;
		case GENLOSECAPS:
			do { ++index; } while (index < limit && (storedMoves[index].myMove.isCap() == false || storedMoves[index].score != 0));
			break;
		case GENQUIETS:
			do { ++index; } while (index < limit && storedMoves[index].myMove.isCap() == true);
			break;
		}
	}

	bool MoveList::rememberQuiets(Move& m)
	{
		while(memoryIndex < index && storedMoves[memoryIndex].myMove.isCap() == true){ ++memoryIndex; }
		if (memoryIndex == index) { return false; }
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
			if (myThread->myBoard.validateMove(pvMove.myMove)) { playSpecial = true; }
			break;
		case GENKILLPRIMARY:
			if (myThread->myBoard.validateMove(primaryMove.myMove)) { playSpecial = true; }
			break;
		case GENKILLSECONDARY:
			if (myThread->myBoard.validateMove(secondaryMove.myMove)) {playSpecial = true; }
			break;
		case GENWINCAPS:
			index = 0;
			limit += myThread->myBoard.genAllCapMoves(&storedMoves[limit]);
			if (pvMove.myMove.isCap() == true) removeDuplicate(pvMove);
			MVVLVA();
			while (index < limit && SEEcontrol() == false) { storedMoves[index++].score = 0; }
			break;
		case GENQUIETS:
			index = 0;
			limit += myThread->myBoard.genAllNonCapMoves(&storedMoves[limit]);
			if (pvMove.myMove.isCap() == false)	removeDuplicate(pvMove);
			removeDuplicate(primaryMove);
			removeDuplicate(secondaryMove);
			scoreQuiets();
			while (index < limit && storedMoves[index].myMove.isCap() == true) ++index;
			break;
		case GENLOSECAPS:
			index = 0;
			while (index < limit && (storedMoves[index].myMove.isCap() == false || storedMoves[index].score != 0)) ++index;
			break;
		}
	}

	bool MoveList::SEEcontrol()
	{
		// handles en passants and promotions in the laziest way possible
		if (storedMoves[index].myMove.getFlags() == ENPASSANT)
			return true;
		bool side = myThread->myBoard.getTurn();
		unsigned to = storedMoves[index].myMove.getTo(), from = storedMoves[index].myMove.getFrom();
		int see = see_piece_values[myThread->myBoard.getGridAt(to) / 2];
		int trophy = see_piece_values[myThread->myBoard.getGridAt(from) / 2];
		unsigned smallestIndex;
		for (unsigned i = 0; i < 2; ++i) {
			total[i] = myThread->myBoard.getThreatenedAt(i, to);
			for (unsigned j = 0; j < total[i]; ++j) {
				smallestIndex = i * WIDTH + j;
				attackers[smallestIndex] = myThread->myBoard.getAttackersAt(i, j, to);
				// handles promotions, assumes queen value
				attackerValues[smallestIndex] =
					((myThread->myBoard.getGridAt(attackers[smallestIndex]) == WHITE_PAWN && attackers[smallestIndex] < 16) ||
						(myThread->myBoard.getGridAt(attackers[smallestIndex]) == BLACK_PAWN && attackers[smallestIndex] > 47)) ?
					see_piece_values[QUEEN] :
					see_piece_values[myThread->myBoard.getGridAt(attackers[smallestIndex]) / 2];
			}
		}
		for (unsigned i = 0; i < total[side]; ++i) {
			if (attackers[side * WIDTH + i] == from) {
				attackers[side * WIDTH + i] = attackers[side * WIDTH + total[side] - 1];
				attackerValues[side * WIDTH + i] = attackerValues[side * WIDTH + total[side] - 1];
				--total[side];
				updateHiddenAttackers(from, to);
				break;
			}
		}
		while (total[!side]) {
			side = !side;
			smallestIndex = 0;
			for (unsigned i = 1; i < total[side]; ++i)
				if (attackerValues[side * WIDTH + i] < attackerValues[side * WIDTH + smallestIndex])
					smallestIndex = i;
			see -= trophy;
			trophy = attackerValues[side * WIDTH + smallestIndex];
			updateHiddenAttackers(attackers[side * WIDTH + smallestIndex], to);
			attackers[side * WIDTH + smallestIndex] = attackers[side * WIDTH + total[side] - 1];
			attackerValues[side * WIDTH + smallestIndex] = attackerValues[side * WIDTH + total[side] - 1];
			--total[side];
			side = !side;
			if (see > SEE_THRESHOLD)
				return true;
			else if (total[side] == 0)
				return false;
			smallestIndex = 0;
			for (unsigned i = 1; i < total[side]; ++i)
				if (attackerValues[side * WIDTH + i] < attackerValues[side * WIDTH + smallestIndex])
					smallestIndex = i;
			see += trophy;
			trophy = attackerValues[side * WIDTH + smallestIndex];
			updateHiddenAttackers(attackers[side * WIDTH + smallestIndex], to);
			attackers[side * WIDTH + smallestIndex] = attackers[side * WIDTH + total[side] - 1];
			attackerValues[side * WIDTH + smallestIndex] = attackerValues[side * WIDTH + total[side] - 1];
			--total[side];
		}
		return see > SEE_THRESHOLD;
	}

	void MoveList::handleEnPassant(int from, int to)
	{
		if (from > to) { // white
			for (int i = to + 2 * BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
				switch (myThread->myBoard.getGridAt(i))
				{
				case WHITE_ROOK:
				case WHITE_QUEEN:
					attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
					attackers[WHITE * WIDTH + total[WHITE]++] = i;
					return;
				case BLACK_ROOK:
				case BLACK_QUEEN:
					attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
					attackers[BLACK * WIDTH + total[BLACK]++] = i;
					return;
				case EMPTY:
					continue;
				default:
					return;
				}
			}
		}
		else { // black
			for (int i = to + 2 * BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
				switch (myThread->myBoard.getGridAt(i))
				{
				case WHITE_ROOK:
				case WHITE_QUEEN:
					attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
					attackers[WHITE * WIDTH + total[WHITE]++] = i;
					return;
				case BLACK_ROOK:
				case BLACK_QUEEN:
					attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
					attackers[BLACK * WIDTH + total[BLACK]++] = i;
					return;
				case EMPTY:
					continue;
				default:
					return;
				}
			}
		}
	}

	void MoveList::updateHiddenAttackers(int from, int to) {
		int i;
		switch (myThread->myBoard.getGridAt(from) / 2) {
		case PAWN:
		case QUEEN:
		case BISHOP:
			if (NESWslide(from, to)) {
				if (from > to) {
					for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
						switch (myThread->myBoard.getGridAt(i))
						{
						case WHITE_BISHOP:
						case WHITE_QUEEN:
							attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[WHITE * WIDTH + total[WHITE]++] = i;
							return;
						case BLACK_BISHOP:
						case BLACK_QUEEN:
							attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[BLACK * WIDTH + total[BLACK]++] = i;
							return;
						case EMPTY:
							continue;
						default:
							return;
						}
					}
				}
				else {
					for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
						switch (myThread->myBoard.getGridAt(i))
						{
						case WHITE_BISHOP:
						case WHITE_QUEEN:
							attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[WHITE * WIDTH + total[WHITE]++] = i;
							return;
						case BLACK_BISHOP:
						case BLACK_QUEEN:
							attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[BLACK * WIDTH + total[BLACK]++] = i;
							return;
						case EMPTY:
							continue;
						default:
							return;
						}
					}
				}
				return;
			}
			else if (NWSEslide(from,to)) {
				if (from > to) {
					for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
						switch (myThread->myBoard.getGridAt(i))
						{
						case WHITE_BISHOP:
						case WHITE_QUEEN:
							attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[WHITE * WIDTH + total[WHITE]++] = i;
							return;
						case BLACK_BISHOP:
						case BLACK_QUEEN:
							attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[BLACK * WIDTH + total[BLACK]++] = i;
							return;
						case EMPTY:
							continue;
						default:
							return;
						}
					}
				}
				else {
					for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
						switch (myThread->myBoard.getGridAt(i))
						{
						case WHITE_BISHOP:
						case WHITE_QUEEN:
							attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[WHITE * WIDTH + total[WHITE]++] = i;
							return;
						case BLACK_BISHOP:
						case BLACK_QUEEN:
							attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[BLACK * WIDTH + total[BLACK]++] = i;
							return;
						case EMPTY:
							continue;
						default:
							return;
						}
					}
				}
				return;
			}
		case ROOK:
			if (NSslide(from, to)) {
				if (from > to) {
					for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
						switch (myThread->myBoard.getGridAt(i))
						{
						case WHITE_ROOK:
						case WHITE_QUEEN:
							attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[WHITE * WIDTH + total[WHITE]++] = i;
							return;
						case BLACK_ROOK:
						case BLACK_QUEEN:
							attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[BLACK * WIDTH + total[BLACK]++] = i;
							return;
						case EMPTY:
							continue;
						default:
							return;
						}
					}
				}
				else {
					for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
						switch (myThread->myBoard.getGridAt(i))
						{
						case WHITE_ROOK:
						case WHITE_QUEEN:
							attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[WHITE * WIDTH + total[WHITE]++] = i;
							return;
						case BLACK_ROOK:
						case BLACK_QUEEN:
							attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[BLACK * WIDTH + total[BLACK]++] = i;
							return;
						case EMPTY:
							continue;
						default:
							return;
						}
					}
				}
				return;
			}
			else if (EWslide(from, to)) {
				if (from > to) {
					for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
						switch (myThread->myBoard.getGridAt(i))
						{
						case WHITE_ROOK:
						case WHITE_QUEEN:
							attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[WHITE * WIDTH + total[WHITE]++] = i;
							return;
						case BLACK_ROOK:
						case BLACK_QUEEN:
							attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[BLACK * WIDTH + total[BLACK]++] = i;
							return;
						case EMPTY:
							continue;
						default:
							return;
						}
					}
				}
				else {
					for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
						switch (myThread->myBoard.getGridAt(i))
						{
						case WHITE_ROOK:
						case WHITE_QUEEN:
							attackerValues[WHITE * WIDTH + total[WHITE]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[WHITE * WIDTH + total[WHITE]++] = i;
							return;
						case BLACK_ROOK:
						case BLACK_QUEEN:
							attackerValues[BLACK * WIDTH + total[BLACK]] = see_piece_values[myThread->myBoard.getGridAt(i) / 2];
							attackers[BLACK * WIDTH + total[BLACK]++] = i;
							return;
						case EMPTY:
							continue;
						default:
							return;
						}
					}
				}
				return;
			}
			break;
		}
	}
}