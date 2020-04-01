#include "board.h"

namespace Chess {
#define CASTLEWK	(c) ((c) & 0x22)
#define CASTLEWQ	(c) ((c) & 0x12)
#define CASTLEBK	(c) ((c) & 0x0C)
#define CASTLE BQ	(c) ((c) & 0x05

	board::board() {//sets board to starting state
		z = zobrist();
		fenSet(STARTFEN);
	}

	void board::fenSet(std::string fs) {//sets board to state outlined in FEN string, no 50 move rule implementation
		for (int i = 0; i < MEMORY; ++i) { mHist[i] = move(); vHist[i] = 0; zHist[i] = 0; possiblemoves[i] = move(); cHist[i] = 0; }
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
			case 'K': { grid[counter] = KING; ++counter; break; }
			case 'p': { grid[counter] = -PAWN; ++counter; break; }
			case 'r': { grid[counter] = -ROOK; ++counter; break; }
			case 'n': { grid[counter] = -KNIGHT; ++counter; break; }
			case 'b': { grid[counter] = -BISHOP; ++counter; break; }
			case 'q': { grid[counter] = -QUEEN; ++counter; break; }
			case 'k': { grid[counter] = -KING; ++counter; break; }
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
		do{
			switch (fs[index]) {
			case 'K': {currC |= 1 << 0; currC |= 1 << 2; break; }
			case 'Q': {currC |= 1 << 1; currC |= 1 << 2; break; }
			case 'k': {currC |= 1 << 3; currC |= 1 << 4; break; }
			case 'q': {currC |= 1 << 5; currC |= 1 << 4; break; }
			}
			++index;
		} while (fs[index] != ' ');
		++index;
		if(fs[index]!='-'){
			int from = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
			int to   = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			mHist[cturn] = move(from, to, DOUBLEPUSH);
			++cturn;
		}
		++index;
		currV = 0;
		for (int i = 0; i < SPACES; ++i) { currV += grid[i]; }
		currZ = z.newKey(this);
		currM = (cturn) ? mHist[cturn - 1] : move();
		generateMoves();
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

	bool board::movePiece(move m) {//executes a move if legal, return value depicts success (nullmoves considered legal)
		zHist[cturn] = currZ;
		cHist[cturn] = currC;
		switch (m.getFlags()) {
		case STANDARD:
			currZ = (turn) ? currZ ^ z.pieces[grid[m.getFrom()] % 10][WHITE][m.getFrom()] : currZ ^ z.pieces[-grid[m.getFrom()] % 10][BLACK][m.getFrom()];
			currZ = (turn) ? currZ ^ z.pieces[grid[m.getFrom()] % 10][WHITE][m.getTo()] : currZ ^ z.pieces[-grid[m.getFrom()] % 10][BLACK][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
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
			break;
		case ENPASSANT:
			currV = (turn) ? currV + PAWN : currV - PAWN;
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
			break;
		case NPROMOTE:
			currV = (turn) ? currV - PAWN + KNIGHT: currV + PAWN - KNIGHT;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTE:
			currV = (turn) ? currV - PAWN + BISHOP : currV + PAWN - BISHOP;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTE:
			currV = (turn) ? currV - PAWN + ROOK : currV + PAWN - ROOK;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTE:
			currV = (turn) ? currV - PAWN + QUEEN : currV + PAWN - QUEEN;
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		case NPROMOTEC:
			currV = (turn) ? currV - PAWN + KNIGHT - grid[m.getTo()] : currV + PAWN - KNIGHT - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			currV = (turn) ? currV - PAWN + BISHOP - grid[m.getTo()] : currV + PAWN - BISHOP - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			currV = (turn) ? currV - PAWN + ROOK - grid[m.getTo()] : currV + PAWN - ROOK - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			currV = (turn) ? currV - PAWN + QUEEN - grid[m.getTo()] : currV + PAWN - QUEEN - grid[m.getTo()];
			currZ ^= z.pieces[PINDEX][turn][m.getFrom()];
			currZ ^= z.pieces[abs(grid[m.getTo()]) % 10][!turn][m.getTo()];
			currZ ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		case FAIL: {return false; }
		}
		if (currM.getFlags() == DOUBLEPUSH) { currZ ^= z.enpassant[currM.getTo()]; }
		if (currC & 1 << 2) {
			if (m.getFrom() == 60) { 
				currC &= ~(1 << 2); 
				if(currC & 1 << 0){ currC &= ~(1 << 0); currZ ^= z.castle[WHITE][0]; }
				if(currC & 1 << 1){ currC &= ~(1 << 1); currZ ^= z.castle[WHITE][1]; }
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
		generateMoves();
		if (checkTeam(!turn)) { 
			unmovePiece();
			return false; 
		}
		else {return true; }
	}

	void board::unmovePiece() {//unmakes a move
		turn = (turn) ? BLACK : WHITE;
		--cturn;
		switch (currM.getFlags()) {
		case STANDARD:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = EMPTY;
			break;
		case DOUBLEPUSH:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = EMPTY;
			break;
		case KCASTLE:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = EMPTY;
			grid[currM.getTo() + 1]= grid[currM.getTo() - 1]; 
			grid[currM.getTo() - 1] = EMPTY;
			break; 
		case QCASTLE:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = EMPTY;
			grid[currM.getTo() - 2]= grid[currM.getTo() + 1]; 
			grid[currM.getTo() + 1] = EMPTY;
			break; 
		case CAPTURE:
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[currM.getTo()] = vHist[cturn - 1] - currV;
			break; 
		case ENPASSANT:	
			grid[currM.getFrom()] = grid[currM.getTo()];
			grid[mHist[cturn - 1].getTo()] = (turn) ? -PAWN : PAWN;
			grid[currM.getTo()] = EMPTY;
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
		generateMoves();
	}

	move board::createMove(int from, int to) {
		for (int i = 0; i < cmove; ++i) {
			if (possiblemoves[i].getFrom() == from && possiblemoves[i].getTo() == to) { return possiblemoves[i]; }
		}
		return move();
	}

	void board::generateMoves() {//generates all pseudo legal moves
		cmove = 0;
		endgame = true;
		for (int i = 0; i < SPACES; ++i) {
			threatened[WHITE][i] = 0;
			threatened[BLACK][i] = 0;
		}
		for (int from = 0; from < SPACES; ++from) {
			if (grid[from]) { pieceMoves(from); }
		}
	}

	void board::pieceMoves(int from) {//generates all pseudo legal moves for one piece
		int i;
		bool us = (grid[from] > 0) ? WHITE : BLACK;
		switch (abs(grid[from])) {
		case KING:
			if (turn == us) {
				if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) {
					++threatened[us][from + SOUTHEAST];
					if (!threatened[!us][from + SOUTHEAST] && ((us && grid[from + SOUTHEAST] <= 0) || (!us && grid[from + SOUTHEAST] >= 0))) {
						if (!grid[from + SOUTHEAST]) { possiblemoves[cmove] = move(from, from + SOUTHEAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTHEAST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + EAST) % WIDTH > from % WIDTH) {
					++threatened[us][from + EAST];
					if (!threatened[!us][from + EAST] && ((us && grid[from + EAST] <= 0) || (!us && grid[from + EAST] >= 0))) {
						if (!grid[from + EAST]) { possiblemoves[cmove] = move(from, from + EAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + EAST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) {
					++threatened[us][from + NORTHWEST];
					if (!threatened[!us][from + NORTHWEST] && ((us && grid[from + NORTHWEST] <= 0) || (!us && grid[from + NORTHWEST] >= 0))) {
						if (!grid[from + NORTHWEST]) { possiblemoves[cmove] = move(from, from + NORTHWEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTHWEST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + WEST) % WIDTH < from % WIDTH) {
					++threatened[us][from + WEST];
					if (!threatened[!us][from + WEST] && ((us && grid[from + WEST] <= 0) || (!us && grid[from + WEST] >= 0))) {
						if (!grid[from + WEST]) { possiblemoves[cmove] = move(from, from + WEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + WEST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) {
					++threatened[us][from + SOUTHWEST];
					if (!threatened[!us][from + SOUTHWEST] && ((us && grid[from + SOUTHWEST] <= 0) || (!us && grid[from + SOUTHWEST] >= 0))) {
						if (!grid[from + SOUTHWEST]) { possiblemoves[cmove] = move(from, from + SOUTHWEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTHWEST, CAPTURE); }
						++cmove;
					}
				}
				if (from < 56) {
					++threatened[us][from + SOUTH];
					if (!threatened[!us][from + SOUTH] && ((us && grid[from + SOUTH] <= 0) || (!us && grid[from + SOUTH] >= 0))) {
						if (!grid[from + SOUTH]) { possiblemoves[cmove] = move(from, from + SOUTH, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTH, CAPTURE); }
						++cmove;
					}
				}
				if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) {
					++threatened[us][from + NORTHEAST];
					if (!threatened[!us][from + NORTHEAST] && ((us && grid[from + NORTHEAST] <= 0) || (!us && grid[from + NORTHEAST] >= 0))) {
						if (!grid[from + NORTHEAST]) { possiblemoves[cmove] = move(from, from + NORTHEAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTHEAST, CAPTURE); }
						++cmove;
					}
				}
				if (from > 7) {
					++threatened[us][from + NORTH];
					if (!threatened[!us][from + NORTH] && ((us && grid[from + NORTH] <= 0) || (!us && grid[from + NORTH] >= 0))) {
						if (!grid[from + NORTH]) { possiblemoves[cmove] = move(from, from + NORTH, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTH, CAPTURE); }
						++cmove;
					}
				}
				if (us) {
					if (from == 60 && !threatened[BLACK][60] && (currC & 1 << 2)) {
						if (!grid[61] && !grid[62] && !threatened[BLACK][61] && !threatened[BLACK][62] && (currC & 1 << 0)) {
							possiblemoves[cmove] = move(60, 62, KCASTLE);
							++cmove;
						}
						if (!grid[59] && !grid[58] && !grid[57] && !threatened[BLACK][59] && !threatened[BLACK][58] && (currC & 1 << 1)) {
							possiblemoves[cmove] = move(60, 58, QCASTLE);
							++cmove;
						}
					}
				}
				else {
					if (from == 4 && !threatened[WHITE][4] && (currC & 1 << 4)) {
						if (!grid[5] && !grid[6] && !threatened[WHITE][5] && !threatened[WHITE][6] && (currC & 1 << 3)) {
							possiblemoves[cmove] = move(4, 6, KCASTLE);
							++cmove;
						}
						if (!grid[3] && !grid[2] && !grid[1] && !threatened[WHITE][3] && !threatened[WHITE][2] && (currC & 1 << 5)) {
							possiblemoves[cmove] = move(4, 2, QCASTLE);
							++cmove;
						}
					}
				}
			}
			else {
				if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) { ++threatened[us][from + SOUTHEAST]; }
				if ((from + EAST) % WIDTH > from % WIDTH) { ++threatened[us][from + EAST]; }
				if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) { ++threatened[us][from + NORTHWEST]; }
				if ((from + WEST) % WIDTH < from % WIDTH && from > 0) { ++threatened[us][from + WEST]; }
				if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) { ++threatened[us][from + SOUTHWEST]; }
				if (from < 56) { ++threatened[us][from + SOUTH]; }
				if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) { ++threatened[us][from + NORTHEAST]; }
				if (from > 7) { ++threatened[us][from + NORTH]; }
			}
			return;
		case PAWN:
			i = (us) ? NORTH : SOUTH;
			if (turn == us) {
				if (from % WIDTH) {
					++threatened[us][from + i + WEST];
					if ((us && grid[from + i + WEST] < 0) || (!us && grid[from + i + WEST] > 0)) {
						if ((us && from > 15) || (!us && from < 48)) { possiblemoves[cmove] = move(from, from + i + WEST, CAPTURE); }
						else {
							possiblemoves[cmove] = move(from, from + i + WEST, NPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + i + WEST, BPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + i + WEST, RPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + i + WEST, QPROMOTEC);
						}
						++cmove;
					}
				}
				if (from % WIDTH != 7) {
					++threatened[us][from + i + EAST];
					if ((us && grid[from + i + EAST] < 0) || (!us && grid[from + i + EAST] > 0)) {
						if ((us && from > 15) || (!us && from < 48)) { possiblemoves[cmove] = move(from, from + i + EAST, CAPTURE); }
						else {
							possiblemoves[cmove] = move(from, from + i + EAST, NPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + i + EAST, BPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + i + EAST, RPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + i + EAST, QPROMOTEC);
						}
						++cmove;
					}
				}
				if (currM.getFlags() == DOUBLEPUSH && ((currM.getTo() == from + EAST || currM.getTo() == from + WEST))) {
					++threatened[us][currM.getTo()];
					possiblemoves[cmove] = move(from, currM.getTo() + i, ENPASSANT);
					++cmove;
				}
				if (!grid[from + i]) {
					if ((us && from > 15) || (!us && from < 48)) { possiblemoves[cmove] = move(from, from + i, STANDARD); }
					else {
						possiblemoves[cmove] = move(from, from + i, NPROMOTE);
						++cmove;
						possiblemoves[cmove] = move(from, from + i, BPROMOTE);
						++cmove;
						possiblemoves[cmove] = move(from, from + i, RPROMOTE);
						++cmove;
						possiblemoves[cmove] = move(from, from + i, QPROMOTE);
					}
					++cmove;
					if (((us && from > 47) || (!us && from < 16)) && !grid[from + 2 * i]) {
						possiblemoves[cmove] = move(from, from + 2 * i, DOUBLEPUSH);
						++cmove;
					}
				}
			}
			else {
				if (from % WIDTH) { ++threatened[us][from + i + WEST]; }
				if (from % WIDTH != 7) { ++threatened[us][from + i + EAST]; }
			}
			return;
		case KNIGHT:
			if (turn == us) {
				if ((from + 10) % WIDTH > from % WIDTH && from < 54) {
					++threatened[us][from + 10];
					if (!grid[from + 10]) {
						possiblemoves[cmove] = move(from, from + 10, STANDARD);
						++cmove;
					}
					else if ((us && grid[from + 10] < 0) || (!us && grid[from + 10] > 0)) {
						possiblemoves[cmove] = move(from, from + 10, CAPTURE);
						++cmove;
					}
				}
				if ((from + 17) % WIDTH > from % WIDTH && from < 47) {
					++threatened[us][from + 17];
					if (!grid[from + 17]) {
						possiblemoves[cmove] = move(from, from + 17, STANDARD);
						++cmove;
					}
					else if ((us && grid[from + 17] < 0) || (!us && grid[from + 17] > 0)) {
						possiblemoves[cmove] = move(from, from + 17, CAPTURE);
						++cmove;
					}
				}
				if ((from - 10) % WIDTH < from % WIDTH && from > 9) {
					++threatened[us][from - 10];
					if (!grid[from - 10]) {
						possiblemoves[cmove] = move(from, from - 10, STANDARD);
						++cmove;
					}
					else if ((us && grid[from - 10] < 0) || (!us && grid[from - 10] > 0)) {
						possiblemoves[cmove] = move(from, from - 10, CAPTURE);
						++cmove;
					}
				}
				if ((from - 17) % WIDTH < from % WIDTH && from > 16) {
					++threatened[us][from - 17];
					if (!grid[from - 17]) {
						possiblemoves[cmove] = move(from, from - 17, STANDARD);
						++cmove;
					}
					else if ((us && grid[from - 17] < 0) || (!us && grid[from - 17] > 0)) {
						possiblemoves[cmove] = move(from, from - 17, CAPTURE);
						++cmove;
					}
				}
				if ((from + 6) % WIDTH < from % WIDTH && from < 58) {
					++threatened[us][from + 6];
					if (!grid[from + 6]) {
						possiblemoves[cmove] = move(from, from + 6, STANDARD);
						++cmove;
					}
					else if ((us && grid[from + 6] < 0) || (!us && grid[from + 6] > 0)) {
						possiblemoves[cmove] = move(from, from + 6, CAPTURE);
						++cmove;
					}
				}
				if ((from + 15) % WIDTH < from % WIDTH && from < 49) {
					++threatened[us][from + 15];
					if (!grid[from + 15]) {
						possiblemoves[cmove] = move(from, from + 15, STANDARD);
						++cmove;
					}
					else if (us && (grid[from + 15] < 0) || (!us && grid[from + 15] > 0)) {
						possiblemoves[cmove] = move(from, from + 15, CAPTURE);
						++cmove;
					}
				}
				if ((from - 6) % WIDTH > from % WIDTH && from > 5) {
					++threatened[us][from - 6];
					if (!grid[from - 6]) {
						possiblemoves[cmove] = move(from, from - 6, STANDARD);
						++cmove;
					}
					else if ((us && grid[from - 6] < 0) || (!us && grid[from - 6] > 0)) {
						possiblemoves[cmove] = move(from, from - 6, CAPTURE);
						++cmove;
					}
				}
				if ((from - 15) % WIDTH > from % WIDTH && from > 14) {
					++threatened[us][from - 15];
					if (!grid[from - 15]) {
						possiblemoves[cmove] = move(from, from - 15, STANDARD);
						++cmove;
					}
					else if ((us && grid[from - 15] < 0) || (!us && grid[from - 15] > 0)) {
						possiblemoves[cmove] = move(from, from - 15, CAPTURE);
						++cmove;
					}
				}
			}
			else {
				if ((from + 10) % WIDTH > from % WIDTH && from < 54) { ++threatened[us][from + 10]; }
				if ((from + 17) % WIDTH > from % WIDTH && from < 47) { ++threatened[us][from + 17]; }
				if ((from - 10) % WIDTH < from % WIDTH && from > 9) { ++threatened[us][from - 10]; }
				if ((from - 17) % WIDTH < from % WIDTH && from > 16) { ++threatened[us][from - 17]; }
				if ((from + 6) % WIDTH < from % WIDTH && from < 58) { ++threatened[us][from + 6]; }
				if ((from + 15) % WIDTH < from % WIDTH && from < 49) { ++threatened[us][from + 15]; }
				if ((from - 6) % WIDTH > from % WIDTH && from > 5) { ++threatened[us][from - 6]; }
				if ((from - 15) % WIDTH > from % WIDTH && from > 14) { ++threatened[us][from - 15]; }
			}
			return;
		case QUEEN:
			endgame = false;
		case ROOK:
			if (turn == us) {
				for (i = from + NORTH; i >= 0; i += NORTH) {
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((us && grid[i] < 0) || (!us && grid[i] > 0)) {
							possiblemoves[cmove] = move(from, i, CAPTURE);
							++cmove;
						}
						break;
					}
				}
				for (i = from + SOUTH; i < SPACES; i += SOUTH) {
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((us && grid[i] < 0) || (!us && grid[i] > 0)) {
							possiblemoves[cmove] = move(from, i, CAPTURE);
							++cmove;
						}
						break;
					}
				}
				for (i = from + EAST; i % WIDTH; i += EAST) {
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((us && grid[i] < 0) || (!us && grid[i] > 0)) {
							possiblemoves[cmove] = move(from, i, CAPTURE);
							++cmove;
						}
						break;
					}
				}
				for (i = from + WEST; i % WIDTH != 7; i += WEST) {
					if (i < 0) { break; }
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((us && grid[i] < 0) || (!us && grid[i] > 0)) {
							possiblemoves[cmove] = move(from, i, CAPTURE);
							++cmove;
						}
						break;
					}
				}
			}
			else {
				for (i = from + NORTH; i >= 0; i += NORTH) {
					++threatened[us][i];
					if (grid[i]) { break; }
				}
				for (i = from + SOUTH; i < SPACES; i += SOUTH) {
					++threatened[us][i];
					if (grid[i]) { break; }
				}
				for (i = from + EAST; i % WIDTH; i += EAST) {
					++threatened[us][i];
					if (grid[i]) { break; }
				}
				for (i = from + WEST; i % WIDTH != 7; i += WEST) {
					if (i < 0) { break; }
					++threatened[us][i];
					if (grid[i]) { break; }
				}
			}
			if (abs(grid[from]) != QUEEN) { return; }
		case BISHOP:
			if (turn == us) {
				for (i = from + NORTHEAST; i % WIDTH > from % WIDTH; i += NORTHEAST) {
					if (i < 0) { break; }
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((us && grid[i] < 0) || (!us && grid[i] > 0)) {
							possiblemoves[cmove] = move(from, i, CAPTURE);
							++cmove;
						}
						break;
					}
				}
				for (i = from + NORTHWEST; i % WIDTH < from % WIDTH; i += NORTHWEST) {
					if (i < 0) { break; }
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((us && grid[i] < 0) || (!us && grid[i] > 0)) {
							possiblemoves[cmove] = move(from, i, CAPTURE);
							++cmove;
						}
						break;
					}
				}
				for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH; i += SOUTHEAST) {
					if (i >= SPACES) { break; }
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((us && grid[i] < 0) || (!us && grid[i] > 0)) {
							possiblemoves[cmove] = move(from, i, CAPTURE);
							++cmove;
						}
						break;
					}
				}
				for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH; i += SOUTHWEST) {
					if (i >= SPACES) { break; }
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((us && grid[i] < 0) || (!us && grid[i] > 0)) {
							possiblemoves[cmove] = move(from, i, CAPTURE);
							++cmove;
						}
						break;
					}
				}
			}
			else {
				for (i = from + NORTHEAST; i % WIDTH > from % WIDTH; i += NORTHEAST) {
					if (i < 0) { break; }
					++threatened[us][i];
					if (grid[i]) { break; }
				}
				for (i = from + NORTHWEST; i % WIDTH < from % WIDTH; i += NORTHWEST) {
					if (i < 0) { break; }
					++threatened[us][i];
					if (grid[i]) { break; }
				}
				for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH; i += SOUTHEAST) {
					if (i >= SPACES) { break; }
					++threatened[us][i];
					if (grid[i]) { break; }
				}
				for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH; i += SOUTHWEST) {
					if (i >= SPACES) { break; }
					++threatened[us][i];
					if (grid[i]) { break; }
				}
			}
			return;
		}
	}
	
	bool board::checkTeam(bool team) {//looks for check
		int target = (team) ? KING : -KING;
		for (int to = 0; to < SPACES; ++to) {
			if (grid[to] == target) { return (threatened[!team][to]) ? true : false; }
		}
	}
}