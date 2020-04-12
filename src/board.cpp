#include "board.h"

namespace Chess {

	board::board() {//sets board to starting state
		z = zobrist();
		fenSet(STARTFEN);
	}

	void board::fenSet(std::string fs) {//sets board to state outlined in FEN string, no 50 move rule implementation
		for (int i = 0; i < MEMORY; ++i) { mHist[i] = move(); vHist[i] = 0; zHist[i] = 0; cHist[i] = 0; }
		cturn = 0;
		int index = 0;
		int counter = 0;
		int helper;
		while (fs[index] != ' ') {
			switch (fs[index]) {
			case 'P': { grid[counter] = PAWN; ++counter; break; }
			case 'R': { grid[counter] = ROOK; ++counter; break; }
			case 'N': { grid[counter] = KNIGHT; ++counter; break; }
			case 'B': { grid[counter] = BISHOP; ++counter; break; }
			case 'Q': { grid[counter] = QUEEN; ++counter; break; }
			case 'K': { grid[counter] = KING; kpos[WHITE] = counter; ++counter; break; }
			case 'p': { grid[counter] = -PAWN; ++counter; break; }
			case 'r': { grid[counter] = -ROOK; ++counter; break; }
			case 'n': { grid[counter] = -KNIGHT; ++counter; break; }
			case 'b': { grid[counter] = -BISHOP; ++counter; break; }
			case 'q': { grid[counter] = -QUEEN; ++counter; break; }
			case 'k': { grid[counter] = -KING; kpos[BLACK] = counter; ++counter; break; }
			case '/': { break; }
			default:
				helper = fs[index] - '0';
				while (helper) {
					grid[counter] = EMPTY;
					++counter;
					--helper;
				}
			}
			++index;
		}
		++index;
		turn = (fs[index] == 'w') ? WHITE : BLACK;
		++index;
		currC = 0;
		do {
			switch (fs[index]) {
			case 'K': {currC |= 1 << 0; currC |= 1 << 2; break; }
			case 'Q': {currC |= 1 << 1; currC |= 1 << 2; break; }
			case 'k': {currC |= 1 << 3; currC |= 1 << 4; break; }
			case 'q': {currC |= 1 << 5; currC |= 1 << 4; break; }
			}
			++index;
		} while (fs[index] != ' ');
		++index;
		if (fs[index] != '-') {
			int from = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
			int to = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			mHist[cturn] = move(from, to, DOUBLEPUSH);
			++cturn;
		}
		++index;
		currV = 0;
		for (int i = 0; i < SPACES; ++i) { currV += grid[i]; }
		currZ = z.newKey(this);
		currM = (cturn) ? mHist[cturn - 1] : move();
		allThreats();
	}

	bool board::checkMate() {
		move m[MEMORY];
		int cmove = genAll(m);
		cmove = removeIllegal(m, cmove);
		if (!cmove) { return true; }
		return false;
	}

	void board::drawBoard() {//prints board in cmd
		char letter;
		std::cout << "\n  a   b   c   d   e   f   g   h";
		for (int i = 0; i < WIDTH; ++i) {
			std::cout << "\n---------------------------------\n|";
			for (int j = 0; j < WIDTH; ++j) {
				switch (grid[i * WIDTH + j]) {
				case PAWN: { letter = 'P'; break; }
				case ROOK: { letter = 'R'; break; }
				case KNIGHT: { letter = 'N'; break; }
				case BISHOP: { letter = 'B'; break; }
				case QUEEN: { letter = 'Q'; break; }
				case KING: { letter = 'K'; break; }
				case -PAWN: { letter = 'p'; break; }
				case -ROOK: { letter = 'r'; break; }
				case -KNIGHT: { letter = 'n'; break; }
				case -BISHOP: { letter = 'b'; break; }
				case -QUEEN: { letter = 'q'; break; }
				case -KING: { letter = 'k'; break; }
				default: { letter = ' '; }
				}
				std::cout << " " << letter << " |";
			}
			std::cout << " " << WIDTH - i;
		}
		std::cout << "\n---------------------------------\n";
	}

	void board::movePiece(move m) {//executes a move if legal, return value depicts success (nullmoves considered legal)
		zHist[cturn] = currZ;
		cHist[cturn] = currC;
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
			break;
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
			currV += (turn) ? PAWN : - PAWN;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[PINDEX][!turn][currM.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[currM.getTo()] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			break;
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
			currV += (turn) ? - PAWN + KNIGHT : PAWN - KNIGHT;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTE:
			currV += (turn) ? - PAWN + BISHOP : PAWN - BISHOP;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTE:
			currV += (turn) ? - PAWN + ROOK : PAWN - ROOK;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTE:
			currV += (turn) ? - PAWN + QUEEN : PAWN - QUEEN;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		case NPROMOTEC:
			currV += (turn) ? - PAWN + KNIGHT - grid[m.getTo()] : PAWN - KNIGHT - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			currV += (turn) ? - PAWN + BISHOP - grid[m.getTo()] : PAWN - BISHOP - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			currV += (turn) ? - PAWN + ROOK - grid[m.getTo()] : PAWN - ROOK - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			currV += (turn) ? - PAWN + QUEEN - grid[m.getTo()] : + PAWN - QUEEN - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		}
		if (currM.getFlags() == DOUBLEPUSH) { currZ ^= z.enpassant[currM.getTo()]; }
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
		currZ ^= z.side;
		vHist[cturn] = currV;
		mHist[cturn] = m;
		currM = m;
		++cturn;
		turn = (turn) ? BLACK : WHITE;
		allThreats();
	}

	void board::unmovePiece() {//unmakes a move
		turn = (turn) ? BLACK : WHITE;
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
		currZ = zHist[cturn];
		currC = cHist[cturn];
		currV = vHist[cturn - 1];
		currM = mHist[cturn - 1];
		allThreats();
	}

	move board::createMove(int from, int to) {
		move m[MEMORY];
		int cmove = genAll(m);
		cmove = removeIllegal(m, cmove);
		for (int i = 0; i < cmove; ++i) {
			if (m[i].getFrom() == from && m[i].getTo() == to) { return m[i]; }
		}
		return move();
	}

	void board::allThreats() {
		cpins = 0;
		endgame = true;
		cpos[0] = NOCHECK;
		cpos[1] = NOCHECK;
		for (int i = 0; i < SPACES; ++i) {
			threatened[WHITE][i] = 0;
			threatened[BLACK][i] = 0;
		}
		if (turn) {
			for (int from = 0; from < SPACES; ++from) {
				if (grid[from] < 0) { pieceThreats(from); }
			}
		}
		else {
			for (int from = 0; from < SPACES; ++from) {
				if (grid[from] > 0) { pieceThreats(from); }
			}
		}
	}

	int board::removeIllegal(move* m, int cmove) {
		bool undo = false;
		for (int i = 0; i < cpins; ++i) {
			for (int j = 0; j < cmove; ++j) {
				if (m[j].getFrom() == pins[i][0]) {
					switch (pins[i][1]) {
					case SOUTH:
					case NORTH:
						if (!(NSslide(m[j].getFrom(), m[j].getTo()))) { undo = true; }
						break;
					case WEST:
					case EAST:
						if (!(EWslide(m[j].getFrom(), m[j].getTo()))) { undo = true; }
						break;
					case NORTHEAST:
					case SOUTHWEST:
						if (!(NESWslide(m[j].getFrom(), m[j].getTo()))) { undo = true; }
						break;
					case NORTHWEST:
					case SOUTHEAST:
						if (!(NWSEslide(m[j].getFrom(), m[j].getTo()))) { undo = true; }
						break;
					}
					if (undo) {
						undo = false;
						--cmove;
						m[j] = m[cmove];
						--j;
					}
				}
			}
		}
		for (int i = 0; i < 2 && cpos[i] != NOCHECK; ++i) {
			int checktype;
			if (abs(grid[cpos[i]]) <= KNIGHT) { checktype = LEAP; }
			else if (NSslide(cpos[i], kpos[turn])) { checktype = (cpos[i] > kpos[turn]) ? NORTH : SOUTH; }
			else if (EWslide(cpos[i], kpos[turn])) { checktype = (cpos[i] > kpos[turn]) ? WEST : EAST; }
			else if (NESWslide(cpos[i], kpos[turn])) { checktype = (cpos[i] > kpos[turn]) ? NORTHEAST : SOUTHWEST; }
			else { checktype = (cpos[i] > kpos[turn]) ? NORTHWEST : SOUTHEAST; }
			for (int j = 0; j < cmove; ++j) {
				int to = m[j].getTo();
				int from = m[j].getFrom();
				if (from != kpos[turn]) {
					if (checktype != LEAP && ((to - cpos[i]) % checktype || (to < cpos[i] && to < kpos[turn]) || (to > cpos[i] && to > kpos[turn]))) {
						--cmove;
						m[j] = m[cmove];
						--j;

					}
					else if (checktype == LEAP && to != cpos[i] && m[j].getFlags() != ENPASSANT) {
						--cmove;
						m[j] = m[cmove];
						--j;
					}
				}
				else if (checktype != LEAP && to - from == checktype) {
					--cmove;
					m[j] = m[cmove];
					--j;
				}
			}
		}
		return cmove;
	}

	bool board::validateMove(move m) {//validates a single move
		if (m == move()) { return false; }
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

	int board::genAllNonCaps(move * m) {
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

	int board::pieceCapMoves(move* m, int from) {//generates all pseudo legal moves for one piece
		int i;
		int cmove = 0;
		switch (abs(grid[from])) {
		case KING:
			if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && !threatened[!turn][from + SOUTHEAST] && ((turn && grid[from + SOUTHEAST] < 0) || (!turn && grid[from + SOUTHEAST] > 0))) {
				m[cmove] = move(from, from + SOUTHEAST, CAPTURE);
				++cmove;
			}
			if ((from + EAST) % WIDTH > from % WIDTH && !threatened[!turn][from + EAST] && ((turn && grid[from + EAST] < 0) || (!turn && grid[from + EAST] > 0))) {
				m[cmove] = move(from, from + EAST, CAPTURE);
				++cmove;
			}
			if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8 && !threatened[!turn][from + NORTHWEST] && ((turn && grid[from + NORTHWEST] < 0) || (!turn && grid[from + NORTHWEST] > 0))) {
				m[cmove] = move(from, from + NORTHWEST, CAPTURE);
				++cmove;
			}
			if ((from + WEST) % WIDTH < from % WIDTH && from > 0 && !threatened[!turn][from + WEST] && ((turn && grid[from + WEST] < 0) || (!turn && grid[from + WEST] > 0))) {
				m[cmove] = move(from, from + WEST, CAPTURE);
				++cmove;
			}
			if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && !threatened[!turn][from + SOUTHWEST] && ((turn && grid[from + SOUTHWEST] < 0) || (!turn && grid[from + SOUTHWEST] > 0))) {
				m[cmove] = move(from, from + SOUTHWEST, CAPTURE);
				++cmove;
			}
			if (from < 56 && !threatened[!turn][from + SOUTH] && ((turn && grid[from + SOUTH] < 0) || (!turn && grid[from + SOUTH] > 0))) {
				m[cmove] = move(from, from + SOUTH, CAPTURE);
				++cmove;
			}
			if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6 && !threatened[!turn][from + NORTHEAST] && ((turn && grid[from + NORTHEAST] < 0) || (!turn && grid[from + NORTHEAST] > 0))) {
				m[cmove] = move(from, from + NORTHEAST, CAPTURE);
				++cmove;
			}
			if (from > 7 && !threatened[!turn][from + NORTH] && ((turn && grid[from + NORTH] < 0) || (!turn && grid[from + NORTH] > 0))) {
				m[cmove] = move(from, from + NORTH, CAPTURE);
				++cmove;
			}
			return cmove;
		case PAWN:
			i = (turn) ? NORTH : SOUTH;
			if (from % WIDTH && (turn && grid[from + i + WEST] < 0) || (!turn && grid[from + i + WEST] > 0)) {
				if ((turn && from > 15) || (!turn && from < 48)) {
					m[cmove] = move(from, from + i + WEST, CAPTURE);
					++cmove;
				}
				else {
					m[cmove] = move(from, from + i + WEST, NPROMOTEC);
					++cmove;
					m[cmove] = move(from, from + i + WEST, BPROMOTEC);
					++cmove;
					m[cmove] = move(from, from + i + WEST, RPROMOTEC);
					++cmove;
					m[cmove] = move(from, from + i + WEST, QPROMOTEC);
					++cmove;
				}
			}
			if (from % WIDTH != 7 && (turn && grid[from + i + EAST] < 0) || (!turn && grid[from + i + EAST] > 0)) {
				if ((turn && from > 15) || (!turn && from < 48)) {
					m[cmove] = move(from, from + i + EAST, CAPTURE);
					++cmove;
				}
				else {
					m[cmove] = move(from, from + i + EAST, NPROMOTEC);
					++cmove;
					m[cmove] = move(from, from + i + EAST, BPROMOTEC);
					++cmove;
					m[cmove] = move(from, from + i + EAST, RPROMOTEC);
					++cmove;
					m[cmove] = move(from, from + i + EAST, QPROMOTEC);
					++cmove;
				}
			}
			if (currM.getFlags() == DOUBLEPUSH && ((currM.getTo() == from + EAST && from % WIDTH != 7) || (currM.getTo() == from + WEST && from % WIDTH))) {
				m[cmove] = move(from, currM.getTo() + i, ENPASSANT);
				++cmove;
			}
			return cmove;
		case KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && ((turn && grid[from + 10] < 0) || (!turn && grid[from + 10] > 0))) {
				m[cmove] = move(from, from + 10, CAPTURE);
				++cmove;
			}
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && ((turn && grid[from + 17] < 0) || (!turn && grid[from + 17] > 0))) {
				m[cmove] = move(from, from + 17, CAPTURE);
				++cmove;
			}
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && ((turn && grid[from - 10] < 0) || (!turn && grid[from - 10] > 0))) {
				m[cmove] = move(from, from - 10, CAPTURE);
				++cmove;
			}
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && ((turn && grid[from - 17] < 0) || (!turn && grid[from - 17] > 0))) {
				m[cmove] = move(from, from - 17, CAPTURE);
				++cmove;
			}
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && ((turn && grid[from + 6] < 0) || (!turn && grid[from + 6] > 0))) {
				m[cmove] = move(from, from + 6, CAPTURE);
				++cmove;
			}
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && (turn && (grid[from + 15] < 0) || (!turn && grid[from + 15] > 0))) {
				m[cmove] = move(from, from + 15, CAPTURE);
				++cmove;
			}
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && ((turn && grid[from - 6] < 0) || (!turn && grid[from - 6] > 0))) {
				m[cmove] = move(from, from - 6, CAPTURE);
				++cmove;
			}
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && ((turn && grid[from - 15] < 0) || (!turn && grid[from - 15] > 0))) {
				m[cmove] = move(from, from - 15, CAPTURE);
				++cmove;
			}
			return cmove;
		case QUEEN:
		case ROOK:
			for (i = from + NORTH; i >= 0; i += NORTH) {
				if(grid[i]){
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				if(grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			if (abs(grid[from]) != QUEEN) { return cmove; }
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				if (grid[i]) {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
		}
		return cmove;
	}

	int board::pieceNonCapMoves(move* m, int from) {//generates all pseudo legal moves for one piece
		int i;
		int cmove = 0;
		switch (abs(grid[from])) {
		case KING:
			if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55 && !threatened[!turn][from + SOUTHEAST] && !grid[from + SOUTHEAST]) {
				m[cmove] = move(from, from + SOUTHEAST, STANDARD);
				++cmove;
			}
			if ((from + EAST) % WIDTH > from % WIDTH && !threatened[!turn][from + EAST] && !grid[from + EAST]) {
				m[cmove] = move(from, from + EAST, STANDARD);
				++cmove;
			}
			if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8 && !threatened[!turn][from + NORTHWEST] && !grid[from + NORTHWEST]) {
				m[cmove] = move(from, from + NORTHWEST, STANDARD);
				++cmove;
			}
			if ((from + WEST) % WIDTH < from % WIDTH && from > 0 && !threatened[!turn][from + WEST] && !grid[from + WEST]) {
				m[cmove] = move(from, from + WEST, STANDARD);
				++cmove;
			}
			if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57 && !threatened[!turn][from + SOUTHWEST] && !grid[from + SOUTHWEST]) {
				m[cmove] = move(from, from + SOUTHWEST, STANDARD);
				++cmove;
			}
			if (from < 56 && !threatened[!turn][from + SOUTH] && !grid[from + SOUTH]) {
				m[cmove] = move(from, from + SOUTH, STANDARD);
				++cmove;
			}
			if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6 && !threatened[!turn][from + NORTHEAST] && !grid[from + NORTHEAST]) {
				m[cmove] = move(from, from + NORTHEAST, STANDARD);
				++cmove;
			}
			if (from > 7 && !threatened[!turn][from + NORTH] && !grid[from + NORTH]) {
				m[cmove] = move(from, from + NORTH, STANDARD);
				++cmove;
			}
			if (turn) {
				if (from == 60 && !threatened[BLACK][60] && (currC & 1 << 2)) {
					if (!grid[61] && !grid[62] && !threatened[BLACK][61] && !threatened[BLACK][62] && (currC & 1 << 0)) {
						m[cmove] = move(60, 62, KCASTLE);
						++cmove;
					}
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[BLACK][59] && !threatened[BLACK][58] && (currC & 1 << 1)) {
						m[cmove] = move(60, 58, QCASTLE);
						++cmove;
					}
				}
			}
			else {
				if (from == 4 && !threatened[WHITE][4] && (currC & 1 << 4)) {
					if (!grid[5] && !grid[6] && !threatened[WHITE][5] && !threatened[WHITE][6] && (currC & 1 << 3)) {
						m[cmove] = move(4, 6, KCASTLE);
						++cmove;
					}
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[WHITE][3] && !threatened[WHITE][2] && (currC & 1 << 5)) {
						m[cmove] = move(4, 2, QCASTLE);
						++cmove;
					}
				}
			}
			return cmove;
		case PAWN:
			i = (turn) ? NORTH : SOUTH;
			if (!grid[from + i]) {
				if ((turn && from > 15) || (!turn && from < 48)) {
					m[cmove] = move(from, from + i, STANDARD);
					++cmove;
					if (((turn && from > 47) || (!turn && from < 16)) && !grid[from + 2 * i]) {
						m[cmove] = move(from, from + 2 * i, DOUBLEPUSH);
						++cmove;
						return cmove;
					}
				}
				else {
					m[cmove] = move(from, from + i, NPROMOTE);
					++cmove;
					m[cmove] = move(from, from + i, BPROMOTE);
					++cmove;
					m[cmove] = move(from, from + i, RPROMOTE);
					++cmove;
					m[cmove] = move(from, from + i, QPROMOTE);
					++cmove;
					return cmove;
				}
			}
			return cmove;
		case KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54 && !grid[from + 10]) {
				m[cmove] = move(from, from + 10, STANDARD);
				++cmove;
			}
			if ((from + 17) % WIDTH > from % WIDTH && from < 47 && !grid[from + 17]) {
				m[cmove] = move(from, from + 17, STANDARD);
				++cmove;
			}
			if ((from - 10) % WIDTH < from % WIDTH && from > 9 && !grid[from - 10]) {
				m[cmove] = move(from, from - 10, STANDARD);
				++cmove;
			}
			if ((from - 17) % WIDTH < from % WIDTH && from > 16 && !grid[from - 17]) {
				m[cmove] = move(from, from - 17, STANDARD);
				++cmove;
			}
			if ((from + 6) % WIDTH < from % WIDTH && from < 58 && !grid[from + 6]) {
				m[cmove] = move(from, from + 6, STANDARD);
				++cmove;
			}
			if ((from + 15) % WIDTH < from % WIDTH && from < 49 && !grid[from + 15]) {
				m[cmove] = move(from, from + 15, STANDARD);
				++cmove;
			}
			if ((from - 6) % WIDTH > from % WIDTH && from > 5 && !grid[from - 6]) {
				m[cmove] = move(from, from - 6, STANDARD);
				++cmove;
			}
			if ((from - 15) % WIDTH > from % WIDTH && from > 14 && !grid[from - 15]) {
				m[cmove] = move(from, from - 15, STANDARD);
				++cmove;
			}
			return cmove;
		case QUEEN:
		case ROOK:
			for (i = from + NORTH; i >= 0; i += NORTH) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else { break; }
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else { break; }
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else { break; }
			}
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else { break; }
			}
			if (abs(grid[from]) != QUEEN) { return cmove; }
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else { break; }
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else { break; }
			}
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else { break; }
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else { break; }
			}
		}
		return cmove;
	}

	int board::pieceMoves(move* m, int from) {//generates all pseudo legal moves for one piece
		int i;
		int cmove = 0;
		switch (abs(grid[from])) {
		case KING:
			if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) {
				if (!threatened[!turn][from + SOUTHEAST] && ((turn && grid[from + SOUTHEAST] <= 0) || (!turn && grid[from + SOUTHEAST] >= 0))) {
					if (!grid[from + SOUTHEAST]) { m[cmove] = move(from, from + SOUTHEAST, STANDARD); }
					else { m[cmove] = move(from, from + SOUTHEAST, CAPTURE); }
					++cmove;
				}
			}
			if ((from + EAST) % WIDTH > from % WIDTH) {
				if (!threatened[!turn][from + EAST] && ((turn && grid[from + EAST] <= 0) || (!turn && grid[from + EAST] >= 0))) {
					if (!grid[from + EAST]) { m[cmove] = move(from, from + EAST, STANDARD); }
					else { m[cmove] = move(from, from + EAST, CAPTURE); }
					++cmove;
				}
			}
			if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) {
				if (!threatened[!turn][from + NORTHWEST] && ((turn && grid[from + NORTHWEST] <= 0) || (!turn && grid[from + NORTHWEST] >= 0))) {
					if (!grid[from + NORTHWEST]) { m[cmove] = move(from, from + NORTHWEST, STANDARD); }
					else { m[cmove] = move(from, from + NORTHWEST, CAPTURE); }
					++cmove;
				}
			}
			if ((from + WEST) % WIDTH < from % WIDTH && from > 0) {
				if (!threatened[!turn][from + WEST] && ((turn && grid[from + WEST] <= 0) || (!turn && grid[from + WEST] >= 0))) {
					if (!grid[from + WEST]) { m[cmove] = move(from, from + WEST, STANDARD); }
					else { m[cmove] = move(from, from + WEST, CAPTURE); }
					++cmove;
				}
			}
			if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) {
				if (!threatened[!turn][from + SOUTHWEST] && ((turn && grid[from + SOUTHWEST] <= 0) || (!turn && grid[from + SOUTHWEST] >= 0))) {
					if (!grid[from + SOUTHWEST]) { m[cmove] = move(from, from + SOUTHWEST, STANDARD); }
					else { m[cmove] = move(from, from + SOUTHWEST, CAPTURE); }
					++cmove;
				}
			}
			if (from < 56) {
				if (!threatened[!turn][from + SOUTH] && ((turn && grid[from + SOUTH] <= 0) || (!turn && grid[from + SOUTH] >= 0))) {
					if (!grid[from + SOUTH]) { m[cmove] = move(from, from + SOUTH, STANDARD); }
					else { m[cmove] = move(from, from + SOUTH, CAPTURE); }
					++cmove;
				}
			}
			if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) {
				if (!threatened[!turn][from + NORTHEAST] && ((turn && grid[from + NORTHEAST] <= 0) || (!turn && grid[from + NORTHEAST] >= 0))) {
					if (!grid[from + NORTHEAST]) { m[cmove] = move(from, from + NORTHEAST, STANDARD); }
					else { m[cmove] = move(from, from + NORTHEAST, CAPTURE); }
					++cmove;
				}
			}
			if (from > 7) {
				if (!threatened[!turn][from + NORTH] && ((turn && grid[from + NORTH] <= 0) || (!turn && grid[from + NORTH] >= 0))) {
					if (!grid[from + NORTH]) { m[cmove] = move(from, from + NORTH, STANDARD); }
					else { m[cmove] = move(from, from + NORTH, CAPTURE); }
					++cmove;
				}
			}
			if (turn) {
				if (from == 60 && !threatened[BLACK][60] && (currC & 1 << 2)) {
					if (!grid[61] && !grid[62] && !threatened[BLACK][61] && !threatened[BLACK][62] && (currC & 1 << 0)) {
						m[cmove] = move(60, 62, KCASTLE);
						++cmove;
					}
					if (!grid[59] && !grid[58] && !grid[57] && !threatened[BLACK][59] && !threatened[BLACK][58] && (currC & 1 << 1)) {
						m[cmove] = move(60, 58, QCASTLE);
						++cmove;
					}
				}
			}
			else {
				if (from == 4 && !threatened[WHITE][4] && (currC & 1 << 4)) {
					if (!grid[5] && !grid[6] && !threatened[WHITE][5] && !threatened[WHITE][6] && (currC & 1 << 3)) {
						m[cmove] = move(4, 6, KCASTLE);
						++cmove;
					}
					if (!grid[3] && !grid[2] && !grid[1] && !threatened[WHITE][3] && !threatened[WHITE][2] && (currC & 1 << 5)) {
						m[cmove] = move(4, 2, QCASTLE);
						++cmove;
					}
				}
			}
			return cmove;
		case PAWN:
			i = (turn) ? NORTH : SOUTH;
			if (from % WIDTH) {
				if ((turn && grid[from + i + WEST] < 0) || (!turn && grid[from + i + WEST] > 0)) {
					if ((turn && from > 15) || (!turn && from < 48)) {
						m[cmove] = move(from, from + i + WEST, CAPTURE);
						++cmove;
					}
					else {
						m[cmove] = move(from, from + i + WEST, NPROMOTEC);
						++cmove;
						m[cmove] = move(from, from + i + WEST, BPROMOTEC);
						++cmove;
						m[cmove] = move(from, from + i + WEST, RPROMOTEC);
						++cmove;
						m[cmove] = move(from, from + i + WEST, QPROMOTEC);
						++cmove;
					}
				}
			}
			if (from % WIDTH != 7) {
				if ((turn && grid[from + i + EAST] < 0) || (!turn && grid[from + i + EAST] > 0)) {
					if ((turn && from > 15) || (!turn && from < 48)) {
						m[cmove] = move(from, from + i + EAST, CAPTURE);
						++cmove;
					}
					else {
						m[cmove] = move(from, from + i + EAST, NPROMOTEC);
						++cmove;
						m[cmove] = move(from, from + i + EAST, BPROMOTEC);
						++cmove;
						m[cmove] = move(from, from + i + EAST, RPROMOTEC);
						++cmove;
						m[cmove] = move(from, from + i + EAST, QPROMOTEC);
						++cmove;
					}
				}
			}
			if (!grid[from + i]) {
				if ((turn && from > 15) || (!turn && from < 48)) {
					m[cmove] = move(from, from + i, STANDARD);
					++cmove;
					if (((turn && from > 47) || (!turn && from < 16)) && !grid[from + 2 * i]) {
						m[cmove] = move(from, from + 2 * i, DOUBLEPUSH);
						++cmove;
						return cmove;
					}
				}
				else {
					m[cmove] = move(from, from + i, NPROMOTE);
					++cmove;
					m[cmove] = move(from, from + i, BPROMOTE);
					++cmove;
					m[cmove] = move(from, from + i, RPROMOTE);
					++cmove;
					m[cmove] = move(from, from + i, QPROMOTE);
					++cmove;
					return cmove;
				}
			}
			if (currM.getFlags() == DOUBLEPUSH && ((currM.getTo() == from + EAST && from % WIDTH != 7) || (currM.getTo() == from + WEST && from % WIDTH))) {
				m[cmove] = move(from, currM.getTo() + i, ENPASSANT);
				++cmove;
			}
			return cmove;
		case KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54) {
				if (!grid[from + 10]) {
					m[cmove] = move(from, from + 10, STANDARD);
					++cmove;
				}
				else if ((turn && grid[from + 10] < 0) || (!turn && grid[from + 10] > 0)) {
					m[cmove] = move(from, from + 10, CAPTURE);
					++cmove;
				}
			}
			if ((from + 17) % WIDTH > from % WIDTH && from < 47) {
				if (!grid[from + 17]) {
					m[cmove] = move(from, from + 17, STANDARD);
					++cmove;
				}
				else if ((turn && grid[from + 17] < 0) || (!turn && grid[from + 17] > 0)) {
					m[cmove] = move(from, from + 17, CAPTURE);
					++cmove;
				}
			}
			if ((from - 10) % WIDTH < from % WIDTH && from > 9) {
				if (!grid[from - 10]) {
					m[cmove] = move(from, from - 10, STANDARD);
					++cmove;
				}
				else if ((turn && grid[from - 10] < 0) || (!turn && grid[from - 10] > 0)) {
					m[cmove] = move(from, from - 10, CAPTURE);
					++cmove;
				}
			}
			if ((from - 17) % WIDTH < from % WIDTH && from > 16) {
				if (!grid[from - 17]) {
					m[cmove] = move(from, from - 17, STANDARD);
					++cmove;
				}
				else if ((turn && grid[from - 17] < 0) || (!turn && grid[from - 17] > 0)) {
					m[cmove] = move(from, from - 17, CAPTURE);
					++cmove;
				}
			}
			if ((from + 6) % WIDTH < from % WIDTH && from < 58) {
				if (!grid[from + 6]) {
					m[cmove] = move(from, from + 6, STANDARD);
					++cmove;
				}
				else if ((turn && grid[from + 6] < 0) || (!turn && grid[from + 6] > 0)) {
					m[cmove] = move(from, from + 6, CAPTURE);
					++cmove;
				}
			}
			if ((from + 15) % WIDTH < from % WIDTH && from < 49) {
				if (!grid[from + 15]) {
					m[cmove] = move(from, from + 15, STANDARD);
					++cmove;
				}
				else if (turn && (grid[from + 15] < 0) || (!turn && grid[from + 15] > 0)) {
					m[cmove] = move(from, from + 15, CAPTURE);
					++cmove;
				}
			}
			if ((from - 6) % WIDTH > from % WIDTH && from > 5) {
				if (!grid[from - 6]) {
					m[cmove] = move(from, from - 6, STANDARD);
					++cmove;
				}
				else if ((turn && grid[from - 6] < 0) || (!turn && grid[from - 6] > 0)) {
					m[cmove] = move(from, from - 6, CAPTURE);
					++cmove;
				}
			}
			if ((from - 15) % WIDTH > from % WIDTH && from > 14) {
				if (!grid[from - 15]) {
					m[cmove] = move(from, from - 15, STANDARD);
					++cmove;
				}
				else if ((turn && grid[from - 15] < 0) || (!turn && grid[from - 15] > 0)) {
					m[cmove] = move(from, from - 15, CAPTURE);
					++cmove;
				}
			}
			return cmove;
		case QUEEN:
		case ROOK:
			for (i = from + NORTH; i >= 0; i += NORTH) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			if (abs(grid[from]) != QUEEN) { return cmove; }
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
					break;
				}
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				if (!grid[i]) {
					m[cmove] = move(from, i, STANDARD);
					++cmove;
				}
				else {
					if ((turn && grid[i] < 0) || (!turn && grid[i] > 0)) {
						m[cmove] = move(from, i, CAPTURE);
						++cmove;
					}
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
			if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) { ++threatened[us][from + SOUTHEAST]; }
			if ((from + EAST) % WIDTH > from % WIDTH) { ++threatened[us][from + EAST]; }
			if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) { ++threatened[us][from + NORTHWEST]; }
			if ((from + WEST) % WIDTH < from % WIDTH && from > 0) { ++threatened[us][from + WEST]; }
			if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) { ++threatened[us][from + SOUTHWEST]; }
			if (from < 56) { ++threatened[us][from + SOUTH]; }
			if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) { ++threatened[us][from + NORTHEAST]; }
			if (from > 7) { ++threatened[us][from + NORTH]; }
			return;
		case PAWN:
			i = (us) ? NORTH : SOUTH;
			if (from % WIDTH) { ++threatened[us][from + i + WEST]; }
			if (from % WIDTH != 7) { ++threatened[us][from + i + EAST]; }
			break;
		case KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54) { ++threatened[us][from + 10]; }
			if ((from + 17) % WIDTH > from % WIDTH && from < 47) { ++threatened[us][from + 17]; }
			if ((from - 10) % WIDTH < from % WIDTH && from > 9) { ++threatened[us][from - 10]; }
			if ((from - 17) % WIDTH < from % WIDTH && from > 16) { ++threatened[us][from - 17]; }
			if ((from + 6) % WIDTH < from % WIDTH && from < 58) { ++threatened[us][from + 6]; }
			if ((from + 15) % WIDTH < from % WIDTH && from < 49) { ++threatened[us][from + 15]; }
			if ((from - 6) % WIDTH > from % WIDTH && from > 5) { ++threatened[us][from - 6]; }
			if ((from - 15) % WIDTH > from % WIDTH && from > 14) { ++threatened[us][from - 15]; }
			break;
		case QUEEN:
			endgame = false;
		case ROOK:
			for (i = from + NORTH; i >= 0; i += NORTH) {
				++threatened[us][i];
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = NORTH;
						for (j = i + NORTH; j != kpos[turn]; j += NORTH) {
							if (grid[j]) { break; }
						}
						if (j == kpos[turn]) { ++cpins; }
					}
					break;
				}
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				++threatened[us][i];
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = SOUTH;
						for (j = i + SOUTH; j != kpos[turn]; j += SOUTH) {
							if (grid[j]) { break; }
						}
						if (j == kpos[turn]) { ++cpins; }
					}
					break;
				}
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				++threatened[us][i];
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = EAST;
						for (j = i + EAST; j != kpos[turn]; j += EAST) {
							if (grid[j]) { break; }
						}
						if (j == kpos[turn]) { ++cpins; }
					}
					break;
				}
			}
			for (i = from + WEST; i % WIDTH != 7 && i >= 0; i += WEST) {
				++threatened[us][i];
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = WEST;
						for (j = i + WEST; j != kpos[turn]; j += WEST) {
							if (grid[j]) { break; }
						}
						if (j == kpos[turn]) { ++cpins; }
					}
					break;
				}
			}
			if (abs(grid[from]) != QUEEN) { break; }
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += NORTHEAST) {
				++threatened[us][i];
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = NORTHEAST;
						for (j = i + NORTHEAST; j != kpos[turn]; j += NORTHEAST) {
							if (grid[j]) { break; }
						}
						if (j == kpos[turn]) { ++cpins; }
					}
					break;
				}
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += NORTHWEST) {
				++threatened[us][i];
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = NORTHWEST;
						for (j = i + NORTHWEST; j != kpos[turn]; j += NORTHWEST) {
							if (grid[j]) { break; }
						}
						if (j == kpos[turn]) { ++cpins; }
					}
					break;
				}
			}
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += SOUTHEAST) {
				++threatened[us][i];
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = SOUTHEAST;
						for (j = i + SOUTHEAST; j != kpos[turn]; j += SOUTHEAST) {
							if (grid[j]) { break; }
						}
						if (j == kpos[turn]) { ++cpins; }
					}
					break;
				}
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += SOUTHWEST) {
				++threatened[us][i];
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						pins[cpins][0] = i;
						pins[cpins][1] = SOUTHWEST;
						for (j = i + SOUTHWEST; j != kpos[turn]; j += SOUTHWEST) {
							if (grid[j]) { break; }
						}
						if (j == kpos[turn]) { ++cpins; }
					}
					break;
				}
			}
		}
		if (threatened[!turn][kpos[turn]] && cpos[threatened[!turn][kpos[turn]] - 1] == NOCHECK) { cpos[threatened[!turn][kpos[turn]] - 1] = from; }
	}
}
