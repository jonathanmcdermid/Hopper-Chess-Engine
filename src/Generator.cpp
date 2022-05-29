#include "Board.h"
#include <cmath>
#include <cassert>

namespace Hopper
{
	bool Board::validateMove(Move nextMove)
	{//validates a single move
		if (!validPiece(myPosition.grid[nextMove.getFrom()], turn))
			return false;
		scoredMove temp[28];
		int moveCount;
		moveCount = genAllMovesAt(temp, nextMove.getFrom());
		assert(moveCount <= 28);
		moveCount = removeIllegalMoves(temp, moveCount);
		for (int i = 0; i < moveCount; ++i) {
			if (temp[i].myMove == nextMove)
				return true;
		}
		return false;
	}

	Move Board::createMove(int from, int to)
	{
		scoredMove temp[28];
		unsigned moveCount = genAllMovesAt(temp, from);
		assert(moveCount <= 28);
		moveCount = removeIllegalMoves(temp, moveCount);
		for (unsigned i = 0; i < moveCount; ++i) {
			if (temp[i].myMove.getFrom() == from && temp[i].myMove.getTo() == to)
				return temp[i].myMove;
		}
		return NULLMOVE;
	}

	unsigned Board::removeIllegalMoves(scoredMove* nextMove, unsigned moveCount)
	{
		for (unsigned i = 0; i < moveCount; ++i) {
			if (myPosition.grid[nextMove[i].myMove.getFrom()] == WHITE_KING || myPosition.grid[nextMove[i].myMove.getFrom()] == BLACK_KING) {
				if (threatened[!turn][nextMove[i].myMove.getTo()]) {
					nextMove[i--] = nextMove[--moveCount];
				}
				else if (turn == WHITE) {
					if (nextMove[i].myMove.getFlags() == KCASTLE && (threatened[BLACK][60] || threatened[BLACK][61]))
						nextMove[i--] = nextMove[--moveCount];
					else if (nextMove[i].myMove.getFlags() == QCASTLE && (threatened[BLACK][60] || threatened[BLACK][59]))
						nextMove[i--] = nextMove[--moveCount];
				}
				else {
					if (nextMove[i].myMove.getFlags() == KCASTLE && (threatened[WHITE][4] || threatened[WHITE][5]))
						nextMove[i--] = nextMove[--moveCount];
					else if (nextMove[i].myMove.getFlags() == QCASTLE && (threatened[WHITE][4] || threatened[WHITE][3]))
						nextMove[i--] = nextMove[--moveCount];
				}
			}
		}
		for (int i = 0; i < pinCount; ++i) {
			for (unsigned j = 0; j < moveCount; ++j) {
				if (nextMove[j].myMove.getFrom() == pinnedPieces[i]) {
					switch (pinnedPieces[i + 5]) {
					case BOARD_SOUTH:
					case BOARD_NORTH:
						if (NSslide(nextMove[j].myMove.getFrom(), nextMove[j].myMove.getTo()))
							continue;
						else
							break;
					case BOARD_WEST:
					case BOARD_EAST:
						if (EWslide(nextMove[j].myMove.getFrom(), nextMove[j].myMove.getTo()))
							continue;
						else
							break;
					case BOARD_NORTHEAST:
					case BOARD_SOUTHWEST:
						if (NESWslide(nextMove[j].myMove.getFrom(), nextMove[j].myMove.getTo()))
							continue;
						else
							break;
					case BOARD_NORTHWEST:
					case BOARD_SOUTHEAST:
						if (NWSEslide(nextMove[j].myMove.getFrom(), nextMove[j].myMove.getTo()))
							continue;
						else
							break;
					}
					nextMove[j--] = nextMove[--moveCount];
				}
			}
		}
		for (int i = 0; i < threatened[!turn][kingPos[turn]]; ++i) {
			int checktype;
			if (myPosition.grid[attackers[!turn][i][kingPos[turn]]] <= BLACK_KNIGHT)
				checktype = BOARD_LEAP;
			else if (NSslide(attackers[!turn][i][kingPos[turn]], kingPos[turn]))
				checktype = (attackers[!turn][i][kingPos[turn]] > kingPos[turn]) ? BOARD_NORTH : BOARD_SOUTH;
			else if (EWslide(attackers[!turn][i][kingPos[turn]], kingPos[turn]))
				checktype = (attackers[!turn][i][kingPos[turn]] > kingPos[turn]) ? BOARD_WEST : BOARD_EAST;
			else if (NESWslide(attackers[!turn][i][kingPos[turn]], kingPos[turn]))
				checktype = (attackers[!turn][i][kingPos[turn]] > kingPos[turn]) ? BOARD_NORTHEAST : BOARD_SOUTHWEST;
			else
				checktype = (attackers[!turn][i][kingPos[turn]] > kingPos[turn]) ? BOARD_NORTHWEST : BOARD_SOUTHEAST;
			for (unsigned j = 0; j < moveCount; ++j) {
				if (nextMove[j].myMove.getFrom() != kingPos[turn]) {
					if (checktype != BOARD_LEAP && ((nextMove[j].myMove.getTo() - attackers[!turn][i][kingPos[turn]]) % checktype
						|| (nextMove[j].myMove.getTo() < attackers[!turn][i][kingPos[turn]] && nextMove[j].myMove.getTo() < kingPos[turn])
						|| (nextMove[j].myMove.getTo() > attackers[!turn][i][kingPos[turn]] && nextMove[j].myMove.getTo() > kingPos[turn])))
						nextMove[j--] = nextMove[--moveCount];
					else if (checktype == BOARD_LEAP && nextMove[j].myMove.getTo() != attackers[!turn][i][kingPos[turn]] && nextMove[j].myMove.getFlags() != ENPASSANT)
						nextMove[j--] = nextMove[--moveCount];
				}
				else if (checktype != BOARD_LEAP && nextMove[j].myMove.getTo() - nextMove[j].myMove.getFrom() == checktype)
					nextMove[j--] = nextMove[--moveCount];
			}
		}
		return moveCount;
	}

	unsigned Board::genAllMoves(scoredMove* nextMove)
	{//generates all legal moves
		unsigned moveCount = 0;
		for (unsigned from = 0; from < SPACES; ++from) {
			if (validPiece(myPosition.grid[from], turn))
				moveCount += genAllMovesAt(&nextMove[moveCount], from);
		}
		assert(moveCount <= MEMORY);
		moveCount = removeIllegalMoves(nextMove, moveCount);
		return moveCount;
	}

	unsigned Board::genAllCapMoves(scoredMove* nextMove)
	{
		unsigned moveCount = 0;
		for (unsigned from = 0; from < SPACES; ++from) {
			if (validPiece(myPosition.grid[from], turn))
				moveCount += genCapMovesAt(&nextMove[moveCount], from);
		}
		assert(moveCount <= SPACES);
		moveCount = removeIllegalMoves(nextMove, moveCount);
		return moveCount;
	}

	unsigned Board::genAllNonCapMoves(scoredMove* nextMove)
	{
		unsigned moveCount = 0;
		for (unsigned from = 0; from < SPACES; ++from) {
			if (validPiece(myPosition.grid[from], turn))
				moveCount += genNonCapMovesAt(&nextMove[moveCount], from);
		}
		assert(moveCount <= SPACES);
		moveCount = removeIllegalMoves(nextMove, moveCount);
		return moveCount;
	}

	unsigned Board::genCapMovesAt(scoredMove* nextMove, int from)
	{//generates all pseudo legal capture moves for one piece
		int i;
		unsigned moveCount = 0;
		switch (myPosition.grid[from]) {
		case WHITE_KING:
		case BLACK_KING:
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && enemyPiece(myPosition.grid[from + BOARD_SOUTHEAST], turn))
				nextMove[moveCount++].myMove = Move(from, from + BOARD_SOUTHEAST, CAPTURE);
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH && enemyPiece(myPosition.grid[from + BOARD_EAST], turn))
				nextMove[moveCount++].myMove = Move(from, from + BOARD_EAST, CAPTURE);
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8 && enemyPiece(myPosition.grid[from + BOARD_NORTHWEST], turn))
				nextMove[moveCount++].myMove = Move(from, from + BOARD_NORTHWEST, CAPTURE);
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0 && enemyPiece(myPosition.grid[from + BOARD_WEST], turn))
				nextMove[moveCount++].myMove = Move(from, from + BOARD_WEST, CAPTURE);
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && enemyPiece(myPosition.grid[from + BOARD_SOUTHWEST], turn))
				nextMove[moveCount++].myMove = Move(from, from + BOARD_SOUTHWEST, CAPTURE);
			if (from < 56 && enemyPiece(myPosition.grid[from + BOARD_SOUTH], turn))
				nextMove[moveCount++].myMove = Move(from, from + BOARD_SOUTH, CAPTURE);
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6 && enemyPiece(myPosition.grid[from + BOARD_NORTHEAST], turn))
				nextMove[moveCount++].myMove = Move(from, from + BOARD_NORTHEAST, CAPTURE);
			if (from > 7 && enemyPiece(myPosition.grid[from + BOARD_NORTH], turn))
				nextMove[moveCount++].myMove = Move(from, from + BOARD_NORTH, CAPTURE);
			return moveCount;
		case WHITE_PAWN:
		case BLACK_PAWN:
			i = (turn == BLACK) ? BOARD_SOUTH : BOARD_NORTH;
			if (from % WIDTH && enemyPiece(myPosition.grid[from + i + BOARD_WEST], turn)) {
				if ((!turn && from > 15) || (turn && from < 48))
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, CAPTURE);
				else {
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, QPROMOTEC);
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, NPROMOTEC);
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, BPROMOTEC);
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, RPROMOTEC);
				}
			}
			if (from % WIDTH != 7 && enemyPiece(myPosition.grid[from + i + BOARD_EAST], turn)) {
				if ((!turn && from > 15) || (turn && from < 48))
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, CAPTURE);
				else {
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, QPROMOTEC);
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, NPROMOTEC);
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, BPROMOTEC);
					nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, RPROMOTEC);
				}
			}
			if (myHistory[halfMoveClock].mHist.getFlags() == DOUBLEPUSH && ((myHistory[halfMoveClock].mHist.getTo() == from + BOARD_EAST && from % WIDTH != 7) || (myHistory[halfMoveClock].mHist.getTo() == from + BOARD_WEST && from % WIDTH)))
				nextMove[moveCount++].myMove = Move(from, myHistory[halfMoveClock].mHist.getTo() + i, ENPASSANT);
			return moveCount;
		case WHITE_KNIGHT:
		case BLACK_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && enemyPiece(myPosition.grid[from + 10], turn))
				nextMove[moveCount++].myMove = Move(from, from + 10, CAPTURE);
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && enemyPiece(myPosition.grid[from + 17], turn))
				nextMove[moveCount++].myMove = Move(from, from + 17, CAPTURE);
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && enemyPiece(myPosition.grid[from - 10], turn))
				nextMove[moveCount++].myMove = Move(from, from - 10, CAPTURE);
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && enemyPiece(myPosition.grid[from - 17], turn))
				nextMove[moveCount++].myMove = Move(from, from - 17, CAPTURE);
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && enemyPiece(myPosition.grid[from + 6], turn))
				nextMove[moveCount++].myMove = Move(from, from + 6, CAPTURE);
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && enemyPiece(myPosition.grid[from + 15], turn))
				nextMove[moveCount++].myMove = Move(from, from + 15, CAPTURE);
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && enemyPiece(myPosition.grid[from - 6], turn))
				nextMove[moveCount++].myMove = Move(from, from - 6, CAPTURE);
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && enemyPiece(myPosition.grid[from - 15], turn))
				nextMove[moveCount++].myMove = Move(from, from - 15, CAPTURE);
			return moveCount;
		case WHITE_QUEEN:
		case WHITE_ROOK:
		case BLACK_QUEEN:
		case BLACK_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
			}
			if (myPosition.grid[from] != WHITE_QUEEN && myPosition.grid[from] != BLACK_QUEEN)
				return moveCount;
		case WHITE_BISHOP:
		case BLACK_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
			}
		}
		return moveCount;
	}

	unsigned Board::genNonCapMovesAt(scoredMove* nextMove, int from)
	{//generates all pseudo legal non capture moves for one piece
		int i;
		unsigned moveCount = 0;
		switch (myPosition.grid[from])
		{
		case WHITE_KING:
		case BLACK_KING:
			if (turn == WHITE) {
				if (from == 60) {
					if (myPosition.grid[61] == EMPTY && myPosition.grid[62] == EMPTY && (myHistory[halfMoveClock].cHist & 1 << 0))
						nextMove[moveCount++].myMove = Move(60, 62, KCASTLE);
					if (myPosition.grid[59] == EMPTY && myPosition.grid[58] == EMPTY && myPosition.grid[57] == EMPTY && (myHistory[halfMoveClock].cHist & 1 << 1))
						nextMove[moveCount++].myMove = Move(60, 58, QCASTLE);
				}
			}
			else {
				if (from == 4) {
					if (myPosition.grid[5] == EMPTY && myPosition.grid[6] == EMPTY && (myHistory[halfMoveClock].cHist & 1 << 2))
						nextMove[moveCount++].myMove = Move(4, 6, KCASTLE);
					if (myPosition.grid[3] == EMPTY && myPosition.grid[2] == EMPTY && myPosition.grid[1] == EMPTY && (myHistory[halfMoveClock].cHist & 1 << 3))
						nextMove[moveCount++].myMove = Move(4, 2, QCASTLE);
				}
			}
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && myPosition.grid[from + BOARD_SOUTHEAST] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + BOARD_SOUTHEAST, STANDARD);
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH && myPosition.grid[from + BOARD_EAST] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + BOARD_EAST, STANDARD);
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8 && myPosition.grid[from + BOARD_NORTHWEST] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + BOARD_NORTHWEST, STANDARD);
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0 && myPosition.grid[from + BOARD_WEST] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + BOARD_WEST, STANDARD);
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && myPosition.grid[from + BOARD_SOUTHWEST] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + BOARD_SOUTHWEST, STANDARD);
			if (from < 56 && myPosition.grid[from + BOARD_SOUTH] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + BOARD_SOUTH, STANDARD);
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6 && myPosition.grid[from + BOARD_NORTHEAST] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + BOARD_NORTHEAST, STANDARD);
			if (from > 7 && myPosition.grid[from + BOARD_NORTH] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + BOARD_NORTH, STANDARD);
			return moveCount;
		case WHITE_PAWN:
		case BLACK_PAWN:
			i = (turn == BLACK) ? BOARD_SOUTH : BOARD_NORTH;
			if (myPosition.grid[from + i] == EMPTY)
			{
				if ((!turn && from > 15) || (turn && from < 48)) {
					nextMove[moveCount++].myMove = Move(from, from + i, STANDARD);
					if (((!turn && from > 47) || (turn && from < 16)) && myPosition.grid[from + 2 * i] == EMPTY)
						nextMove[moveCount++].myMove = Move(from, from + 2 * i, DOUBLEPUSH);
				}
				else {
					nextMove[moveCount++].myMove = Move(from, from + i, QPROMOTE);
					nextMove[moveCount++].myMove = Move(from, from + i, NPROMOTE);
					nextMove[moveCount++].myMove = Move(from, from + i, BPROMOTE);
					nextMove[moveCount++].myMove = Move(from, from + i, RPROMOTE);
				}
			}
			return moveCount;
		case WHITE_KNIGHT:
		case BLACK_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && myPosition.grid[from + 10] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + 10, STANDARD);
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && myPosition.grid[from + 17] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + 17, STANDARD);
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && myPosition.grid[from - 10] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from - 10, STANDARD);
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && myPosition.grid[from - 17] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from - 17, STANDARD);
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && myPosition.grid[from + 6] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + 6, STANDARD);
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && myPosition.grid[from + 15] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from + 15, STANDARD);
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && myPosition.grid[from - 6] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from - 6, STANDARD);
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && myPosition.grid[from - 15] == EMPTY)
				nextMove[moveCount++].myMove = Move(from, from - 15, STANDARD);
			return moveCount;
		case WHITE_QUEEN:
		case WHITE_ROOK:
		case BLACK_QUEEN:
		case BLACK_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
				if (myPosition.grid[i] != EMPTY)
					break;
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
				if (myPosition.grid[i] != EMPTY)
					break;
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
				if (myPosition.grid[i] != EMPTY)
					break;
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
				if (myPosition.grid[i] != EMPTY)
					break;
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			if (myPosition.grid[from] != WHITE_QUEEN && myPosition.grid[from] != BLACK_QUEEN)
				return moveCount;
		case WHITE_BISHOP:
		case BLACK_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
				if (myPosition.grid[i] != EMPTY)
					break;
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
				if (myPosition.grid[i] != EMPTY)
					break;
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
				if (myPosition.grid[i] != EMPTY)
					break;
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
				if (myPosition.grid[i] != EMPTY)
					break;
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
		}
		return moveCount;
	}

	unsigned Board::genAllMovesAt(scoredMove* nextMove, int from)
	{//generates all pseudo legal moves for one piece
		int i;
		unsigned moveCount = 0;
		switch (myPosition.grid[from]) {
		case WHITE_KING:
		case BLACK_KING:
			if (turn == WHITE) {
				if (from == 60) {
					if (myPosition.grid[61] == EMPTY && myPosition.grid[62] == EMPTY && (myHistory[halfMoveClock].cHist & 1 << 0))
						nextMove[moveCount++].myMove = Move(60, 62, KCASTLE);
					if (myPosition.grid[59] == EMPTY && myPosition.grid[58] == EMPTY && myPosition.grid[57] == EMPTY && (myHistory[halfMoveClock].cHist & 1 << 1))
						nextMove[moveCount++].myMove = Move(60, 58, QCASTLE);
				}
			}
			else {
				if (from == 4) {
					if (myPosition.grid[5] == EMPTY && myPosition.grid[6] == EMPTY && (myHistory[halfMoveClock].cHist & 1 << 2))
						nextMove[moveCount++].myMove = Move(4, 6, KCASTLE);
					if (myPosition.grid[3] == EMPTY && myPosition.grid[2] == EMPTY && myPosition.grid[1] == EMPTY && (myHistory[halfMoveClock].cHist & 1 << 3))
						nextMove[moveCount++].myMove = Move(4, 2, QCASTLE);
				}
			}
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55)
				if (!validPiece(myPosition.grid[from + BOARD_SOUTHEAST], turn))
					nextMove[moveCount++].myMove = myPosition.grid[from + BOARD_SOUTHEAST] == EMPTY ? 
					Move(from, from + BOARD_SOUTHEAST, STANDARD) : 
					Move(from, from + BOARD_SOUTHEAST, CAPTURE);
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH)
				if (!validPiece(myPosition.grid[from + BOARD_EAST], turn))
					nextMove[moveCount++].myMove = myPosition.grid[from + BOARD_EAST] == EMPTY ? 
					Move(from, from + BOARD_EAST, STANDARD) : 
					Move(from, from + BOARD_EAST, CAPTURE);
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8) //bad
				if (!validPiece(myPosition.grid[from + BOARD_NORTHWEST], turn))
					nextMove[moveCount++].myMove = myPosition.grid[from + BOARD_NORTHWEST] == EMPTY ?
					Move(from, from + BOARD_NORTHWEST, STANDARD) :
					Move(from, from + BOARD_NORTHWEST, CAPTURE);
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0)
				if (!validPiece(myPosition.grid[from + BOARD_WEST], turn))
					nextMove[moveCount++].myMove = myPosition.grid[from + BOARD_WEST] == EMPTY ? 
					Move(from, from + BOARD_WEST, STANDARD) : 
					Move(from, from + BOARD_WEST, CAPTURE);
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57)
				if (!validPiece(myPosition.grid[from + BOARD_SOUTHWEST], turn))
					nextMove[moveCount++].myMove = myPosition.grid[from + BOARD_SOUTHWEST] == EMPTY ? 
					Move(from, from + BOARD_SOUTHWEST, STANDARD) : 
					Move(from, from + BOARD_SOUTHWEST, CAPTURE);
			if (from < 56)
				if (!validPiece(myPosition.grid[from + BOARD_SOUTH], turn))
					nextMove[moveCount++].myMove = myPosition.grid[from + BOARD_SOUTH] == EMPTY ? 
					Move(from, from + BOARD_SOUTH, STANDARD) :
					Move(from, from + BOARD_SOUTH, CAPTURE);
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6)//bad
				if (!validPiece(myPosition.grid[from + BOARD_NORTHEAST], turn))
					nextMove[moveCount++].myMove = myPosition.grid[from + BOARD_NORTHEAST] == EMPTY ? 
					Move(from, from + BOARD_NORTHEAST, STANDARD) : 
					Move(from, from + BOARD_NORTHEAST, CAPTURE);
			if (from > 7)//bad
				if (!validPiece(myPosition.grid[from + BOARD_NORTH], turn))
					nextMove[moveCount++].myMove = myPosition.grid[from + BOARD_NORTH] == EMPTY ? 
					Move(from, from + BOARD_NORTH, STANDARD) : 
					Move(from, from + BOARD_NORTH, CAPTURE);
			return moveCount;
		case WHITE_PAWN:
		case BLACK_PAWN:
			i = (turn == BLACK) ? BOARD_SOUTH : BOARD_NORTH;
			if (from % WIDTH) {
				if (enemyPiece(myPosition.grid[from + i + BOARD_WEST], turn)) {
					if ((!turn && from > 15) || (turn && from < 48))
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, CAPTURE);
					else {
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, QPROMOTEC);
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, NPROMOTEC);
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, BPROMOTEC);
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_WEST, RPROMOTEC);
					}
				}
			}
			if (from % WIDTH != 7) {
				if (enemyPiece(myPosition.grid[from + i + BOARD_EAST], turn)) {
					if ((!turn && from > 15) || (turn && from < 48))
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, CAPTURE);
					else {
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, QPROMOTEC);
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, NPROMOTEC);
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, BPROMOTEC);
						nextMove[moveCount++].myMove = Move(from, from + i + BOARD_EAST, RPROMOTEC);
					}
				}
			}
			if (myPosition.grid[from + i] == EMPTY) {
				if ((!turn && from > 15) || (turn && from < 48)) {
					nextMove[moveCount++].myMove = Move(from, from + i, STANDARD);
					if (((!turn && from > 47) || (turn && from < 16)) && myPosition.grid[from + 2 * i] == EMPTY)
						nextMove[moveCount++].myMove = Move(from, from + 2 * i, DOUBLEPUSH);
				}
				else {
					nextMove[moveCount++].myMove = Move(from, from + i, QPROMOTE);
					nextMove[moveCount++].myMove = Move(from, from + i, NPROMOTE);
					nextMove[moveCount++].myMove = Move(from, from + i, BPROMOTE);
					nextMove[moveCount++].myMove = Move(from, from + i, RPROMOTE);
				}
			}
			if (myHistory[halfMoveClock].mHist.getFlags() == DOUBLEPUSH && 
				((myHistory[halfMoveClock].mHist.getTo() == from + BOARD_EAST && from % WIDTH != 7) 
				|| (myHistory[halfMoveClock].mHist.getTo() == from + BOARD_WEST && from % WIDTH)))
				nextMove[moveCount++].myMove = Move(from, myHistory[halfMoveClock].mHist.getTo() + i, ENPASSANT);
			return moveCount;
		case WHITE_KNIGHT:
		case BLACK_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54) {
				if (myPosition.grid[from + 10] == EMPTY)
					nextMove[moveCount++].myMove = Move(from, from + 10, STANDARD);
				else if (enemyPiece(myPosition.grid[from + 10], turn))
					nextMove[moveCount++].myMove = Move(from, from + 10, CAPTURE);
			}
			if ((from + 17) % WIDTH > from % WIDTH && from < 47) {
				if (myPosition.grid[from + 17] == EMPTY)
					nextMove[moveCount++].myMove = Move(from, from + 17, STANDARD);
				else if (enemyPiece(myPosition.grid[from + 17], turn))
					nextMove[moveCount++].myMove = Move(from, from + 17, CAPTURE);
			}
			if ((from - 10) % WIDTH < from % WIDTH && from > 9) {
				if (myPosition.grid[from - 10] == EMPTY)
					nextMove[moveCount++].myMove = Move(from, from - 10, STANDARD);
				else if (enemyPiece(myPosition.grid[from - 10], turn))
					nextMove[moveCount++].myMove = Move(from, from - 10, CAPTURE);
			}
			if ((from - 17) % WIDTH < from % WIDTH && from > 16) {
				if (myPosition.grid[from - 17] == EMPTY)
					nextMove[moveCount++].myMove = Move(from, from - 17, STANDARD);
				else if (enemyPiece(myPosition.grid[from - 17], turn))
					nextMove[moveCount++].myMove = Move(from, from - 17, CAPTURE);
			}
			if ((from + 6) % WIDTH < from % WIDTH && from < 58) {
				if (myPosition.grid[from + 6] == EMPTY)
					nextMove[moveCount++].myMove = Move(from, from + 6, STANDARD);
				else if (enemyPiece(myPosition.grid[from + 6], turn))
					nextMove[moveCount++].myMove = Move(from, from + 6, CAPTURE);
			}
			if ((from + 15) % WIDTH < from % WIDTH && from < 49) {
				if (myPosition.grid[from + 15] == EMPTY)
					nextMove[moveCount++].myMove = Move(from, from + 15, STANDARD);
				else if (enemyPiece(myPosition.grid[from + 15], turn))
					nextMove[moveCount++].myMove = Move(from, from + 15, CAPTURE);
			}
			if ((from - 6) % WIDTH > from % WIDTH && from > 5) {
				if (myPosition.grid[from - 6] == EMPTY)
					nextMove[moveCount++].myMove = Move(from, from - 6, STANDARD);
				else if (enemyPiece(myPosition.grid[from - 6], turn))
					nextMove[moveCount++].myMove = Move(from, from - 6, CAPTURE);
			}
			if ((from - 15) % WIDTH > from % WIDTH && from > 14) {
				if (myPosition.grid[from - 15] == EMPTY)
					nextMove[moveCount++].myMove = Move(from, from - 15, STANDARD);
				else if (enemyPiece(myPosition.grid[from - 15], turn))
					nextMove[moveCount++].myMove = Move(from, from - 15, CAPTURE);
			}
			return moveCount;
		case WHITE_QUEEN:
		case WHITE_ROOK:
		case BLACK_QUEEN:
		case BLACK_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			if (myPosition.grid[from] != WHITE_QUEEN && myPosition.grid[from] != BLACK_QUEEN)
				return moveCount;
		case WHITE_BISHOP:
		case BLACK_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
				if (myPosition.grid[i] != EMPTY) {
					if (enemyPiece(myPosition.grid[i], turn))
						nextMove[moveCount++].myMove = Move(from, i, CAPTURE);
					break;
				}
				else
					nextMove[moveCount++].myMove = Move(from, i, STANDARD);
			}
		}
		return moveCount;
	}
}