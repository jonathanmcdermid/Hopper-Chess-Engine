#include "board.h"

namespace Chess {
	bool board::validateMove(move m) {//validates a single move
		move temp[28];
		int cmove;
		if ((turn && grid[m.getFrom()] > 0) || (!turn && grid[m.getFrom()] < 0)) { cmove = pieceMoves(temp, m.getFrom()); }
		else { return false; }
		cmove = removeIllegal(temp, cmove);
		for (int i = 0; i < cmove; ++i) {
			if (temp[i] == m) { return true; }
		}
		return false;
	}

	move board::createMove(int from, int to) {
		move temp[28];
		int cmove = pieceMoves(temp, from);
		cmove = removeIllegal(temp, cmove);
		for (int i = 0; i < cmove; ++i) {
			if (temp[i].getFrom() == from && temp[i].getTo() == to) { return temp[i]; }
		}
		return NULLMOVE;
	}

	int board::removeIllegal(move* m, int cmove) {
		int checktype, to, from;
		for (int i = 0; i < cpins; ++i) {
			for (int j = 0; j < cmove; ++j) {
				if (m[j].getFrom() == pins[i][0]) {
					switch (pins[i][1]) {
					case SOUTH:
					case NORTH:
						if (!(NSslide(m[j].getFrom(), m[j].getTo()))) { goto remove; }
						else { continue; }
					case WEST:
					case EAST:
						if (!(EWslide(m[j].getFrom(), m[j].getTo()))) { goto remove; }
						else { continue; }
					case NORTHEAST:
					case SOUTHWEST:
						if (!(NESWslide(m[j].getFrom(), m[j].getTo()))) { goto remove; }
						else { continue; }
					case NORTHWEST:
					case SOUTHEAST:
						if (!(NWSEslide(m[j].getFrom(), m[j].getTo()))) { goto remove; }
						else { continue; }
					}
				remove:
					m[j--] = m[--cmove];
				}
			}
		}
		for (int i = 0; i < threatened[!turn][kpos[turn]]; ++i) {
			if (abs(grid[attackers[!turn][i][kpos[turn]]]) <= KNIGHT) { checktype = LEAP; }
			else if (NSslide(attackers[!turn][i][kpos[turn]], kpos[turn])) { checktype = (attackers[!turn][i][kpos[turn]] > kpos[turn]) ? NORTH : SOUTH; }
			else if (EWslide(attackers[!turn][i][kpos[turn]], kpos[turn])) { checktype = (attackers[!turn][i][kpos[turn]] > kpos[turn]) ? WEST : EAST; }
			else if (NESWslide(attackers[!turn][i][kpos[turn]], kpos[turn])) { checktype = (attackers[!turn][i][kpos[turn]] > kpos[turn]) ? NORTHEAST : SOUTHWEST; }
			else { checktype = (attackers[!turn][i][kpos[turn]] > kpos[turn]) ? NORTHWEST : SOUTHEAST; }
			for (int j = 0; j < cmove; ++j) {
				to = m[j].getTo();
				from = m[j].getFrom();
				if (from != kpos[turn]) {
					if (checktype != LEAP && ((to - attackers[!turn][i][kpos[turn]]) % checktype || (to < attackers[!turn][i][kpos[turn]] && to < kpos[turn]) || (to > attackers[!turn][i][kpos[turn]] && to > kpos[turn]))) { m[j--] = m[--cmove]; }
					else if (checktype == LEAP && to != attackers[!turn][i][kpos[turn]] && m[j].getFlags() != ENPASSANT) { m[j--] = m[--cmove]; }
				}
				else if (checktype != LEAP && to - from == checktype) { m[j--] = m[--cmove]; }
			}
		}
		return cmove;
	}

	int board::genAll(move* m) {//generates all legal moves
		int cmove = 0;
		if (turn) {
			for (int from = 0; from < SPACES; ++from) {
				if (grid[from] > 0) { cmove += pieceMoves(&m[cmove], from); }
			}
		}
		else {
			for (int from = 0; from < SPACES; ++from) {
				if (grid[from] < 0) { cmove += pieceMoves(&m[cmove], from); }
			}
		}
		cmove = removeIllegal(m, cmove);
		return cmove;
	}

	int board::genAllCaps(move* m) {
		int cmove = 0;
		if (turn) {
			for (int from = 0; from < SPACES; ++from) {
				if (grid[from] > 0) { cmove += pieceCapMoves(&m[cmove], from); }
			}
		}
		else {
			for (int from = 0; from < SPACES; ++from) {
				if (grid[from] < 0) { cmove += pieceCapMoves(&m[cmove], from); }
			}
		}
		cmove = removeIllegal(m, cmove);
		return cmove;
	}

	int board::genAllNonCaps(move* m) {
		int cmove = 0;
		if (turn) {
			for (int from = 0; from < SPACES; ++from) {
				if (grid[from] > 0) { cmove += pieceNonCapMoves(&m[cmove], from); }
			}
		}
		else {
			for (int from = 0; from < SPACES; ++from) {
				if (grid[from] < 0) { cmove += pieceNonCapMoves(&m[cmove], from); }
			}
		}
		cmove = removeIllegal(m, cmove);
		return cmove;
	}

	int board::pieceCapMoves(move* m, int from) {//generates all pseudo legal capture moves for one piece
		int i, cmove = 0;
		switch (abs(grid[from])) {
		case KING:
			if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && !threatened[!turn][from + SOUTHEAST] && ((turn && grid[from + SOUTHEAST] < 0) || (!turn && grid[from + SOUTHEAST] > 0))) { m[cmove++] = move(from, from + SOUTHEAST, CAPTURE); }
			if ((from + EAST) % WIDTH > from % WIDTH && !threatened[!turn][from + EAST] && ((turn && grid[from + EAST] < 0) || (!turn && grid[from + EAST] > 0))) { m[cmove++] = move(from, from + EAST, CAPTURE); }
			if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8 && !threatened[!turn][from + NORTHWEST] && ((turn && grid[from + NORTHWEST] < 0) || (!turn && grid[from + NORTHWEST] > 0))) { m[cmove++] = move(from, from + NORTHWEST, CAPTURE); }
			if ((from + WEST) % WIDTH < from % WIDTH && from > 0 && !threatened[!turn][from + WEST] && ((turn && grid[from + WEST] < 0) || (!turn && grid[from + WEST] > 0))) { m[cmove++] = move(from, from + WEST, CAPTURE); }
			if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && !threatened[!turn][from + SOUTHWEST] && ((turn && grid[from + SOUTHWEST] < 0) || (!turn && grid[from + SOUTHWEST] > 0))) { m[cmove++] = move(from, from + SOUTHWEST, CAPTURE); }
			if (from < 56 && !threatened[!turn][from + SOUTH] && ((turn && grid[from + SOUTH] < 0) || (!turn && grid[from + SOUTH] > 0))) { m[cmove++] = move(from, from + SOUTH, CAPTURE); }
			if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6 && !threatened[!turn][from + NORTHEAST] && ((turn && grid[from + NORTHEAST] < 0) || (!turn && grid[from + NORTHEAST] > 0))) { m[cmove++] = move(from, from + NORTHEAST, CAPTURE); }
			if (from > 7 && !threatened[!turn][from + NORTH] && ((turn && grid[from + NORTH] < 0) || (!turn && grid[from + NORTH] > 0))) { m[cmove++] = move(from, from + NORTH, CAPTURE); }
			return cmove;
		case PAWN:
			i = (turn) ? NORTH : SOUTH;
			if (from % WIDTH && ((turn && grid[from + i + WEST] < 0) || (!turn && grid[from + i + WEST] > 0))) {
				if ((turn && from > 15) || (!turn && from < 48)) { m[cmove++] = move(from, from + i + WEST, CAPTURE); }
				else {
					m[cmove++] = move(from, from + i + WEST, QPROMOTEC);
					m[cmove++] = move(from, from + i + WEST, NPROMOTEC);
					m[cmove++] = move(from, from + i + WEST, BPROMOTEC);
					m[cmove++] = move(from, from + i + WEST, RPROMOTEC);
				}
			}
			if (from % WIDTH != 7 && ((turn && grid[from + i + EAST] < 0) || (!turn && grid[from + i + EAST] > 0))) {
				if ((turn && from > 15) || (!turn && from < 48)) { m[cmove++] = move(from, from + i + EAST, CAPTURE); }
				else {
					m[cmove++] = move(from, from + i + EAST, QPROMOTEC);
					m[cmove++] = move(from, from + i + EAST, NPROMOTEC);
					m[cmove++] = move(from, from + i + EAST, BPROMOTEC);
					m[cmove++] = move(from, from + i + EAST, RPROMOTEC);
				}
			}
			if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && ((mHist[cturn - 1].getTo() == from + EAST && from % WIDTH != 7) || (mHist[cturn - 1].getTo() == from + WEST && from % WIDTH))) { m[cmove++] = move(from, mHist[cturn - 1].getTo() + i, ENPASSANT); }
			return cmove;
		case KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && ((turn && grid[from + 10] < 0) || (!turn && grid[from + 10] > 0))) { m[cmove++] = move(from, from + 10, CAPTURE); }
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && ((turn && grid[from + 17] < 0) || (!turn && grid[from + 17] > 0))) { m[cmove++] = move(from, from + 17, CAPTURE); }
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && ((turn && grid[from - 10] < 0) || (!turn && grid[from - 10] > 0))) { m[cmove++] = move(from, from - 10, CAPTURE); }
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && ((turn && grid[from - 17] < 0) || (!turn && grid[from - 17] > 0))) { m[cmove++] = move(from, from - 17, CAPTURE); }
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && ((turn && grid[from + 6] < 0) || (!turn && grid[from + 6] > 0))) { m[cmove++] = move(from, from + 6, CAPTURE); }
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && (turn && (grid[from + 15] < 0) || (!turn && grid[from + 15] > 0))) { m[cmove++] = move(from, from + 15, CAPTURE); }
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && ((turn && grid[from - 6] < 0) || (!turn && grid[from - 6] > 0))) { m[cmove++] = move(from, from - 6, CAPTURE); }
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && ((turn && grid[from - 15] < 0) || (!turn && grid[from - 15] > 0))) { m[cmove++] = move(from, from - 15, CAPTURE); }
			return cmove;
		case QUEEN:
		case ROOK:
			for (i = from + NORTH; i >= 0; i += NORTH) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			if (abs(grid[from]) != QUEEN) { return cmove; }
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
		}
		return cmove;
	}

	int board::pieceNonCapMoves(move* m, int from) {//generates all pseudo legal non capture moves for one piece
		int i, cmove = 0;
		switch (abs(grid[from])) {
		case KING:
			if (turn) {
				if (from == 60 && !threatened[BLACK][60] && (cHist[cturn - 1] & 1 << 2)) {
					if (!grid[61] && !grid[62] && !threatened[BLACK][61] && !threatened[BLACK][62] && (cHist[cturn - 1] & 1 << 0)) { m[cmove++] = move(60, 62, KCASTLE); }
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[BLACK][59] && !threatened[BLACK][58] && (cHist[cturn - 1] & 1 << 1)) { m[cmove++] = move(60, 58, QCASTLE); }
				}
			}
			else {
				if (from == 4 && !threatened[WHITE][4] && (cHist[cturn - 1] & 1 << 4)) {
					if (!grid[5] && !grid[6] && !threatened[WHITE][5] && !threatened[WHITE][6] && (cHist[cturn - 1] & 1 << 3)) { m[cmove++] = move(4, 6, KCASTLE); }
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[WHITE][3] && !threatened[WHITE][2] && (cHist[cturn - 1] & 1 << 5)) { m[cmove++] = move(4, 2, QCASTLE); }
				}
			}
			if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && !threatened[!turn][from + SOUTHEAST] && !grid[from + SOUTHEAST]) { m[cmove++] = move(from, from + SOUTHEAST, STANDARD); }
			if ((from + EAST) % WIDTH > from % WIDTH && !threatened[!turn][from + EAST] && !grid[from + EAST]) { m[cmove++] = move(from, from + EAST, STANDARD); }
			if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8 && !threatened[!turn][from + NORTHWEST] && !grid[from + NORTHWEST]) { m[cmove++] = move(from, from + NORTHWEST, STANDARD); }
			if ((from + WEST) % WIDTH < from % WIDTH && from > 0 && !threatened[!turn][from + WEST] && !grid[from + WEST]) { m[cmove++] = move(from, from + WEST, STANDARD); }
			if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && !threatened[!turn][from + SOUTHWEST] && !grid[from + SOUTHWEST]) { m[cmove++] = move(from, from + SOUTHWEST, STANDARD); }
			if (from < 56 && !threatened[!turn][from + SOUTH] && !grid[from + SOUTH]) { m[cmove++] = move(from, from + SOUTH, STANDARD); }
			if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6 && !threatened[!turn][from + NORTHEAST] && !grid[from + NORTHEAST]) { m[cmove++] = move(from, from + NORTHEAST, STANDARD); }
			if (from > 7 && !threatened[!turn][from + NORTH] && !grid[from + NORTH]) { m[cmove++] = move(from, from + NORTH, STANDARD); }
			return cmove;
		case PAWN:
			i = (turn) ? NORTH : SOUTH;
			if (!grid[from + i]) {
				if ((turn && from > 15) || (!turn && from < 48)) {
					m[cmove++] = move(from, from + i, STANDARD);
					if (((turn && from > 47) || (!turn && from < 16)) && !grid[from + 2 * i]) { m[cmove++] = move(from, from + 2 * i, DOUBLEPUSH); }
				}
				else {
					m[cmove++] = move(from, from + i, QPROMOTE);
					m[cmove++] = move(from, from + i, NPROMOTE);
					m[cmove++] = move(from, from + i, BPROMOTE);
					m[cmove++] = move(from, from + i, RPROMOTE);
				}
			}
			return cmove;
		case KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && !grid[from + 10]) { m[cmove++] = move(from, from + 10, STANDARD); }
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && !grid[from + 17]) { m[cmove++] = move(from, from + 17, STANDARD); }
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && !grid[from - 10]) { m[cmove++] = move(from, from - 10, STANDARD); }
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && !grid[from - 17]) { m[cmove++] = move(from, from - 17, STANDARD); }
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && !grid[from + 6]) { m[cmove++] = move(from, from + 6, STANDARD); }
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && !grid[from + 15]) { m[cmove++] = move(from, from + 15, STANDARD); }
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && !grid[from - 6]) { m[cmove++] = move(from, from - 6, STANDARD); }
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && !grid[from - 15]) { m[cmove++] = move(from, from - 15, STANDARD); }
			return cmove;
		case QUEEN:
		case ROOK:
			for (i = from + NORTH; i >= 0; i += NORTH) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else { break; }
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else { break; }
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else { break; }
			}
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else { break; }
			}
			if (abs(grid[from]) != QUEEN) { return cmove; }
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else { break; }
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else { break; }
			}
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else { break; }
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else { break; }
			}
		}
		return cmove;
	}

	int board::pieceMoves(move* m, int from) {//generates all pseudo legal moves for one piece
		int i, cmove = 0;
		switch (abs(grid[from])) {
		case KING:
			if (turn) {
				if (from == 60 && !threatened[BLACK][60] && (cHist[cturn - 1] & 1 << 2)) {
					if (!grid[61] && !grid[62] && !threatened[BLACK][61] && !threatened[BLACK][62] && (cHist[cturn - 1] & 1 << 0)) {
						m[cmove++] = move(60, 62, KCASTLE);
					}
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[BLACK][59] && !threatened[BLACK][58] && (cHist[cturn - 1] & 1 << 1)) {
						m[cmove++] = move(60, 58, QCASTLE);
					}
				}
			}
			else {
				if (from == 4 && !threatened[WHITE][4] && (cHist[cturn - 1] & 1 << 4)) {
					if (!grid[5] && !grid[6] && !threatened[WHITE][5] && !threatened[WHITE][6] && (cHist[cturn - 1] & 1 << 3)) {
						m[cmove++] = move(4, 6, KCASTLE);
					}
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[WHITE][3] && !threatened[WHITE][2] && (cHist[cturn - 1] & 1 << 5)) {
						m[cmove++] = move(4, 2, QCASTLE);
					}
				}
			}
			if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) {
				if (!threatened[!turn][from + SOUTHEAST] && ((turn && grid[from + SOUTHEAST] <= 0) || (!turn && grid[from + SOUTHEAST] >= 0))) {
					if (!grid[from + SOUTHEAST]) { m[cmove++] = move(from, from + SOUTHEAST, STANDARD); }
					else { m[cmove++] = move(from, from + SOUTHEAST, CAPTURE); }
				}
			}
			if ((from + EAST) % WIDTH > from % WIDTH) {
				if (!threatened[!turn][from + EAST] && ((turn && grid[from + EAST] <= 0) || (!turn && grid[from + EAST] >= 0))) {
					if (!grid[from + EAST]) { m[cmove++] = move(from, from + EAST, STANDARD); }
					else { m[cmove++] = move(from, from + EAST, CAPTURE); }
				}
			}
			if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) {
				if (!threatened[!turn][from + NORTHWEST] && ((turn && grid[from + NORTHWEST] <= 0) || (!turn && grid[from + NORTHWEST] >= 0))) {
					if (!grid[from + NORTHWEST]) { m[cmove++] = move(from, from + NORTHWEST, STANDARD); }
					else { m[cmove++] = move(from, from + NORTHWEST, CAPTURE); }
				}
			}
			if ((from + WEST) % WIDTH < from % WIDTH && from > 0) {
				if (!threatened[!turn][from + WEST] && ((turn && grid[from + WEST] <= 0) || (!turn && grid[from + WEST] >= 0))) {
					if (!grid[from + WEST]) { m[cmove++] = move(from, from + WEST, STANDARD); }
					else { m[cmove++] = move(from, from + WEST, CAPTURE); }
				}
			}
			if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) {
				if (!threatened[!turn][from + SOUTHWEST] && ((turn && grid[from + SOUTHWEST] <= 0) || (!turn && grid[from + SOUTHWEST] >= 0))) {
					if (!grid[from + SOUTHWEST]) { m[cmove++] = move(from, from + SOUTHWEST, STANDARD); }
					else { m[cmove++] = move(from, from + SOUTHWEST, CAPTURE); }
				}
			}
			if (from < 56) {
				if (!threatened[!turn][from + SOUTH] && ((turn && grid[from + SOUTH] <= 0) || (!turn && grid[from + SOUTH] >= 0))) {
					if (!grid[from + SOUTH]) { m[cmove++] = move(from, from + SOUTH, STANDARD); }
					else { m[cmove++] = move(from, from + SOUTH, CAPTURE); }
				}
			}
			if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) {
				if (!threatened[!turn][from + NORTHEAST] && ((turn && grid[from + NORTHEAST] <= 0) || (!turn && grid[from + NORTHEAST] >= 0))) {
					if (!grid[from + NORTHEAST]) { m[cmove++] = move(from, from + NORTHEAST, STANDARD); }
					else { m[cmove++] = move(from, from + NORTHEAST, CAPTURE); }
				}
			}
			if (from > 7) {
				if (!threatened[!turn][from + NORTH] && ((turn && grid[from + NORTH] <= 0) || (!turn && grid[from + NORTH] >= 0))) {
					if (!grid[from + NORTH]) { m[cmove++] = move(from, from + NORTH, STANDARD); }
					else { m[cmove++] = move(from, from + NORTH, CAPTURE); }
				}
			}
			return cmove;
		case PAWN:
			i = (turn) ? NORTH : SOUTH;
			if (from % WIDTH) {
				if ((turn && grid[from + i + WEST] < 0) || (!turn && grid[from + i + WEST] > 0)) {
					if ((turn && from > 15) || (!turn && from < 48)) { m[cmove++] = move(from, from + i + WEST, CAPTURE); }
					else {
						m[cmove++] = move(from, from + i + WEST, QPROMOTEC);
						m[cmove++] = move(from, from + i + WEST, NPROMOTEC);
						m[cmove++] = move(from, from + i + WEST, BPROMOTEC);
						m[cmove++] = move(from, from + i + WEST, RPROMOTEC);
					}
				}
			}
			if (from % WIDTH != 7) {
				if ((turn && grid[from + i + EAST] < 0) || (!turn && grid[from + i + EAST] > 0)) {
					if ((turn && from > 15) || (!turn && from < 48)) { m[cmove++] = move(from, from + i + EAST, CAPTURE); }
					else {
						m[cmove++] = move(from, from + i + EAST, QPROMOTEC);
						m[cmove++] = move(from, from + i + EAST, NPROMOTEC);
						m[cmove++] = move(from, from + i + EAST, BPROMOTEC);
						m[cmove++] = move(from, from + i + EAST, RPROMOTEC);
					}
				}
			}
			if (!grid[from + i]) {
				if ((turn && from > 15) || (!turn && from < 48)) {
					m[cmove++] = move(from, from + i, STANDARD);
					if (((turn && from > 47) || (!turn && from < 16)) && !grid[from + 2 * i]) { m[cmove++] = move(from, from + 2 * i, DOUBLEPUSH); }
				}
				else {
					m[cmove++] = move(from, from + i, QPROMOTE);
					m[cmove++] = move(from, from + i, NPROMOTE);
					m[cmove++] = move(from, from + i, BPROMOTE);
					m[cmove++] = move(from, from + i, RPROMOTE);
				}
			}
			if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && ((mHist[cturn - 1].getTo() == from + EAST && from % WIDTH != 7) || (mHist[cturn - 1].getTo() == from + WEST && from % WIDTH))) { m[cmove++] = move(from, mHist[cturn - 1].getTo() + i, ENPASSANT); }
			return cmove;
		case KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54) {
				if (!grid[from + 10]) { m[cmove++] = move(from, from + 10, STANDARD); }
				else if ((turn && grid[from + 10] < 0) || (!turn && grid[from + 10] > 0)) { m[cmove++] = move(from, from + 10, CAPTURE); }
			}
			if ((from + 17) % WIDTH > from % WIDTH && from < 47) {
				if (!grid[from + 17]) { m[cmove++] = move(from, from + 17, STANDARD); }
				else if ((turn && grid[from + 17] < 0) || (!turn && grid[from + 17] > 0)) { m[cmove++] = move(from, from + 17, CAPTURE); }
			}
			if ((from - 10) % WIDTH < from % WIDTH && from > 9) {
				if (!grid[from - 10]) { m[cmove++] = move(from, from - 10, STANDARD); }
				else if ((turn && grid[from - 10] < 0) || (!turn && grid[from - 10] > 0)) { m[cmove++] = move(from, from - 10, CAPTURE); }
			}
			if ((from - 17) % WIDTH < from % WIDTH && from > 16) {
				if (!grid[from - 17]) { m[cmove++] = move(from, from - 17, STANDARD); }
				else if ((turn && grid[from - 17] < 0) || (!turn && grid[from - 17] > 0)) { m[cmove++] = move(from, from - 17, CAPTURE); }
			}
			if ((from + 6) % WIDTH < from % WIDTH && from < 58) {
				if (!grid[from + 6]) { m[cmove++] = move(from, from + 6, STANDARD); }
				else if ((turn && grid[from + 6] < 0) || (!turn && grid[from + 6] > 0)) { m[cmove++] = move(from, from + 6, CAPTURE); }
			}
			if ((from + 15) % WIDTH < from % WIDTH && from < 49) {
				if (!grid[from + 15]) { m[cmove++] = move(from, from + 15, STANDARD); }
				else if (turn && (grid[from + 15] < 0) || (!turn && grid[from + 15] > 0)) { m[cmove++] = move(from, from + 15, CAPTURE); }
			}
			if ((from - 6) % WIDTH > from % WIDTH && from > 5) {
				if (!grid[from - 6]) { m[cmove++] = move(from, from - 6, STANDARD); }
				else if ((turn && grid[from - 6] < 0) || (!turn && grid[from - 6] > 0)) { m[cmove++] = move(from, from - 6, CAPTURE); }
			}
			if ((from - 15) % WIDTH > from % WIDTH && from > 14) {
				if (!grid[from - 15]) { m[cmove++] = move(from, from - 15, STANDARD); }
				else if ((turn && grid[from - 15] < 0) || (!turn && grid[from - 15] > 0)) { m[cmove++] = move(from, from - 15, CAPTURE); }
			}
			return cmove;
		case QUEEN:
		case ROOK:
			for (i = from + NORTH; i >= 0; i += NORTH) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			if (abs(grid[from]) != QUEEN) { return cmove; }
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				if (!grid[i]) { m[cmove++] = move(from, i, STANDARD); }
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) { m[cmove++] = move(from, i, CAPTURE); }
					break;
				}
			}
		}
		return cmove;
	}
}