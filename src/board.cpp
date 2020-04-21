#include "board.h"

namespace Chess {
	board::board() {//sets board to starting state
		z = zobrist();
		fenSet(STARTFEN);
	}

	void board::fenSet(std::string fs) {//sets board to state outlined in FEN string
		cturn = 0;
		int index = 0, counter = 0, helper;
		while (fs[index] != ' ') {
			switch (fs[index]) {
			case 'P': { grid[counter] = PAWN; ++counter; break; }
			case 'R': { grid[counter] = ROOK; ++counter; break; }
			case 'N': { grid[counter] = KNIGHT; ++counter; break; }
			case 'B': { grid[counter] = BISHOP; ++counter; break; }
			case 'Q': { grid[counter] = QUEEN; ++counter; break; }
			case 'K': { grid[counter] = KING; kpos[WHITE] = counter++; break; }
			case 'p': { grid[counter] = -PAWN; ++counter; break; }
			case 'r': { grid[counter] = -ROOK; ++counter; break; }
			case 'n': { grid[counter] = -KNIGHT; ++counter; break; }
			case 'b': { grid[counter] = -BISHOP; ++counter; break; }
			case 'q': { grid[counter] = -QUEEN; ++counter; break; }
			case 'k': { grid[counter] = -KING; kpos[BLACK] = counter++; break; }
			case '/': { break; }
			default:
				helper = fs[index] - '0';
				while (helper--) { grid[counter++] = EMPTY; }
			}
			++index;
		}
		turn = fs[++index] == 'w';
		++index;
		currC = 0;
		do {
			switch (fs[index++]) {
			case 'K': {currC |= 1 << 0; currC |= 1 << 2; break; }
			case 'Q': {currC |= 1 << 1; currC |= 1 << 2; break; }
			case 'k': {currC |= 1 << 3; currC |= 1 << 4; break; }
			case 'q': {currC |= 1 << 5; currC |= 1 << 4; break; }
			}
		} while (fs[index] != ' ');
		if (fs[++index] != '-') {
			int from = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
			int to = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			mHist[cturn] = move(from, to, DOUBLEPUSH);
			++cturn;
		}
		index += 2;
		fmr = fs[index] - '0';
		currV = 0;
		for (int i = 0; i < SPACES; ++i) { currV += grid[i]; }
		currZ = z.newKey(this);
		currM = (cturn) ? mHist[cturn - 1] : move();
		allThreats();
	}

	bool board::isCheckMate() {
		if (isDraw()) { return true; }
		move m[28];
		int cmove;
		if (turn) {
			for (int i = 0; i < SPACES; ++i) {
				if (grid[i] > 0) {
					cmove = pieceMoves(m, i);
					if (cmove) {
						cmove = removeIllegal(m, cmove);
						if (cmove) { return false; }
					}
				}
			}
		}
		else {
			for (int i = 0; i < SPACES; ++i) {
				if (grid[i] < 0) {
					cmove = pieceMoves(m, i);
					if (cmove) {
						cmove = removeIllegal(m, cmove);
						if (cmove) { return false; }
					}
				}
			}
		}
		return true;
	}

	bool board::isDraw() { 
		if (fmr >= 100) { return true; }
		for (int i = 4; i < fmr; i+=4) {
			if (zHist[cturn - i] == zHist[cturn]) { return true; }
		}
		return false;
	}

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
		return move();
	}

	void board::movePiece(move m) {//executes a move if legal, return value depicts success (nullmoves considered legal)
		zHist[cturn] = currZ;
		cHist[cturn] = currC;
		fHist[cturn] = fmr;
		switch (m.getFlags()) {
		case STANDARD:
			currZ = (turn) ? currZ ^ z.pieces[grid[m.getFrom()] % 10][WHITE][m.getFrom()] : currZ ^ z.pieces[-grid[m.getFrom()] % 10][BLACK][m.getFrom()];
			currZ = (turn) ? currZ ^ z.pieces[grid[m.getFrom()] % 10][WHITE][m.getTo()] : currZ ^ z.pieces[-grid[m.getFrom()] % 10][BLACK][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			if (abs(grid[m.getTo()]) == KING) { kpos[turn] = m.getTo(); }
			break;
		case DOUBLEPUSH:
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[PINDEX][turn][m.getTo()];
			currZ ^= z.enpassant[m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			goto nocastle;
		case KCASTLE:
			currZ ^= z.pieces[KINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[KINDEX][turn][m.getTo()];
			currZ ^= z.pieces[RINDEX][turn][m.getTo() + 1];
			currZ ^= z.pieces[RINDEX][turn][m.getTo() - 1];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() - 1] = grid[m.getTo() + 1];
			grid[m.getTo() + 1] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			kpos[turn] = m.getTo();
			break;
		case QCASTLE:
			currZ ^= z.pieces[KINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[KINDEX][turn][m.getTo()];
			currZ ^= z.pieces[RINDEX][turn][m.getTo() - 2];
			currZ ^= z.pieces[RINDEX][turn][m.getTo() + 1];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() + 1] = grid[m.getTo() - 2];
			grid[m.getTo() - 2] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			kpos[turn] = m.getTo();
			break;
		case ENPASSANT:
			currV += (turn) ? PAWN : -PAWN;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[PINDEX][!turn][currM.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[currM.getTo()] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			goto nocastle;
		case CAPTURE:
			currV -= grid[m.getTo()];
			currZ ^= z.pieces[abs(grid[m.getFrom()]) % 10][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[abs(grid[m.getFrom()]) % 10][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			if (abs(grid[m.getTo()]) == KING) { kpos[turn] = m.getTo(); }
			break;
		case NPROMOTE:
			currV += (turn) ? -PAWN + KNIGHT : PAWN - KNIGHT;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			grid[m.getFrom()] = EMPTY;
			goto nocastle;
		case BPROMOTE:
			currV += (turn) ? -PAWN + BISHOP : PAWN - BISHOP;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			grid[m.getFrom()] = EMPTY;
			goto nocastle;
		case RPROMOTE:
			currV += (turn) ? -PAWN + ROOK : PAWN - ROOK;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			grid[m.getFrom()] = EMPTY;
			goto nocastle;
		case QPROMOTE:
			currV += (turn) ? -PAWN + QUEEN : PAWN - QUEEN;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			grid[m.getFrom()] = EMPTY;
			goto nocastle;
		case NPROMOTEC:
			currV += (turn) ? -PAWN + KNIGHT - grid[m.getTo()] : PAWN - KNIGHT - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			currV += (turn) ? -PAWN + BISHOP - grid[m.getTo()] : PAWN - BISHOP - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			currV += (turn) ? -PAWN + ROOK - grid[m.getTo()] : PAWN - ROOK - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			currV += (turn) ? -PAWN + QUEEN - grid[m.getTo()] : +PAWN - QUEEN - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			grid[m.getFrom()] = EMPTY;
		case NULLMOVE:
			goto nocastle;
		}
		if (currC & 1 << 2) {
			if (m.getFrom() == 60) {
				currC &= ~(1 << 2);
				if (currC & 1 << 0) { currC &= ~(1 << 0); currZ ^= z.castle[WHITE][0]; }
				if (currC & 1 << 1) { currC &= ~(1 << 1); currZ ^= z.castle[WHITE][1]; }
			}
			else if (currC & 1 << 0 && (m.getTo() == 63 || m.getFrom() == 63)) { currC &= ~(1 << 0); currZ ^= z.castle[WHITE][0]; }
			else if (currC & 1 << 1 && (m.getTo() == 56 || m.getFrom() == 56)) { currC &= ~(1 << 1); currZ ^= z.castle[WHITE][1]; }
		}
		if (currC & 1 << 4) {
			if (m.getFrom() == 4) {
				currC &= ~(1 << 4);
				if (currC & 1 << 3) { currC &= ~(1 << 3); currZ ^= z.castle[BLACK][0]; }
				if (currC & 1 << 5) { currC &= ~(1 << 5); currZ ^= z.castle[BLACK][1]; }
			}
			else if ((currC & 1 << 3) && (m.getTo() == 7 || m.getFrom() == 7)) { currC &= ~(1 << 3); currZ ^= z.castle[BLACK][0]; }
			else if ((currC & 1 << 5) && (m.getTo() == 0 || m.getFrom() == 0)) { currC &= ~(1 << 5); currZ ^= z.castle[BLACK][1]; }
		}
	nocastle:
		if (currM.getFlags() == DOUBLEPUSH) { currZ ^= z.enpassant[currM.getTo()]; }
		fmr = (m.getFlags() == STANDARD && abs(grid[m.getTo()]) != PAWN) ? ++fmr : 0;
		currZ ^= z.side;
		vHist[cturn] = currV;
		mHist[cturn] = m;
		currM = m;
		++cturn;
		turn = !turn;
		allThreats();
	}

	void board::unmovePiece() {//unmakes a move
		turn = !turn;
		--cturn;
		switch (currM.getFlags()) {
		case STANDARD:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = EMPTY;
			if (abs(grid[currM.getFrom()]) == KING) { kpos[turn] = currM.getFrom(); }
			break;
		case DOUBLEPUSH:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = EMPTY;
			break;
		case KCASTLE:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = EMPTY;
			grid[currM.getTo() + 1] = grid[currM.getTo() - 1];
			grid[currM.getTo() - 1] = EMPTY;
			kpos[turn] = currM.getFrom();
			break;
		case QCASTLE:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = EMPTY;
			grid[currM.getTo() - 2] = grid[currM.getTo() + 1];
			grid[currM.getTo() + 1] = EMPTY;
			kpos[turn] = currM.getFrom();
			break;
		case CAPTURE:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = vHist[cturn - 1] - currV;
			if (abs(grid[currM.getFrom()]) == KING) { kpos[turn] = currM.getFrom(); }
			break;
		case ENPASSANT:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = EMPTY;
			grid[mHist[cturn - 1].getTo()] = (turn) ? -PAWN : PAWN;
			break;
		case NPROMOTE:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = EMPTY;
			break;
		case BPROMOTE:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = EMPTY;
			break;
		case RPROMOTE:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = EMPTY;
			break;
		case QPROMOTE:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = EMPTY;
			break;
		case NPROMOTEC:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = (turn) ? vHist[cturn - 1] - currV + KNIGHT - PAWN : vHist[cturn - 1] - currV - KNIGHT + PAWN;
			break;
		case BPROMOTEC:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = (turn) ? vHist[cturn - 1] - currV + BISHOP - PAWN : vHist[cturn - 1] - currV - BISHOP + PAWN;
			break;
		case RPROMOTEC:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = (turn) ? vHist[cturn - 1] - currV + ROOK - PAWN : vHist[cturn - 1] - currV - ROOK + PAWN;
			break;
		case QPROMOTEC:
			grid[currM.getFrom()] = (turn) ? PAWN : -PAWN;
			grid[currM.getTo()] = (turn) ? vHist[cturn - 1] - currV + QUEEN - PAWN : vHist[cturn - 1] - currV - QUEEN + PAWN;
			break;
		}
		fmr = fHist[cturn];
		currZ = zHist[cturn];
		currC = cHist[cturn];
		currV = vHist[cturn - 1];
		currM = mHist[cturn - 1];
		allThreats();
	}

	void board::allThreats() {
		cpins = 0;
		endgame = true;
		zugswang = true;
		for (int i = 0; i < SPACES; ++i) {
			threatened[BLACK][i] = 0;
			threatened[WHITE][i] = 0;
		}
		for (int from = 0; from < SPACES; ++from) {
			if (grid[from]) { pieceThreats(from); }
		}
	}

	int board::removeIllegal(move* m, int cmove) {
		int checktype;
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
				int to = m[j].getTo();
				int from = m[j].getFrom();
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
			if (currM.getFlags() == DOUBLEPUSH && ((currM.getTo() == from + EAST && from % WIDTH != 7) || (currM.getTo() == from + WEST && from % WIDTH))) { m[cmove++] = move(from, currM.getTo() + i, ENPASSANT); }
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
				if (from == 60 && !threatened[BLACK][60] && (currC & 1 << 2)) {
					if (!grid[61] && !grid[62] && !threatened[BLACK][61] && !threatened[BLACK][62] && (currC & 1 << 0)) { m[cmove++] = move(60, 62, KCASTLE); }
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[BLACK][59] && !threatened[BLACK][58] && (currC & 1 << 1)) { m[cmove++] = move(60, 58, QCASTLE); }
				}
			}
			else {
				if (from == 4 && !threatened[WHITE][4] && (currC & 1 << 4)) {
					if (!grid[5] && !grid[6] && !threatened[WHITE][5] && !threatened[WHITE][6] && (currC & 1 << 3)) { m[cmove++] = move(4, 6, KCASTLE); }
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[WHITE][3] && !threatened[WHITE][2] && (currC & 1 << 5)) { m[cmove++] = move(4, 2, QCASTLE); }
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
				if (from == 60 && !threatened[BLACK][60] && (currC & 1 << 2)) {
					if (!grid[61] && !grid[62] && !threatened[BLACK][61] && !threatened[BLACK][62] && (currC & 1 << 0)) {
						m[cmove++] = move(60, 62, KCASTLE);
					}
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[BLACK][59] && !threatened[BLACK][58] && (currC & 1 << 1)) {
						m[cmove++] = move(60, 58, QCASTLE);
					}
				}
			}
			else {
				if (from == 4 && !threatened[WHITE][4] && (currC & 1 << 4)) {
					if (!grid[5] && !grid[6] && !threatened[WHITE][5] && !threatened[WHITE][6] && (currC & 1 << 3)) {
						m[cmove++] = move(4, 6, KCASTLE);
					}
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[WHITE][3] && !threatened[WHITE][2] && (currC & 1 << 5)) {
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
			if (currM.getFlags() == DOUBLEPUSH && ((currM.getTo() == from + EAST && from % WIDTH != 7) || (currM.getTo() == from + WEST && from % WIDTH))) { m[cmove++] = move(from, currM.getTo() + i, ENPASSANT); }
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

	void board::pieceThreats(int from) {//generates all pseudo legal moves for one piece
		int i, j;
		bool us = (grid[from] > 0) ? WHITE : BLACK;
		switch (abs(grid[from])) {
		case KING:
			if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) { attackers[us][threatened[us][from + SOUTHEAST]++][from + SOUTHEAST] = from; }
			if ((from + EAST) % WIDTH > from % WIDTH) { attackers[us][threatened[us][from + EAST]++][from + EAST] = from; }
			if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) { attackers[us][threatened[us][from + NORTHWEST]++][from + NORTHWEST] = from; }
			if ((from + WEST) % WIDTH < from % WIDTH && from > 0) { attackers[us][threatened[us][from + WEST]++][from + WEST] = from; }
			if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) { attackers[us][threatened[us][from + SOUTHWEST]++][from + SOUTHWEST] = from; }
			if (from < 56) { attackers[us][threatened[us][from + SOUTH]++][from + SOUTH] = from; }
			if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) { attackers[us][threatened[us][from + NORTHEAST]++][from + NORTHEAST] = from; }
			if (from > 7) { attackers[us][threatened[us][from + NORTH]++][from + NORTH] = from; }
			return;
		case PAWN:
			i = (us) ? NORTH : SOUTH;
			if (from % WIDTH) { attackers[us][threatened[us][from + i + WEST]++][from + i + WEST] = from; }
			if (from % WIDTH != 7) { attackers[us][threatened[us][from + i + EAST]++][from + i + EAST] = from; }
			break;
		case KNIGHT:
			zugswang = false;
			if ((from + 10) % WIDTH > from % WIDTH && from < 54) { attackers[us][threatened[us][from + 10]++][from + 10] = from; }
			if ((from + 17) % WIDTH > from % WIDTH && from < 47) { attackers[us][threatened[us][from + 17]++][from + 17] = from; }
			if ((from - 10) % WIDTH < from % WIDTH && from > 9) { attackers[us][threatened[us][from - 10]++][from - 10] = from; }
			if ((from - 17) % WIDTH < from % WIDTH && from > 16) { attackers[us][threatened[us][from - 17]++][from - 17] = from; }
			if ((from + 6) % WIDTH < from % WIDTH && from < 58) { attackers[us][threatened[us][from + 6]++][from + 6] = from; }
			if ((from + 15) % WIDTH < from % WIDTH && from < 49) { attackers[us][threatened[us][from + 15]++][from + 15] = from; }
			if ((from - 6) % WIDTH > from % WIDTH && from > 5) { attackers[us][threatened[us][from - 6]++][from - 6] = from; }
			if ((from - 15) % WIDTH > from % WIDTH && from > 14) { attackers[us][threatened[us][from - 15]++][from - 15] = from; }
			break;
		case QUEEN:
			endgame = false;
		case ROOK:
			zugswang = false;
			for (i = from + NORTH; i >= 0; i += NORTH) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = NORTH;
						for (j = i + NORTH; j != kpos[turn]; j += NORTH) {
							if (grid[j]) { goto failN; }
						}
						++cpins;
					}
					break;
				}
			}
		failN:
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = SOUTH;
						for (j = i + SOUTH; j != kpos[turn]; j += SOUTH) {
							if (grid[j]) { goto failS; }
						}
						++cpins;
					}
					break;
				}
			}
		failS:
			for (i = from + EAST; i % WIDTH; i += EAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = EAST;
						for (j = i + EAST; j != kpos[turn]; j += EAST) {
							if (grid[j]) { goto failE; }
						}
						++cpins;
					}
					break;
				}
			}
		failE:
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = WEST;
						for (j = i + WEST; j != kpos[turn]; j += WEST) {
							if (grid[j]) { goto failW; }
						}
						++cpins;
					}
					break;
				}
			}
		failW:
			if (abs(grid[from]) != QUEEN) { break; }
		case BISHOP:
			zugswang = false;
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = NORTHEAST;
						for (j = i + NORTHEAST; j != kpos[turn]; j += NORTHEAST) {
							if (grid[j]) { goto failNE; }
						}
						++cpins;
					}
					break;
				}
			}
		failNE:
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = NORTHWEST;
						for (j = i + NORTHWEST; j != kpos[turn]; j += NORTHWEST) {
							if (grid[j]) { goto failNW; }
						}
						++cpins;
					}
					break;
				}
			}
		failNW:
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = SOUTHEAST;
						for (j = i + SOUTHEAST; j != kpos[turn]; j += SOUTHEAST) {
							if (grid[j]) { goto failSE; }
						}
						++cpins;
					}
					break;
				}
			}
		failSE:
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = SOUTHWEST;
						for (j = i + SOUTHWEST; j != kpos[turn]; j += SOUTHWEST) {
							if (grid[j]) { goto failSW; }
						}
						++cpins;
					}
					break;
				}
			}
		failSW:
			break;
		}
	}
}
