#include "Board.h"
#include <cmath>

namespace Hopper 
{
	bool Board::validateMove(const Move& m) 
	{//validates a single move
		Move temp[28];
		int cmove;
		if ((turn && grid[m.getFrom()] > 0) || (!turn && grid[m.getFrom()] < 0)) 
			cmove = pieceMoves(temp, m.getFrom());
		else 
			return false;
		cmove = removeIllegal(temp, cmove);
		for (int i = 0; i < cmove; ++i)
		{
			if (temp[i] == m)
				return true;
		}
		return false;
	}

	Move Board::createMove(int from, int to) 
	{
		Move temp[28];
		int cmove = pieceMoves(temp, from);
		cmove = removeIllegal(temp, cmove);
		for (int i = 0; i < cmove; ++i)
		{
			if (temp[i].getFrom() == from && temp[i].getTo() == to)
				return temp[i];
		}
		return NULLMOVE;
	}

	int Board::removeIllegal(Move* m, int cmove) 
	{
		int checktype, to, from;
		for (int i = 0; i < cpins; ++i) 
		{
			for (int j = 0; j < cmove; ++j) 
			{
				if (m[j].getFrom() == pins[i]) 
				{
					switch (pins[5 + i])
					{
					case BOARD_SOUTH:
					case BOARD_NORTH:
						if (!(NSslide(m[j].getFrom(), m[j].getTo()))) 
							goto remove;
						else 
							continue;
					case BOARD_WEST:
					case BOARD_EAST:
						if (!(EWslide(m[j].getFrom(), m[j].getTo()))) 
							goto remove;
						else 
							continue;
					case BOARD_NORTHEAST:
					case BOARD_SOUTHWEST:
						if (!(NESWslide(m[j].getFrom(), m[j].getTo()))) 
							goto remove;
						else 
							continue;
					case BOARD_NORTHWEST:
					case BOARD_SOUTHEAST:
						if (!(NWSEslide(m[j].getFrom(), m[j].getTo()))) 
							goto remove;
						else 
							continue;
					}
				remove:
					m[j--] = m[--cmove];
				}
			}
		}
		for (int i = 0; i < threatened[(int) (!turn << 6) + kpos[turn]]; ++i) 
		{
			if (abs(grid[attackers[(int) (!turn << 3) + i][kpos[turn]]]) <= W_KNIGHT) 
				checktype = BOARD_LEAP;
			else if (NSslide(attackers[(int) (!turn << 3) + i][kpos[turn]], kpos[turn])) 
				checktype = (attackers[(int) (!turn << 3) + i][kpos[turn]] > kpos[turn]) ? BOARD_NORTH : BOARD_SOUTH;
			else if (EWslide(attackers[(int) (!turn << 3) + i][kpos[turn]], kpos[turn])) 
				checktype = (attackers[(int) (!turn << 3) + i][kpos[turn]] > kpos[turn]) ? BOARD_WEST : BOARD_EAST;
			else if (NESWslide(attackers[(int) (!turn << 3) + i][kpos[turn]], kpos[turn])) 
				checktype = (attackers[(int) (!turn << 3) + i][kpos[turn]] > kpos[turn]) ? BOARD_NORTHEAST : BOARD_SOUTHWEST;
			else 
				checktype = (attackers[(int) (!turn << 3) + i][kpos[turn]] > kpos[turn]) ? BOARD_NORTHWEST : BOARD_SOUTHEAST;
			for (int j = 0; j < cmove; ++j) 
			{
				to = m[j].getTo();
				from = m[j].getFrom();
				if (from != kpos[turn]) 
				{
					if (checktype != BOARD_LEAP && ((to - attackers[(int) (!turn << 3) + i][kpos[turn]]) % checktype || (to < attackers[(int) (!turn << 3) + i][kpos[turn]] && to < kpos[turn]) || (to > attackers[(int) (!turn << 3) + i][kpos[turn]] && to > kpos[turn]))) 
						m[j--] = m[--cmove];
					else if (checktype == BOARD_LEAP && to != attackers[(int) (!turn << 3) + i][kpos[turn]] && m[j].getFlags() != ENPASSANT) 
						m[j--] = m[--cmove];
				}
				else if (checktype != BOARD_LEAP && to - from == checktype) 
						m[j--] = m[--cmove];
			}
		}
		return cmove;
	}

	int Board::genAll(Move* m) 
	{//generates all legal moves
		int cmove = 0;
		if (turn) 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] > 0) 
					cmove += pieceMoves(&m[cmove], from);
			}
		}
		else 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] < 0) 
					cmove += pieceMoves(&m[cmove], from);
			}
		}
		cmove = removeIllegal(m, cmove);
		return cmove;
	}

	int Board::genAllCaps(Move* m) 
	{
		int cmove = 0;
		if (turn) 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] > 0) 
					cmove += pieceCapMoves(&m[cmove], from);
			}
		}
		else 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] < 0) 
					cmove += pieceCapMoves(&m[cmove], from);
			}
		}
		cmove = removeIllegal(m, cmove);
		return cmove;
	}

	int Board::genAllNonCaps(Move* m) 
	{
		int cmove = 0;
		if (turn) 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] > 0) 
					cmove += pieceNonCapMoves(&m[cmove], from);
			}
		}
		else 
		{
			for (int from = 0; from < SPACES; ++from) 
			{
				if (grid[from] < 0) 
					cmove += pieceNonCapMoves(&m[cmove], from);
			}
		}
		cmove = removeIllegal(m, cmove);
		return cmove;
	}

	int Board::pieceCapMoves(Move* m, int from)
	{//generates all pseudo legal capture moves for one piece
		int i, cmove = 0;
		switch (abs(grid[from]))
		{
		case W_KING:
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && !threatened[(int) (!turn << 6) + from + BOARD_SOUTHEAST] && ((turn && grid[from + BOARD_SOUTHEAST] < 0) || (!turn && grid[from + BOARD_SOUTHEAST] > 0)))
				m[cmove++] = Move(from, from + BOARD_SOUTHEAST, CAPTURE);
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH && !threatened[(int) (!turn << 6) + from + BOARD_EAST] && ((turn && grid[from + BOARD_EAST] < 0) || (!turn && grid[from + BOARD_EAST] > 0)))
				m[cmove++] = Move(from, from + BOARD_EAST, CAPTURE);
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8 && !threatened[(int) (!turn << 6) + from + BOARD_NORTHWEST] && ((turn && grid[from + BOARD_NORTHWEST] < 0) || (!turn && grid[from + BOARD_NORTHWEST] > 0)))
				m[cmove++] = Move(from, from + BOARD_NORTHWEST, CAPTURE);
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0 && !threatened[(int) (!turn << 6) + from + BOARD_WEST] && ((turn && grid[from + BOARD_WEST] < 0) || (!turn && grid[from + BOARD_WEST] > 0)))
				m[cmove++] = Move(from, from + BOARD_WEST, CAPTURE);
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && !threatened[(int) (!turn << 6) + from + BOARD_SOUTHWEST] && ((turn && grid[from + BOARD_SOUTHWEST] < 0) || (!turn && grid[from + BOARD_SOUTHWEST] > 0)))
				m[cmove++] = Move(from, from + BOARD_SOUTHWEST, CAPTURE);
			if (from < 56 && !threatened[(int) (!turn << 6) + from + BOARD_SOUTH] && ((turn && grid[from + BOARD_SOUTH] < 0) || (!turn && grid[from + BOARD_SOUTH] > 0))) 
				m[cmove++] = Move(from, from + BOARD_SOUTH, CAPTURE);
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6 && !threatened[(int) (!turn << 6) + from + BOARD_NORTHEAST] && ((turn && grid[from + BOARD_NORTHEAST] < 0) || (!turn && grid[from + BOARD_NORTHEAST] > 0)))
				m[cmove++] = Move(from, from + BOARD_NORTHEAST, CAPTURE);
			if (from > 7 && !threatened[(int) (!turn << 6) + from + BOARD_NORTH] && ((turn && grid[from + BOARD_NORTH] < 0) || (!turn && grid[from + BOARD_NORTH] > 0)))
				m[cmove++] = Move(from, from + BOARD_NORTH, CAPTURE);
			return cmove;
		case W_PAWN:
			i = (turn) ? BOARD_NORTH : BOARD_SOUTH;
			if (from % WIDTH && ((turn && grid[from + i + BOARD_WEST] < 0) || (!turn && grid[from + i + BOARD_WEST] > 0)))
			{
				if ((turn && from > 15) || (!turn && from < 48))
					m[cmove++] = Move(from, from + i + BOARD_WEST, CAPTURE);
				else {
					m[cmove++] = Move(from, from + i + BOARD_WEST, QPROMOTEC);
					m[cmove++] = Move(from, from + i + BOARD_WEST, NPROMOTEC);
					m[cmove++] = Move(from, from + i + BOARD_WEST, BPROMOTEC);
					m[cmove++] = Move(from, from + i + BOARD_WEST, RPROMOTEC);
				}
			}
			if (from % WIDTH != 7 && ((turn && grid[from + i + BOARD_EAST] < 0) || (!turn && grid[from + i + BOARD_EAST] > 0)))
			{
				if ((turn && from > 15) || (!turn && from < 48))
					m[cmove++] = Move(from, from + i + BOARD_EAST, CAPTURE);
				else {
					m[cmove++] = Move(from, from + i + BOARD_EAST, QPROMOTEC);
					m[cmove++] = Move(from, from + i + BOARD_EAST, NPROMOTEC);
					m[cmove++] = Move(from, from + i + BOARD_EAST, BPROMOTEC);
					m[cmove++] = Move(from, from + i + BOARD_EAST, RPROMOTEC);
				}
			}
			if (hist.back().mHist.getFlags() == DOUBLEPUSH && ((hist.back().mHist.getTo() == from + BOARD_EAST && from % WIDTH != 7) || (hist.back().mHist.getTo() == from + BOARD_WEST && from % WIDTH)))
				m[cmove++] = Move(from, hist.back().mHist.getTo() + i, ENPASSANT);
			return cmove;
		case W_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && ((turn && grid[from + 10] < 0) || (!turn && grid[from + 10] > 0)))
				m[cmove++] = Move(from, from + 10, CAPTURE);
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && ((turn && grid[from + 17] < 0) || (!turn && grid[from + 17] > 0)))
				m[cmove++] = Move(from, from + 17, CAPTURE);
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && ((turn && grid[from - 10] < 0) || (!turn && grid[from - 10] > 0)))
				m[cmove++] = Move(from, from - 10, CAPTURE);
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && ((turn && grid[from - 17] < 0) || (!turn && grid[from - 17] > 0)))
				m[cmove++] = Move(from, from - 17, CAPTURE);
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && ((turn && grid[from + 6] < 0) || (!turn && grid[from + 6] > 0)))
				m[cmove++] = Move(from, from + 6, CAPTURE);
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && (turn && (grid[from + 15] < 0) || (!turn && grid[from + 15] > 0)))
				m[cmove++] = Move(from, from + 15, CAPTURE);
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && ((turn && grid[from - 6] < 0) || (!turn && grid[from - 6] > 0)))
				m[cmove++] = Move(from, from - 6, CAPTURE);
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && ((turn && grid[from - 15] < 0) || (!turn && grid[from - 15] > 0)))
				m[cmove++] = Move(from, from - 15, CAPTURE);
			return cmove;
		case W_QUEEN:
		case W_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			if (abs(grid[from]) != W_QUEEN)
				return cmove;
		case W_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST)
			{
				if (grid[i])
				{
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
		}
		return cmove;
	}

	int Board::pieceNonCapMoves(Move* m, int from)
	{//generates all pseudo legal non capture moves for one piece
		int i, cmove = 0;
		switch (abs(grid[from]))
		{
		case W_KING:
			if (turn)
			{
				if (from == 60 && !threatened[60])
				{
					if (!grid[61] && !grid[62] && !threatened[61] && !threatened[62] && (hist.back().cHist & 1 << 0))
						m[cmove++] = Move(60, 62, KCASTLE);
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[59] && !threatened[58] && (hist.back().cHist & 1 << 1))
						m[cmove++] = Move(60, 58, QCASTLE);
				}
			}
			else
			{
				if (from == 4 && !threatened[SPACES + 4])
				{
					if (!grid[5] && !grid[6] && !threatened[SPACES + 5] && !threatened[SPACES + 6] && (hist.back().cHist & 1 << 2))
						m[cmove++] = Move(4, 6, KCASTLE);
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[SPACES + 3] && !threatened[SPACES + 2] && (hist.back().cHist & 1 << 3))
						m[cmove++] = Move(4, 2, QCASTLE);
				}
			}
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && !threatened[(int) (!turn << 6) + from + BOARD_SOUTHEAST] && !grid[from + BOARD_SOUTHEAST])
				m[cmove++] = Move(from, from + BOARD_SOUTHEAST, STANDARD);
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH && !threatened[(int) (!turn << 6) + from + BOARD_EAST] && !grid[from + BOARD_EAST])
				m[cmove++] = Move(from, from + BOARD_EAST, STANDARD);
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8 && !threatened[(int) (!turn << 6) + from + BOARD_NORTHWEST] && !grid[from + BOARD_NORTHWEST])
				m[cmove++] = Move(from, from + BOARD_NORTHWEST, STANDARD);
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0 && !threatened[(int) (!turn << 6) + from + BOARD_WEST] && !grid[from + BOARD_WEST])
				m[cmove++] = Move(from, from + BOARD_WEST, STANDARD);
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && !threatened[(int) (!turn << 6) + from + BOARD_SOUTHWEST] && !grid[from + BOARD_SOUTHWEST])
				m[cmove++] = Move(from, from + BOARD_SOUTHWEST, STANDARD);
			if (from < 56 && !threatened[(int) (!turn << 6) + from + BOARD_SOUTH] && !grid[from + BOARD_SOUTH])
				m[cmove++] = Move(from, from + BOARD_SOUTH, STANDARD);
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6 && !threatened[(int) (!turn << 6) + from + BOARD_NORTHEAST] && !grid[from + BOARD_NORTHEAST])
				m[cmove++] = Move(from, from + BOARD_NORTHEAST, STANDARD);
			if (from > 7 && !threatened[(int) (!turn << 6) + from + BOARD_NORTH] && !grid[from + BOARD_NORTH])
				m[cmove++] = Move(from, from + BOARD_NORTH, STANDARD);
			return cmove;
		case W_PAWN:
			i = (turn) ? BOARD_NORTH : BOARD_SOUTH;
			if (!grid[from + i])
			{
				if ((turn && from > 15) || (!turn && from < 48))
				{
					m[cmove++] = Move(from, from + i, STANDARD);
					if (((turn && from > 47) || (!turn && from < 16)) && !grid[from + 2 * i])
						m[cmove++] = Move(from, from + 2 * i, DOUBLEPUSH);
				}
				else
				{
					m[cmove++] = Move(from, from + i, QPROMOTE);
					m[cmove++] = Move(from, from + i, NPROMOTE);
					m[cmove++] = Move(from, from + i, BPROMOTE);
					m[cmove++] = Move(from, from + i, RPROMOTE);
				}
			}
			return cmove;
		case W_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && !grid[from + 10])
				m[cmove++] = Move(from, from + 10, STANDARD);
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && !grid[from + 17])
				m[cmove++] = Move(from, from + 17, STANDARD);
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && !grid[from - 10])
				m[cmove++] = Move(from, from - 10, STANDARD);
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && !grid[from - 17])
				m[cmove++] = Move(from, from - 17, STANDARD);
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && !grid[from + 6])
				m[cmove++] = Move(from, from + 6, STANDARD);
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && !grid[from + 15])
				m[cmove++] = Move(from, from + 15, STANDARD);
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && !grid[from - 6])
				m[cmove++] = Move(from, from - 6, STANDARD);
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && !grid[from - 15])
				m[cmove++] = Move(from, from - 15, STANDARD);
			return cmove;
		case W_QUEEN:
		case W_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else
					break;
			}
			if (abs(grid[from]) != W_QUEEN)
				return cmove;
		case W_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else
					break;
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else
					break;
			}
		}
		return cmove;
	}

	int Board::pieceMoves(Move* m, int from)
	{//generates all pseudo legal moves for one piece
		int i, cmove = 0;
		switch (abs(grid[from]))
		{
		case W_KING:
			if (turn)
			{
				if (from == 60 && !threatened[60])
				{
					if (!grid[61] && !grid[62] && !threatened[61] && !threatened[62] && (hist.back().cHist & 1 << 0))
						m[cmove++] = Move(60, 62, KCASTLE);
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[59] && !threatened[58] && (hist.back().cHist & 1 << 1))
						m[cmove++] = Move(60, 58, QCASTLE);
				}
			}
			else
			{
				if (from == 4 && !threatened[SPACES + 4])
				{
					if (!grid[5] && !grid[6] && !threatened[SPACES + 5] && !threatened[SPACES + 6] && (hist.back().cHist & 1 << 2))
						m[cmove++] = Move(4, 6, KCASTLE);
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[SPACES + 3] && !threatened[SPACES + 2] && (hist.back().cHist & 1 << 3))
						m[cmove++] = Move(4, 2, QCASTLE);
				}
			}
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55)
			{
				if (!threatened[(int) (!turn << 6) + from + BOARD_SOUTHEAST] && ((turn && grid[from + BOARD_SOUTHEAST] <= 0) || (!turn && grid[from + BOARD_SOUTHEAST] >= 0)))
				{
					if (!grid[from + BOARD_SOUTHEAST])
						m[cmove++] = Move(from, from + BOARD_SOUTHEAST, STANDARD);
					else
						m[cmove++] = Move(from, from + BOARD_SOUTHEAST, CAPTURE);
				}
			}
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH)
			{
				if (!threatened[(int) (!turn << 6) + from + BOARD_EAST] && ((turn && grid[from + BOARD_EAST] <= 0) || (!turn && grid[from + BOARD_EAST] >= 0)))
				{
					if (!grid[from + BOARD_EAST])
						m[cmove++] = Move(from, from + BOARD_EAST, STANDARD);
					else
						m[cmove++] = Move(from, from + BOARD_EAST, CAPTURE);
				}
			}
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8)
			{
				if (!threatened[(int) (!turn << 6) + from + BOARD_NORTHWEST] && ((turn && grid[from + BOARD_NORTHWEST] <= 0) || (!turn && grid[from + BOARD_NORTHWEST] >= 0)))
				{
					if (!grid[from + BOARD_NORTHWEST])
						m[cmove++] = Move(from, from + BOARD_NORTHWEST, STANDARD);
					else
						m[cmove++] = Move(from, from + BOARD_NORTHWEST, CAPTURE);
				}
			}
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0)
			{
				if (!threatened[(int) (!turn << 6) + from + BOARD_WEST] && ((turn && grid[from + BOARD_WEST] <= 0) || (!turn && grid[from + BOARD_WEST] >= 0)))
				{
					if (!grid[from + BOARD_WEST])
						m[cmove++] = Move(from, from + BOARD_WEST, STANDARD);
					else
						m[cmove++] = Move(from, from + BOARD_WEST, CAPTURE);
				}
			}
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57)
			{
				if (!threatened[(int) (!turn << 6) + from + BOARD_SOUTHWEST] && ((turn && grid[from + BOARD_SOUTHWEST] <= 0) || (!turn && grid[from + BOARD_SOUTHWEST] >= 0)))
				{
					if (!grid[from + BOARD_SOUTHWEST])
						m[cmove++] = Move(from, from + BOARD_SOUTHWEST, STANDARD);
					else
						m[cmove++] = Move(from, from + BOARD_SOUTHWEST, CAPTURE);
				}
			}
			if (from < 56)
			{
				if (!threatened[(int) (!turn << 6) + from + BOARD_SOUTH] && ((turn && grid[from + BOARD_SOUTH] <= 0) || (!turn && grid[from + BOARD_SOUTH] >= 0)))
				{
					if (!grid[from + BOARD_SOUTH])
						m[cmove++] = Move(from, from + BOARD_SOUTH, STANDARD);
					else
						m[cmove++] = Move(from, from + BOARD_SOUTH, CAPTURE);
				}
			}
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6)
			{
				if (!threatened[(int) (!turn << 6) + from + BOARD_NORTHEAST] && ((turn && grid[from + BOARD_NORTHEAST] <= 0) || (!turn && grid[from + BOARD_NORTHEAST] >= 0)))
				{
					if (!grid[from + BOARD_NORTHEAST])
						m[cmove++] = Move(from, from + BOARD_NORTHEAST, STANDARD);
					else
						m[cmove++] = Move(from, from + BOARD_NORTHEAST, CAPTURE);
				}
			}
			if (from > 7) {
				if (!threatened[(int) (!turn << 6) + from + BOARD_NORTH] && ((turn && grid[from + BOARD_NORTH] <= 0) || (!turn && grid[from + BOARD_NORTH] >= 0)))
				{
					if (!grid[from + BOARD_NORTH])
						m[cmove++] = Move(from, from + BOARD_NORTH, STANDARD);
					else
						m[cmove++] = Move(from, from + BOARD_NORTH, CAPTURE);
				}
			}
			return cmove;
		case W_PAWN:
			i = (turn) ? BOARD_NORTH : BOARD_SOUTH;
			if (from % WIDTH)
			{
				if ((turn && grid[from + i + BOARD_WEST] < 0) || (!turn && grid[from + i + BOARD_WEST] > 0))
				{
					if ((turn && from > 15) || (!turn && from < 48))
						m[cmove++] = Move(from, from + i + BOARD_WEST, CAPTURE);
					else
					{
						m[cmove++] = Move(from, from + i + BOARD_WEST, QPROMOTEC);
						m[cmove++] = Move(from, from + i + BOARD_WEST, NPROMOTEC);
						m[cmove++] = Move(from, from + i + BOARD_WEST, BPROMOTEC);
						m[cmove++] = Move(from, from + i + BOARD_WEST, RPROMOTEC);
					}
				}
			}
			if (from % WIDTH != 7)
			{
				if ((turn && grid[from + i + BOARD_EAST] < 0) || (!turn && grid[from + i + BOARD_EAST] > 0))
				{
					if ((turn && from > 15) || (!turn && from < 48))
						m[cmove++] = Move(from, from + i + BOARD_EAST, CAPTURE);
					else
					{
						m[cmove++] = Move(from, from + i + BOARD_EAST, QPROMOTEC);
						m[cmove++] = Move(from, from + i + BOARD_EAST, NPROMOTEC);
						m[cmove++] = Move(from, from + i + BOARD_EAST, BPROMOTEC);
						m[cmove++] = Move(from, from + i + BOARD_EAST, RPROMOTEC);
					}
				}
			}
			if (!grid[from + i]) {
				if ((turn && from > 15) || (!turn && from < 48))
				{
					m[cmove++] = Move(from, from + i, STANDARD);
					if (((turn && from > 47) || (!turn && from < 16)) && !grid[from + 2 * i])
						m[cmove++] = Move(from, from + 2 * i, DOUBLEPUSH);
				}
				else
				{
					m[cmove++] = Move(from, from + i, QPROMOTE);
					m[cmove++] = Move(from, from + i, NPROMOTE);
					m[cmove++] = Move(from, from + i, BPROMOTE);
					m[cmove++] = Move(from, from + i, RPROMOTE);
				}
			}
			if (hist.back().mHist.getFlags() == DOUBLEPUSH && ((hist.back().mHist.getTo() == from + BOARD_EAST && from % WIDTH != 7) || (hist.back().mHist.getTo() == from + BOARD_WEST && from % WIDTH)))
				m[cmove++] = Move(from, hist.back().mHist.getTo() + i, ENPASSANT);
			return cmove;
		case W_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54)
			{
				if (!grid[from + 10])
					m[cmove++] = Move(from, from + 10, STANDARD);
				else if ((turn && grid[from + 10] < 0) || (!turn && grid[from + 10] > 0))
					m[cmove++] = Move(from, from + 10, CAPTURE);
			}
			if ((from + 17) % WIDTH > from % WIDTH && from < 47)
			{
				if (!grid[from + 17])
					m[cmove++] = Move(from, from + 17, STANDARD);
				else if ((turn && grid[from + 17] < 0) || (!turn && grid[from + 17] > 0))
					m[cmove++] = Move(from, from + 17, CAPTURE);
			}
			if ((from - 10) % WIDTH < from % WIDTH && from > 9)
			{
				if (!grid[from - 10])
					m[cmove++] = Move(from, from - 10, STANDARD);
				else if ((turn && grid[from - 10] < 0) || (!turn && grid[from - 10] > 0))
					m[cmove++] = Move(from, from - 10, CAPTURE);
			}
			if ((from - 17) % WIDTH < from % WIDTH && from > 16)
			{
				if (!grid[from - 17])
					m[cmove++] = Move(from, from - 17, STANDARD);
				else if ((turn && grid[from - 17] < 0) || (!turn && grid[from - 17] > 0))
					m[cmove++] = Move(from, from - 17, CAPTURE);
			}
			if ((from + 6) % WIDTH < from % WIDTH && from < 58)
			{
				if (!grid[from + 6])
					m[cmove++] = Move(from, from + 6, STANDARD);
				else if ((turn && grid[from + 6] < 0) || (!turn && grid[from + 6] > 0))
					m[cmove++] = Move(from, from + 6, CAPTURE);
			}
			if ((from + 15) % WIDTH < from % WIDTH && from < 49)
			{
				if (!grid[from + 15])
					m[cmove++] = Move(from, from + 15, STANDARD);
				else if (turn && (grid[from + 15] < 0) || (!turn && grid[from + 15] > 0))
					m[cmove++] = Move(from, from + 15, CAPTURE);
			}
			if ((from - 6) % WIDTH > from % WIDTH && from > 5)
			{
				if (!grid[from - 6])
					m[cmove++] = Move(from, from - 6, STANDARD);
				else if ((turn && grid[from - 6] < 0) || (!turn && grid[from - 6] > 0))
					m[cmove++] = Move(from, from - 6, CAPTURE);
			}
			if ((from - 15) % WIDTH > from % WIDTH && from > 14)
			{
				if (!grid[from - 15])
					m[cmove++] = Move(from, from - 15, STANDARD);
				else if ((turn && grid[from - 15] < 0) || (!turn && grid[from - 15] > 0))
					m[cmove++] = Move(from, from - 15, CAPTURE);
			}
			return cmove;
		case W_QUEEN:
		case W_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			if (abs(grid[from]) != W_QUEEN)
				return cmove;
		case W_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST)
			{
				if (!grid[i])
					m[cmove++] = Move(from, i, STANDARD);
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0))
						m[cmove++] = Move(from, i, CAPTURE);
					break;
				}
			}
		}
		return cmove;
	}
}