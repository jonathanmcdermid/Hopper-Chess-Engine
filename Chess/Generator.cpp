#include "Board.h"
#include <cmath>

namespace Hopper 
{
	bool Board::validateMove(Move nextMove) 
	{//validates a single move
		Move temp[28];
		int moveCount;
		if ((turn && grid[nextMove.getFrom()] > 0) || (!turn && grid[nextMove.getFrom()] < 0)) 
			moveCount = genAllMovesAt(temp, nextMove.getFrom());
		else 
			return false;
		moveCount = removeIllegalMoves(temp, moveCount);
		for (int i = 0; i < moveCount; ++i)
		{
			if (temp[i] == nextMove)
				return true;
		}
		return false;
	}

	Move Board::createMove(int from, int to) 
	{
		Move temp[28];
		int moveCount = genAllMovesAt(temp, from);
		moveCount = removeIllegalMoves(temp, moveCount);
		for (int i = 0; i < moveCount; ++i)
		{
			if (temp[i].getFrom() == from && temp[i].getTo() == to)
				return temp[i];
		}
		return NULLMOVE;
	}

	int Board::removeIllegalMoves(Move* nextMove, int moveCount) 
	{
		int checktype, to, from;
		for (int i = 0; i < pinCount; ++i)
		{
			for (int j = 0; j < moveCount; ++j)
			{
				if (nextMove[j].getFrom() == pinnedPieces[i])
				{
					switch (pinnedPieces[i + KINDEX])
					{
					case BOARD_SOUTH:
					case BOARD_NORTH:
						if (!(NSslide(nextMove[j].getFrom(), nextMove[j].getTo())))
							goto remove;
						else
							continue;
					case BOARD_WEST:
					case BOARD_EAST:
						if (!(EWslide(nextMove[j].getFrom(), nextMove[j].getTo())))
							goto remove;
						else
							continue;
					case BOARD_NORTHEAST:
					case BOARD_SOUTHWEST:
						if (!(NESWslide(nextMove[j].getFrom(), nextMove[j].getTo())))
							goto remove;
						else
							continue;
					case BOARD_NORTHWEST:
					case BOARD_SOUTHEAST:
						if (!(NWSEslide(nextMove[j].getFrom(), nextMove[j].getTo())))
							goto remove;
						else
							continue;
					}
				remove:
					nextMove[j--] = nextMove[--moveCount];
				}
			}
		}
		for (int i = 0; i < threatened[!turn * SPACES + kingPos[turn]]; ++i)
		{
			if (abs(grid[attackers[!turn][i][kingPos[turn]]]) <= W_KNIGHT)
				checktype = BOARD_LEAP;
			else if (NSslide(attackers[!turn][i][kingPos[turn]], kingPos[turn]))
				checktype = (attackers[!turn][i][kingPos[turn]] > kingPos[turn]) ? BOARD_NORTH : BOARD_SOUTH;
			else if (EWslide(attackers[!turn][i][kingPos[turn]], kingPos[turn]))
				checktype = (attackers[!turn][i][kingPos[turn]] > kingPos[turn]) ? BOARD_WEST : BOARD_EAST;
			else if (NESWslide(attackers[!turn][i][kingPos[turn]], kingPos[turn]))
				checktype = (attackers[!turn][i][kingPos[turn]] > kingPos[turn]) ? BOARD_NORTHEAST : BOARD_SOUTHWEST;
			else
				checktype = (attackers[!turn][i][kingPos[turn]] > kingPos[turn]) ? BOARD_NORTHWEST : BOARD_SOUTHEAST;
			for (int j = 0; j < moveCount; ++j)
			{
				to = nextMove[j].getTo();
				from = nextMove[j].getFrom();
				if (from != kingPos[turn])
				{
					if (checktype != BOARD_LEAP && ((to - attackers[!turn][i][kingPos[turn]]) % checktype || (to < attackers[!turn][i][kingPos[turn]] && to < kingPos[turn]) || (to > attackers[!turn][i][kingPos[turn]] && to > kingPos[turn])))
						nextMove[j--] = nextMove[--moveCount];
					else if (checktype == BOARD_LEAP && to != attackers[!turn][i][kingPos[turn]] && nextMove[j].getFlags() != ENPASSANT)
						nextMove[j--] = nextMove[--moveCount];
				}
				else if (checktype != BOARD_LEAP && to - from == checktype)
					nextMove[j--] = nextMove[--moveCount];
			}
		}
		return moveCount;
	}

	int Board::genAllMoves(Move* nextMove) 
	{//generates all legal moves
		int moveCount = 0;
		if (turn) 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] > 0) 
					moveCount += genAllMovesAt(&nextMove[moveCount], from);
			}
		}
		else 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] < 0) 
					moveCount += genAllMovesAt(&nextMove[moveCount], from);
			}
		}
		moveCount = removeIllegalMoves(nextMove, moveCount);
		return moveCount;
	}

	int Board::genAllCapMoves(Move* nextMove) 
	{
		int moveCount = 0;
		if (turn) 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] > 0) 
					moveCount += genCapMovesAt(&nextMove[moveCount], from);
			}
		}
		else 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] < 0) 
					moveCount += genCapMovesAt(&nextMove[moveCount], from);
			}
		}
		moveCount = removeIllegalMoves(nextMove, moveCount);
		return moveCount;
	}

	int Board::genAllNonCapMoves(Move* nextMove) 
	{
		int moveCount = 0;
		if (turn) 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] > 0) 
					moveCount += genNonCapMovesAt(&nextMove[moveCount], from);
			}
		}
		else 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] < 0) 
					moveCount += genNonCapMovesAt(&nextMove[moveCount], from);
			}
		}
		moveCount = removeIllegalMoves(nextMove, moveCount);
		return moveCount;
	}

	int Board::genCapMovesAt(Move* nextMove, int from)
	{//generates all pseudo legal capture moves for one piece
		int i, moveCount = 0;
		switch (abs(grid[from]))
		{
		case W_KING:
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && !threatened[!turn * SPACES + from + BOARD_SOUTHEAST] && ((turn && grid[from + BOARD_SOUTHEAST] < 0) || (!turn && grid[from + BOARD_SOUTHEAST] > 0)))
				nextMove[moveCount++] = Move(from, from + BOARD_SOUTHEAST, CAPTURE);
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH && !threatened[!turn * SPACES + from + BOARD_EAST] && ((turn && grid[from + BOARD_EAST] < 0) || (!turn && grid[from + BOARD_EAST] > 0)))
				nextMove[moveCount++] = Move(from, from + BOARD_EAST, CAPTURE);
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8 && !threatened[!turn * SPACES + from + BOARD_NORTHWEST] && ((turn && grid[from + BOARD_NORTHWEST] < 0) || (!turn && grid[from + BOARD_NORTHWEST] > 0)))
				nextMove[moveCount++] = Move(from, from + BOARD_NORTHWEST, CAPTURE);
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0 && !threatened[!turn * SPACES + from + BOARD_WEST] && ((turn && grid[from + BOARD_WEST] < 0) || (!turn && grid[from + BOARD_WEST] > 0)))
				nextMove[moveCount++] = Move(from, from + BOARD_WEST, CAPTURE);
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && !threatened[!turn * SPACES + from + BOARD_SOUTHWEST] && ((turn && grid[from + BOARD_SOUTHWEST] < 0) || (!turn && grid[from + BOARD_SOUTHWEST] > 0)))
				nextMove[moveCount++] = Move(from, from + BOARD_SOUTHWEST, CAPTURE);
			if (from < 56 && !threatened[!turn * SPACES + from + BOARD_SOUTH] && ((turn && grid[from + BOARD_SOUTH] < 0) || (!turn && grid[from + BOARD_SOUTH] > 0))) 
				nextMove[moveCount++] = Move(from, from + BOARD_SOUTH, CAPTURE);
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6 && !threatened[!turn * SPACES + from + BOARD_NORTHEAST] && ((turn && grid[from + BOARD_NORTHEAST] < 0) || (!turn && grid[from + BOARD_NORTHEAST] > 0)))
				nextMove[moveCount++] = Move(from, from + BOARD_NORTHEAST, CAPTURE);
			if (from > 7 && !threatened[!turn * SPACES + from + BOARD_NORTH] && ((turn && grid[from + BOARD_NORTH] < 0) || (!turn && grid[from + BOARD_NORTH] > 0)))
				nextMove[moveCount++] = Move(from, from + BOARD_NORTH, CAPTURE);
			return moveCount;
		case W_PAWN:
			i = (turn) ? BOARD_NORTH : BOARD_SOUTH;
			if (from % WIDTH && ((turn && grid[from + i + BOARD_WEST] < 0) || (!turn && grid[from + i + BOARD_WEST] > 0)))
			{
				if ((turn && from > 15) || (!turn && from < 48))
					nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, CAPTURE);
				else {
					nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, QPROMOTEC);
					nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, NPROMOTEC);
					nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, BPROMOTEC);
					nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, RPROMOTEC);
				}
			}
			if (from % WIDTH != 7 && ((turn && grid[from + i + BOARD_EAST] < 0) || (!turn && grid[from + i + BOARD_EAST] > 0)))
			{
				if ((turn && from > 15) || (!turn && from < 48))
					nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, CAPTURE);
				else {
					nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, QPROMOTEC);
					nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, NPROMOTEC);
					nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, BPROMOTEC);
					nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, RPROMOTEC);
				}
			}
			if (myHistory[halfMoveClock].mHist.getFlags() == DOUBLEPUSH && ((myHistory[halfMoveClock].mHist.getTo() == from + BOARD_EAST && from % WIDTH != 7) || (myHistory[halfMoveClock].mHist.getTo() == from + BOARD_WEST && from % WIDTH)))
				nextMove[moveCount++] = Move(from, myHistory[halfMoveClock].mHist.getTo() + i, ENPASSANT);
			return moveCount;
		case W_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && ((turn && grid[from + 10] < 0) || (!turn && grid[from + 10] > 0)))
				nextMove[moveCount++] = Move(from, from + 10, CAPTURE);
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && ((turn && grid[from + 17] < 0) || (!turn && grid[from + 17] > 0)))
				nextMove[moveCount++] = Move(from, from + 17, CAPTURE);
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && ((turn && grid[from - 10] < 0) || (!turn && grid[from - 10] > 0)))
				nextMove[moveCount++] = Move(from, from - 10, CAPTURE);
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && ((turn && grid[from - 17] < 0) || (!turn && grid[from - 17] > 0)))
				nextMove[moveCount++] = Move(from, from - 17, CAPTURE);
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && ((turn && grid[from + 6] < 0) || (!turn && grid[from + 6] > 0)))
				nextMove[moveCount++] = Move(from, from + 6, CAPTURE);
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && (turn && (grid[from + 15] < 0) || (!turn && grid[from + 15] > 0)))
				nextMove[moveCount++] = Move(from, from + 15, CAPTURE);
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && ((turn && grid[from - 6] < 0) || (!turn && grid[from - 6] > 0)))
				nextMove[moveCount++] = Move(from, from - 6, CAPTURE);
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && ((turn && grid[from - 15] < 0) || (!turn && grid[from - 15] > 0)))
				nextMove[moveCount++] = Move(from, from - 15, CAPTURE);
			return moveCount;
		case W_QUEEN:
		case W_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			if (abs(grid[from]) != W_QUEEN)
				return moveCount;
		case W_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
		}
		return moveCount;
	}

	int Board::genNonCapMovesAt(Move* nextMove, int from)
	{//generates all pseudo legal non capture moves for one piece
		int i, moveCount = 0;
		switch (abs(grid[from]))
		{
		case W_KING:
			if (turn)
			{
				if (from == 60 && !threatened[60])
				{
					if (!grid[61] && !grid[62] && !threatened[61] && !threatened[62] && (myHistory[halfMoveClock].cHist & 1 << 0))
						nextMove[moveCount++] = Move(60, 62, KCASTLE);
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[59] && !threatened[58] && (myHistory[halfMoveClock].cHist & 1 << 1))
						nextMove[moveCount++] = Move(60, 58, QCASTLE);
				}
			}
			else
			{
				if (from == 4 && !threatened[SPACES + 4])
				{
					if (!grid[5] && !grid[6] && !threatened[SPACES + 5] && !threatened[SPACES + 6] && (myHistory[halfMoveClock].cHist & 1 << 2))
						nextMove[moveCount++] = Move(4, 6, KCASTLE);
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[SPACES + 3] && !threatened[SPACES + 2] && (myHistory[halfMoveClock].cHist & 1 << 3))
						nextMove[moveCount++] = Move(4, 2, QCASTLE);
				}
			}
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && !threatened[!turn * SPACES + from + BOARD_SOUTHEAST] && !grid[from + BOARD_SOUTHEAST])
				nextMove[moveCount++] = Move(from, from + BOARD_SOUTHEAST, STANDARD);
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH && !threatened[!turn * SPACES + from + BOARD_EAST] && !grid[from + BOARD_EAST])
				nextMove[moveCount++] = Move(from, from + BOARD_EAST, STANDARD);
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8 && !threatened[!turn * SPACES + from + BOARD_NORTHWEST] && !grid[from + BOARD_NORTHWEST])
				nextMove[moveCount++] = Move(from, from + BOARD_NORTHWEST, STANDARD);
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0 && !threatened[!turn * SPACES + from + BOARD_WEST] && !grid[from + BOARD_WEST])
				nextMove[moveCount++] = Move(from, from + BOARD_WEST, STANDARD);
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && !threatened[!turn * SPACES + from + BOARD_SOUTHWEST] && !grid[from + BOARD_SOUTHWEST])
				nextMove[moveCount++] = Move(from, from + BOARD_SOUTHWEST, STANDARD);
			if (from < 56 && !threatened[!turn * SPACES + from + BOARD_SOUTH] && !grid[from + BOARD_SOUTH])
				nextMove[moveCount++] = Move(from, from + BOARD_SOUTH, STANDARD);
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6 && !threatened[!turn * SPACES + from + BOARD_NORTHEAST] && !grid[from + BOARD_NORTHEAST])
				nextMove[moveCount++] = Move(from, from + BOARD_NORTHEAST, STANDARD);
			if (from > 7 && !threatened[!turn * SPACES + from + BOARD_NORTH] && !grid[from + BOARD_NORTH])
				nextMove[moveCount++] = Move(from, from + BOARD_NORTH, STANDARD);
			return moveCount;
		case W_PAWN:
			i = (turn) ? BOARD_NORTH : BOARD_SOUTH;
			if (!grid[from + i])
			{
				if ((turn && from > 15) || (!turn && from < 48))
				{
					nextMove[moveCount++] = Move(from, from + i, STANDARD);
					if (((turn && from > 47) || (!turn && from < 16)) && !grid[from + 2 * i])
						nextMove[moveCount++] = Move(from, from + 2 * i, DOUBLEPUSH);
				}
				else
				{
					nextMove[moveCount++] = Move(from, from + i, QPROMOTE);
					nextMove[moveCount++] = Move(from, from + i, NPROMOTE);
					nextMove[moveCount++] = Move(from, from + i, BPROMOTE);
					nextMove[moveCount++] = Move(from, from + i, RPROMOTE);
				}
			}
			return moveCount;
		case W_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && !grid[from + 10])
				nextMove[moveCount++] = Move(from, from + 10, STANDARD);
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && !grid[from + 17])
				nextMove[moveCount++] = Move(from, from + 17, STANDARD);
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && !grid[from - 10])
				nextMove[moveCount++] = Move(from, from - 10, STANDARD);
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && !grid[from - 17])
				nextMove[moveCount++] = Move(from, from - 17, STANDARD);
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && !grid[from + 6])
				nextMove[moveCount++] = Move(from, from + 6, STANDARD);
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && !grid[from + 15])
				nextMove[moveCount++] = Move(from, from + 15, STANDARD);
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && !grid[from - 6])
				nextMove[moveCount++] = Move(from, from - 6, STANDARD);
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && !grid[from - 15])
				nextMove[moveCount++] = Move(from, from - 15, STANDARD);
			return moveCount;
		case W_QUEEN:
		case W_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else
					break;
			}
			if (abs(grid[from]) != W_QUEEN)
				return moveCount;
		case W_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else
					break;
			}
		}
		return moveCount;
	}

	int Board::genAllMovesAt(Move* nextMove, int from)
	{//generates all pseudo legal moves for one piece
		int i, moveCount = 0;
		switch (abs(grid[from]))
		{
		case W_KING:
			if (turn)
			{
				if (from == 60 && !threatened[60])
				{
					if (!grid[61] && !grid[62] && !threatened[61] && !threatened[62] && (myHistory[halfMoveClock].cHist & 1 << 0))
						nextMove[moveCount++] = Move(60, 62, KCASTLE);
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[59] && !threatened[58] && (myHistory[halfMoveClock].cHist & 1 << 1))
						nextMove[moveCount++] = Move(60, 58, QCASTLE);
				}
			}
			else
			{
				if (from == 4 && !threatened[SPACES + 4])
				{
					if (!grid[5] && !grid[6] && !threatened[SPACES + 5] && !threatened[SPACES + 6] && (myHistory[halfMoveClock].cHist & 1 << 2))
						nextMove[moveCount++] = Move(4, 6, KCASTLE);
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[SPACES + 3] && !threatened[SPACES + 2] && (myHistory[halfMoveClock].cHist & 1 << 3))
						nextMove[moveCount++] = Move(4, 2, QCASTLE);
				}
			}
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55)
			{
				if (!threatened[!turn * SPACES + from + BOARD_SOUTHEAST] && ((turn && grid[from + BOARD_SOUTHEAST] <= 0) || (!turn && grid[from + BOARD_SOUTHEAST] >= 0)))
				{
					if (!grid[from + BOARD_SOUTHEAST])
						nextMove[moveCount++] = Move(from, from + BOARD_SOUTHEAST, STANDARD);
					else
						nextMove[moveCount++] = Move(from, from + BOARD_SOUTHEAST, CAPTURE);
				}
			}
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH)
			{
				if (!threatened[!turn * SPACES + from + BOARD_EAST] && ((turn && grid[from + BOARD_EAST] <= 0) || (!turn && grid[from + BOARD_EAST] >= 0)))
				{
					if (!grid[from + BOARD_EAST])
						nextMove[moveCount++] = Move(from, from + BOARD_EAST, STANDARD);
					else
						nextMove[moveCount++] = Move(from, from + BOARD_EAST, CAPTURE);
				}
			}
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8)
			{
				if (!threatened[!turn * SPACES + from + BOARD_NORTHWEST] && ((turn && grid[from + BOARD_NORTHWEST] <= 0) || (!turn && grid[from + BOARD_NORTHWEST] >= 0)))
				{
					if (!grid[from + BOARD_NORTHWEST])
						nextMove[moveCount++] = Move(from, from + BOARD_NORTHWEST, STANDARD);
					else
						nextMove[moveCount++] = Move(from, from + BOARD_NORTHWEST, CAPTURE);
				}
			}
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0)
			{
				if (!threatened[!turn * SPACES + from + BOARD_WEST] && ((turn && grid[from + BOARD_WEST] <= 0) || (!turn && grid[from + BOARD_WEST] >= 0)))
				{
					if (!grid[from + BOARD_WEST])
						nextMove[moveCount++] = Move(from, from + BOARD_WEST, STANDARD);
					else
						nextMove[moveCount++] = Move(from, from + BOARD_WEST, CAPTURE);
				}
			}
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57)
			{
				if (!threatened[!turn * SPACES + from + BOARD_SOUTHWEST] && ((turn && grid[from + BOARD_SOUTHWEST] <= 0) || (!turn && grid[from + BOARD_SOUTHWEST] >= 0)))
				{
					if (!grid[from + BOARD_SOUTHWEST])
						nextMove[moveCount++] = Move(from, from + BOARD_SOUTHWEST, STANDARD);
					else
						nextMove[moveCount++] = Move(from, from + BOARD_SOUTHWEST, CAPTURE);
				}
			}
			if (from < 56)
			{
				if (!threatened[!turn * SPACES + from + BOARD_SOUTH] && ((turn && grid[from + BOARD_SOUTH] <= 0) || (!turn && grid[from + BOARD_SOUTH] >= 0)))
				{
					if (!grid[from + BOARD_SOUTH])
						nextMove[moveCount++] = Move(from, from + BOARD_SOUTH, STANDARD);
					else
						nextMove[moveCount++] = Move(from, from + BOARD_SOUTH, CAPTURE);
				}
			}
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6)
			{
				if (!threatened[!turn * SPACES + from + BOARD_NORTHEAST] && ((turn && grid[from + BOARD_NORTHEAST] <= 0) || (!turn && grid[from + BOARD_NORTHEAST] >= 0)))
				{
					if (!grid[from + BOARD_NORTHEAST])
						nextMove[moveCount++] = Move(from, from + BOARD_NORTHEAST, STANDARD);
					else
						nextMove[moveCount++] = Move(from, from + BOARD_NORTHEAST, CAPTURE);
				}
			}
			if (from > 7) {
				if (!threatened[!turn * SPACES + from + BOARD_NORTH] && ((turn && grid[from + BOARD_NORTH] <= 0) || (!turn && grid[from + BOARD_NORTH] >= 0)))
				{
					if (!grid[from + BOARD_NORTH])
						nextMove[moveCount++] = Move(from, from + BOARD_NORTH, STANDARD);
					else
						nextMove[moveCount++] = Move(from, from + BOARD_NORTH, CAPTURE);
				}
			}
			return moveCount;
		case W_PAWN:
			i = (turn) ? BOARD_NORTH : BOARD_SOUTH;
			if (from % WIDTH)
			{
				if ((turn && grid[from + i + BOARD_WEST] < 0) || (!turn && grid[from + i + BOARD_WEST] > 0))
				{
					if ((turn && from > 15) || (!turn && from < 48))
						nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, CAPTURE);
					else
					{
						nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, QPROMOTEC);
						nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, NPROMOTEC);
						nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, BPROMOTEC);
						nextMove[moveCount++] = Move(from, from + i + BOARD_WEST, RPROMOTEC);
					}
				}
			}
			if (from % WIDTH != 7)
			{
				if ((turn && grid[from + i + BOARD_EAST] < 0) || (!turn && grid[from + i + BOARD_EAST] > 0))
				{
					if ((turn && from > 15) || (!turn && from < 48))
						nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, CAPTURE);
					else
					{
						nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, QPROMOTEC);
						nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, NPROMOTEC);
						nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, BPROMOTEC);
						nextMove[moveCount++] = Move(from, from + i + BOARD_EAST, RPROMOTEC);
					}
				}
			}
			if (!grid[from + i]) {
				if ((turn && from > 15) || (!turn && from < 48))
				{
					nextMove[moveCount++] = Move(from, from + i, STANDARD);
					if (((turn && from > 47) || (!turn && from < 16)) && !grid[from + 2 * i])
						nextMove[moveCount++] = Move(from, from + 2 * i, DOUBLEPUSH);
				}
				else
				{
					nextMove[moveCount++] = Move(from, from + i, QPROMOTE);
					nextMove[moveCount++] = Move(from, from + i, NPROMOTE);
					nextMove[moveCount++] = Move(from, from + i, BPROMOTE);
					nextMove[moveCount++] = Move(from, from + i, RPROMOTE);
				}
			}
			if (myHistory[halfMoveClock].mHist.getFlags() == DOUBLEPUSH && ((myHistory[halfMoveClock].mHist.getTo() == from + BOARD_EAST && from % WIDTH != 7) || (myHistory[halfMoveClock].mHist.getTo() == from + BOARD_WEST && from % WIDTH)))
				nextMove[moveCount++] = Move(from, myHistory[halfMoveClock].mHist.getTo() + i, ENPASSANT);
			return moveCount;
		case W_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54)
			{
				if (!grid[from + 10])
					nextMove[moveCount++] = Move(from, from + 10, STANDARD);
				else if ((turn && grid[from + 10] < 0) || (!turn && grid[from + 10] > 0))
					nextMove[moveCount++] = Move(from, from + 10, CAPTURE);
			}
			if ((from + 17) % WIDTH > from % WIDTH && from < 47)
			{
				if (!grid[from + 17])
					nextMove[moveCount++] = Move(from, from + 17, STANDARD);
				else if ((turn && grid[from + 17] < 0) || (!turn && grid[from + 17] > 0))
					nextMove[moveCount++] = Move(from, from + 17, CAPTURE);
			}
			if ((from - 10) % WIDTH < from % WIDTH && from > 9)
			{
				if (!grid[from - 10])
					nextMove[moveCount++] = Move(from, from - 10, STANDARD);
				else if ((turn && grid[from - 10] < 0) || (!turn && grid[from - 10] > 0))
					nextMove[moveCount++] = Move(from, from - 10, CAPTURE);
			}
			if ((from - 17) % WIDTH < from % WIDTH && from > 16)
			{
				if (!grid[from - 17])
					nextMove[moveCount++] = Move(from, from - 17, STANDARD);
				else if ((turn && grid[from - 17] < 0) || (!turn && grid[from - 17] > 0))
					nextMove[moveCount++] = Move(from, from - 17, CAPTURE);
			}
			if ((from + 6) % WIDTH < from % WIDTH && from < 58)
			{
				if (!grid[from + 6])
					nextMove[moveCount++] = Move(from, from + 6, STANDARD);
				else if ((turn && grid[from + 6] < 0) || (!turn && grid[from + 6] > 0))
					nextMove[moveCount++] = Move(from, from + 6, CAPTURE);
			}
			if ((from + 15) % WIDTH < from % WIDTH && from < 49)
			{
				if (!grid[from + 15])
					nextMove[moveCount++] = Move(from, from + 15, STANDARD);
				else if (turn && (grid[from + 15] < 0) || (!turn && grid[from + 15] > 0))
					nextMove[moveCount++] = Move(from, from + 15, CAPTURE);
			}
			if ((from - 6) % WIDTH > from % WIDTH && from > 5)
			{
				if (!grid[from - 6])
					nextMove[moveCount++] = Move(from, from - 6, STANDARD);
				else if ((turn && grid[from - 6] < 0) || (!turn && grid[from - 6] > 0))
					nextMove[moveCount++] = Move(from, from - 6, CAPTURE);
			}
			if ((from - 15) % WIDTH > from % WIDTH && from > 14)
			{
				if (!grid[from - 15])
					nextMove[moveCount++] = Move(from, from - 15, STANDARD);
				else if ((turn && grid[from - 15] < 0) || (!turn && grid[from - 15] > 0))
					nextMove[moveCount++] = Move(from, from - 15, CAPTURE);
			}
			return moveCount;
		case W_QUEEN:
		case W_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			if (abs(grid[from]) != W_QUEEN)
				return moveCount;
		case W_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST)
			{
				if (!grid[i])
					nextMove[moveCount++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						nextMove[moveCount++] = Move(from, i, CAPTURE);
					break;
				}
			}
		}
		return moveCount;
	}
}