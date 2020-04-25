#include "board.h"
#include "interface.h"

namespace Chess {
	board::board(interface* ifx) {
		fx = ifx;
		fenSet(STARTFEN);
	}

	void board::fenSet(std::string fs) {//sets board to state outlined in FEN string
		cturn = 1;
		for (int i = 0; i < MEMORY; ++i) {
			fHist[i] = 0;
			cHist[i] = 0;
			vHist[i] = 0;
			mHist[i] = NULLMOVE;
			zHist[i] = 0;
		}
		for (int i = PINDEX; i < KINDEX; ++i) {
			roles[BLACK][i] = 0;
			roles[WHITE][i] = 0;
		}
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 5; ++j) {
				pins[i][j] = 0;
			}
		}
		int index = 0, counter = 0, helper;
		while (fs[index] != ' ') {
			switch (fs[index]) {
			case 'P':  
				grid[counter++] = PAWN; 
				++roles[WHITE][PINDEX];
				break; 
			case 'R': 
				grid[counter++] = ROOK;
				++roles[WHITE][RINDEX];
				break;
			case 'N':  
				grid[counter++] = KNIGHT; 
				++roles[WHITE][NINDEX];
				break;
			case 'B':  
				grid[counter++] = BISHOP; 
				++roles[WHITE][BINDEX];
				break;
			case 'Q':  
				grid[counter++] = QUEEN; 
				++roles[WHITE][QINDEX];
				break;
			case 'K':  
				kpos[WHITE] = counter;
				grid[counter++] = KING; 
				break; 
			case 'p':  
				grid[counter++] = -PAWN; 
				++roles[BLACK][PINDEX];
				break;
			case 'r':  
				grid[counter++] = -ROOK; 
				++roles[BLACK][RINDEX];
				break;
			case 'n':  
				grid[counter++] = -KNIGHT; 
				++roles[BLACK][NINDEX];
				break;
			case 'b':
				grid[counter++] = -BISHOP; 
				++roles[BLACK][BINDEX];
				break;
			case 'q':  
				grid[counter++] = -QUEEN; 
				++roles[BLACK][QINDEX];
				break;
			case 'k':  
				kpos[BLACK] = counter;
				grid[counter++] = -KING;
				break; 
			case '/':  
				break; 
			default:
				helper = fs[index] - '0';
				while (helper--) { grid[counter++] = EMPTY; }
			}
			++index;
		}
		turn = fs[++index] == 'w';
		++index;
		cHist[cturn- 1] = 0;
		do {
			switch (fs[index++]) {
			case 'K': {cHist[cturn - 1] |= 1 << 0; cHist[cturn - 1] |= 1 << 2; break; }
			case 'Q': {cHist[cturn - 1] |= 1 << 1; cHist[cturn - 1] |= 1 << 2; break; }
			case 'k': {cHist[cturn - 1] |= 1 << 3; cHist[cturn - 1] |= 1 << 4; break; }
			case 'q': {cHist[cturn - 1] |= 1 << 5; cHist[cturn - 1] |= 1 << 4; break; }
			}
		} while (fs[index] != ' ');
		if (fs[++index] != '-') {
			int from = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
			int to = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			mHist[cturn - 1] = move(from, to, DOUBLEPUSH);
		}
		index += 2;
		fHist[cturn - 1] = fs[index] - '0';
		vHist[cturn - 1] = 0;
		for (int i = 0; i < SPACES; ++i) { vHist[cturn - 1] += grid[i]; }
		zHist[cturn - 1] = z.newKey(this);
		allThreats();
	}

	bool board::isCheckMate() {
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
		if (fHist[cturn - 1] >= 100) { return true; }
		for (int i = 4; i < fHist[cturn - 1]; i += 4) {
			if (zHist[cturn - i - 1] == zHist[cturn - 1]) { return true; }
		}
		return false;
	}

	bool board::insufficientMaterial() {
		if (roles[WHITE][PINDEX] || roles[BLACK][PINDEX] || roles[WHITE][QINDEX] || roles[BLACK][QINDEX]) { return false; }
		int helper;
		for (int i = 0; i < 2; ++i) {
			helper = 0;
			for (int j = NINDEX; j < QINDEX; ++j) { helper += j * roles[i][j]; }
			if (helper > BINDEX) { return false; }
		}
		return true;
	}

	bool board::isEndgame() {
		for (int i = 0; i < 2; ++i) {
			if (roles[i][QINDEX]) {
				for (int j = NINDEX; j < QINDEX; ++j) {
					if (roles[i][j]) { return false; }
				}
				if (roles[i][QINDEX] > 1) { return false; }
			}
		}
		return true;
	}

	void board::movePiece(move m) {//executes a move if legal
		cHist[cturn] = cHist[cturn - 1];
		vHist[cturn] = vHist[cturn - 1];
		mHist[cturn] = m;
		fHist[cturn] = (m.getFlags() == STANDARD && abs(grid[m.getFrom()]) != PAWN) ? fHist[cturn - 1] + 1 : 0;
		zHist[cturn] = (mHist[cturn - 1].getFlags() == DOUBLEPUSH) ? zHist[cturn - 1] ^ z.side ^ z.enpassant[mHist[cturn - 1].getTo()] : zHist[cturn - 1] ^ z.side;
		switch (m.getFlags()) {
		case STANDARD:
			zHist[cturn] ^= z.pieces[abs(grid[m.getFrom()]) % 10][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[abs(grid[m.getFrom()]) % 10][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			if (abs(grid[m.getTo()]) == KING) { kpos[turn] = m.getTo(); }
			break;
		case DOUBLEPUSH:
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getTo()];
			zHist[cturn] ^= z.enpassant[m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			break;
		case KCASTLE:
			zHist[cturn] ^= z.pieces[KINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[KINDEX][turn][m.getTo()];
			zHist[cturn] ^= z.pieces[RINDEX][turn][m.getTo() + 1];
			zHist[cturn] ^= z.pieces[RINDEX][turn][m.getTo() - 1];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() - 1] = grid[m.getTo() + 1];
			grid[m.getTo() + 1] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			kpos[turn] = m.getTo();
			break;
		case QCASTLE:
			zHist[cturn] ^= z.pieces[KINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[KINDEX][turn][m.getTo()];
			zHist[cturn] ^= z.pieces[RINDEX][turn][m.getTo() - 2];
			zHist[cturn] ^= z.pieces[RINDEX][turn][m.getTo() + 1];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() + 1] = grid[m.getTo() - 2];
			grid[m.getTo() - 2] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			kpos[turn] = m.getTo();
			break;
		case ENPASSANT:
			--roles[!turn][PINDEX];
			vHist[cturn] += (turn) ? PAWN : -PAWN;
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getTo()];
			zHist[cturn] ^= (turn) ? z.pieces[PINDEX][BLACK][m.getTo() + SOUTH] : z.pieces[PINDEX][WHITE][m.getTo() + NORTH];
			grid[m.getTo()] = grid[m.getFrom()];
			(turn) ? grid[m.getTo() + SOUTH] = EMPTY : grid[m.getTo() + NORTH] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			break;
		case CAPTURE:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			vHist[cturn] -= grid[m.getTo()];
			zHist[cturn] ^= z.pieces[abs(grid[m.getFrom()]) % 10][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			zHist[cturn] ^= z.pieces[abs(grid[m.getFrom()]) % 10][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			if (abs(grid[m.getTo()]) == KING) { kpos[turn] = m.getTo(); }
			break;
		case NPROMOTE:
			--roles[turn][PINDEX];
			++roles[turn][NINDEX];
			vHist[cturn] += (turn) ? -PAWN + KNIGHT : PAWN - KNIGHT;
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTE:
			--roles[turn][PINDEX];
			++roles[turn][BINDEX];
			vHist[cturn] += (turn) ? -PAWN + BISHOP : PAWN - BISHOP;
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTE:
			--roles[turn][PINDEX];
			++roles[turn][RINDEX];
			vHist[cturn] += (turn) ? -PAWN + ROOK : PAWN - ROOK;
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTE:
			--roles[turn][PINDEX];
			++roles[turn][QINDEX];
			vHist[cturn] += (turn) ? -PAWN + QUEEN : PAWN - QUEEN;
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		case NPROMOTEC:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			--roles[turn][PINDEX];
			++roles[turn][NINDEX];
			vHist[cturn] += (turn) ? -PAWN + KNIGHT - grid[m.getTo()] : PAWN - KNIGHT - grid[m.getTo()];
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			zHist[cturn] ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			--roles[turn][PINDEX];
			++roles[turn][BINDEX];
			vHist[cturn] += (turn) ? -PAWN + BISHOP - grid[m.getTo()] : PAWN - BISHOP - grid[m.getTo()];
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			zHist[cturn] ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			--roles[turn][PINDEX];
			++roles[turn][RINDEX];
			vHist[cturn] += (turn) ? -PAWN + ROOK - grid[m.getTo()] : PAWN - ROOK - grid[m.getTo()];
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			zHist[cturn] ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			--roles[turn][PINDEX];
			++roles[turn][QINDEX];
			vHist[cturn] += (turn) ? -PAWN + QUEEN - grid[m.getTo()] : +PAWN - QUEEN - grid[m.getTo()];
			zHist[cturn] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn] ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			zHist[cturn] ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		}
		if (cHist[cturn] & 1 << 2) {
			if (m.getFrom() == 60) {
				cHist[cturn] &= ~(1 << 2);
				if (cHist[cturn] & 1 << 0) { cHist[cturn] &= ~(1 << 0); zHist[cturn] ^= z.castle[WHITE][0]; }
				if (cHist[cturn] & 1 << 1) { cHist[cturn] &= ~(1 << 1); zHist[cturn] ^= z.castle[WHITE][1]; }
			}
			else if (cHist[cturn] & 1 << 0 && (m.getTo() == 63 || m.getFrom() == 63)) { cHist[cturn] &= ~(1 << 0); zHist[cturn] ^= z.castle[WHITE][0]; }
			else if (cHist[cturn] & 1 << 1 && (m.getTo() == 56 || m.getFrom() == 56)) { cHist[cturn] &= ~(1 << 1); zHist[cturn] ^= z.castle[WHITE][1]; }
		}
		if (cHist[cturn] & 1 << 4) {
			if (m.getFrom() == 4) {
				cHist[cturn] &= ~(1 << 4);
				if (cHist[cturn] & 1 << 3) { cHist[cturn] &= ~(1 << 3); zHist[cturn] ^= z.castle[BLACK][0]; }
				if (cHist[cturn] & 1 << 5) { cHist[cturn] &= ~(1 << 5); zHist[cturn] ^= z.castle[BLACK][1]; }
			}
			else if ((cHist[cturn] & 1 << 3) && (m.getTo() == 7 || m.getFrom() == 7)) { cHist[cturn] &= ~(1 << 3); zHist[cturn] ^= z.castle[BLACK][0]; }
			else if ((cHist[cturn] & 1 << 5) && (m.getTo() == 0 || m.getFrom() == 0)) { cHist[cturn] &= ~(1 << 5); zHist[cturn] ^= z.castle[BLACK][1]; }
		}
		++cturn;
		turn = !turn;
		allThreats();
	}

	void board::unmovePiece() {//unmakes a move
		turn = !turn;
		--cturn;
		switch (mHist[cturn].getFlags()) {
		case STANDARD:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			if (abs(grid[mHist[cturn].getFrom()]) == KING) { kpos[turn] = mHist[cturn].getFrom(); }
			break;
		case DOUBLEPUSH:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			break;
		case KCASTLE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			grid[mHist[cturn].getTo() + 1] = grid[mHist[cturn].getTo() - 1];
			grid[mHist[cturn].getTo() - 1] = EMPTY;
			kpos[turn] = mHist[cturn].getFrom();
			break;
		case QCASTLE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			grid[mHist[cturn].getTo() - 2] = grid[mHist[cturn].getTo() + 1];
			grid[mHist[cturn].getTo() + 1] = EMPTY;
			kpos[turn] = mHist[cturn].getFrom();
			break;
		case CAPTURE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = vHist[cturn - 1] - vHist[cturn];
			if (abs(grid[mHist[cturn].getFrom()]) == KING) { kpos[turn] = mHist[cturn].getFrom(); }
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			break;
		case ENPASSANT:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			grid[mHist[cturn - 1].getTo()] = (turn) ? -PAWN : PAWN;
			++roles[!turn][PINDEX];
			break;
		case NPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			++roles[turn][PINDEX];
			--roles[turn][NINDEX];
			break;
		case BPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			++roles[turn][PINDEX];
			--roles[turn][BINDEX];
			break;
		case RPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			++roles[turn][PINDEX];
			--roles[turn][RINDEX];
			break;
		case QPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			++roles[turn][PINDEX];
			--roles[turn][QINDEX];
			break;
		case NPROMOTEC:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + KNIGHT - PAWN : vHist[cturn - 1] - vHist[cturn] - KNIGHT + PAWN;
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			++roles[turn][PINDEX];
			--roles[turn][NINDEX];
			break;
		case BPROMOTEC:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + BISHOP - PAWN : vHist[cturn - 1] - vHist[cturn] - BISHOP + PAWN;
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			++roles[turn][PINDEX];
			--roles[turn][BINDEX];
			break;
		case RPROMOTEC:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + ROOK - PAWN : vHist[cturn - 1] - vHist[cturn] - ROOK + PAWN;
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			++roles[turn][PINDEX];
			--roles[turn][RINDEX];
			break;
		case QPROMOTEC:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + QUEEN - PAWN : vHist[cturn - 1] - vHist[cturn] - QUEEN + PAWN;
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			++roles[turn][PINDEX];
			--roles[turn][QINDEX];
			break;
		}
		allThreats();
	}

	void board::allThreats() {
		cpins = 0;
		for (int i = 0; i < SPACES; ++i) {
			threatened[BLACK][i] = 0;
			threatened[WHITE][i] = 0;
		}
		for (int from = 0; from < SPACES; ++from) {
			if (grid[from]) { pieceThreats(from); }
		}
	}

	void board::pieceThreats(int from) {//generates all pseudo legal moves for one piec
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
		case ROOK:
			for (i = from + NORTH; i >= 0; i += NORTH) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + NORTH; j != kpos[turn]; j += NORTH) {
							if (grid[j]) { goto failN; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = NORTH;
					}
					break;
				}
			}
		failN:
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + SOUTH; j != kpos[turn]; j += SOUTH) {
							if (grid[j]) { goto failS; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = SOUTH;
					}
					break;
				}
			}
		failS:
			for (i = from + EAST; i % WIDTH; i += EAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + EAST; j != kpos[turn]; j += EAST) {
							if (grid[j]) { goto failE; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = EAST;
					}
					break;
				}
			}
		failE:
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + WEST; j != kpos[turn]; j += WEST) {
							if (grid[j]) { goto failW; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = WEST;
					}
					break;
				}
			}
		failW:
			if (abs(grid[from]) != QUEEN) { break; }
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + NORTHEAST; j != kpos[turn]; j += NORTHEAST) {
							if (grid[j]) { goto failNE; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = NORTHEAST;
					}
					break;
				}
			}
		failNE:
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + NORTHWEST; j != kpos[turn]; j += NORTHWEST) {
							if (grid[j]) { goto failNW; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = NORTHWEST;
					}
					break;
				}
			}
		failNW:
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + SOUTHEAST; j != kpos[turn]; j += SOUTHEAST) {
							if (grid[j]) { goto failSE; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = SOUTHEAST;
					}
					break;
				}
			}
		failSE:
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + SOUTHWEST; j != kpos[turn]; j += SOUTHWEST) {
							if (grid[j]) { goto failSW; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = SOUTHWEST;
					}
					break;
				}
			}
		failSW:
			break;
		}
	}
}
