#include "board.h"

namespace Chess {

	board::board() {//sets board to starting state
		z = zobrist();
		fenSet(STARTFEN);
	}

	void board::fenSet(std::string fs) {//sets board to state outlined in FEN string, no 50 move rule implementation
		for (uint8_t i = 0; i < MEMORY; ++i) { mHist[i] = move(); vHist[i] = 0; zHist[i] = 0; possiblemoves[i] = move(); }
		cturn = 0;
		uint8_t index = 0;
		uint8_t counter = 0;
		uint8_t helper;
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
		bool castled[4] = { true,true,true,true };
		do{
			switch (fs[index]) {
			case 'K': {castled[3] = false; break; }
			case 'Q': {castled[2] = false; break; }
			case 'k': {castled[1] = false; break; }
			case 'q': {castled[0] = false; break; }
			}
			++index;
		} while (fs[index] != ' ');
		if (castled[0]) { mHist[cturn] = move(0, 0, NULLMOVE); cturn++; }
		if (castled[1]) { mHist[cturn] = move(7, 0, NULLMOVE); cturn++; }
		if (castled[2]) { mHist[cturn] = move(56, 0, NULLMOVE); cturn++; }
		if (castled[3]) { mHist[cturn] = move(63, 0, NULLMOVE); cturn++; }
		++index;
		if(fs[index]!='-'){
			uint8_t from = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
			uint8_t to   = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			mHist[cturn] = move(from, to, DOUBLEPUSH);
			cturn++;
		}
		++index;
		zHist[cturn] = z.newKey(this);
		generateMoves();
	}

	bool board::movePiece(move m) {//executes a move if legal, return value depicts success (nullmoves considered legal)
		bool enemy = (turn) ? BLACK : WHITE;
		zHist[cturn + 1] = zHist[cturn];
		switch (m.getFlags()) {
		case STANDARD:
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getFrom()]) % 100][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getFrom()]) % 100][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			break;
		case DOUBLEPUSH:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			break;
		case KCASTLE:
			zHist[cturn + 1] ^= z.pieces[KINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[KINDEX][turn][m.getTo()];
			zHist[cturn + 1] ^= z.pieces[RINDEX][turn][m.getTo() + 1];
			zHist[cturn + 1] ^= z.pieces[RINDEX][turn][m.getTo() - 1];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() - 1] = grid[m.getTo() + 1];
			grid[m.getTo() + 1] = EMPTY;
			break;
		case QCASTLE:
			zHist[cturn + 1] ^= z.pieces[KINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[KINDEX][turn][m.getTo()];
			zHist[cturn + 1] ^= z.pieces[RINDEX][turn][m.getTo() - 2];
			zHist[cturn + 1] ^= z.pieces[RINDEX][turn][m.getTo() + 1];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() + 1] = grid[m.getTo() - 2];
			grid[m.getTo() - 2] = EMPTY;
			break;
		case ENPASSANT:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[PINDEX][enemy][m.getTo()];
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[mHist[cturn - 1].getTo()] = EMPTY;
			break;
		case CAPTURE:
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getFrom()]) % 100][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getFrom()]) % 100][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			break;
		case NPROMOTE:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			break;
		case BPROMOTE:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			break;
		case RPROMOTE:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			break;
		case QPROMOTE:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			break;
		case NPROMOTEC:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zHist[cturn + 1] ^= z.pieces[KINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			break;
		case BPROMOTEC:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zHist[cturn + 1] ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			break;
		case RPROMOTEC:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zHist[cturn + 1] ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			break;
		case QPROMOTEC:
			zHist[cturn + 1] ^= z.pieces[PINDEX][turn][m.getFrom()];
			zHist[cturn + 1] ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zHist[cturn + 1] ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			break;
		case FAIL: {return false; }
		}
		if (m.getFlags() != NULLMOVE) { grid[m.getFrom()] = EMPTY; }
		for (uint8_t i = 0; i < SPACES; ++i) { vHist[cturn] += grid[i]; }
		mHist[cturn] = m;
		++cturn;
		turn = (turn) ? BLACK : WHITE;
		generateMoves();
		if (checkTeam(!turn)) { 
			unmovePiece();
			return false; 
		}
		else { return true; }
	}

	void board::unmovePiece() {//unmakes a move
		turn = (turn) ? BLACK : WHITE;
		zHist[cturn] = 0;
		--cturn;
		switch (mHist[cturn].getFlags()) {
		case STANDARD:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			break;
		case DOUBLEPUSH:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			break;
		case KCASTLE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			grid[mHist[cturn].getTo() + 1]= grid[mHist[cturn].getTo() - 1]; 
			grid[mHist[cturn].getTo() - 1] = EMPTY;
			break; 
		case QCASTLE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			grid[mHist[cturn].getTo() - 2]= grid[mHist[cturn].getTo() + 1]; 
			grid[mHist[cturn].getTo() + 1] = EMPTY;
			break; 
		case CAPTURE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = vHist[cturn - 1] - vHist[cturn];
			break; 
		case ENPASSANT:	
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn - 1].getTo()] = (!turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			break; 
		case NPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			break; 
		case BPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			break; 
		case RPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			break; 
		case QPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			break; 
		case NPROMOTEC: 
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + KNIGHT - PAWN : vHist[cturn - 1] - vHist[cturn] - KNIGHT + PAWN;
			break; 
		case BPROMOTEC: 
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + BISHOP - PAWN : vHist[cturn - 1] - vHist[cturn] - BISHOP + PAWN;
			break; 
		case RPROMOTEC: 
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + ROOK - PAWN : vHist[cturn - 1] - vHist[cturn] - ROOK + PAWN;
			break; 
		case QPROMOTEC: 
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + QUEEN - PAWN : vHist[cturn - 1] - vHist[cturn] - QUEEN + PAWN;
			break;
		}
		generateMoves();
		vHist[cturn] = 0;
		mHist[cturn] = move();
	}

	move board::createMove(uint8_t from, uint8_t to) {
		for (uint8_t i = 0; i < cmove; ++i) {
			if (possiblemoves[i].getFrom() == from && possiblemoves[i].getTo() == to) {
				return possiblemoves[i];
			}
		}
		return move();
	}

	int16_t board::negaEval() {//evaluates for negamax function
		int8_t sum = 0;
		for (uint8_t from = 0; from < SPACES; ++from) {
			sum += threatened[WHITE][from];
			sum -= threatened[BLACK][from];
		}
		return (turn) ? vHist[cturn - 1] + 10*sum: -vHist[cturn - 1] - 10*sum; 
	}

	void board::generateMoves() {//generates all pseudo legal moves
		cmove = 0;
		for (uint8_t i = 0; i < SPACES; ++i) {
			threatened[WHITE][i] = 0;
			threatened[BLACK][i] = 0;
		}
		for (uint8_t from = 0; from < SPACES; ++from) {
			if (grid[from]) { pieceMoves(from); }
		}
	}

	void board::pieceMoves(uint8_t from) {//generates all pseudo legal moves for one piece
		int16_t i;
		bool us = (grid[from] > 0) ? WHITE : BLACK;
		switch (grid[from]) {
		case KING:
			if (turn == us) {
				if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) {
					++threatened[WHITE][from + SOUTHEAST];
					if (!threatened[BLACK][from + SOUTHEAST] && grid[from + SOUTHEAST] <= 0) {
						if (!grid[from + SOUTHEAST]) { possiblemoves[cmove] = move(from, from + SOUTHEAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTHEAST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + EAST) % WIDTH > from % WIDTH) {
					++threatened[WHITE][from + EAST];
					if (!threatened[BLACK][from + EAST] && grid[from + EAST] <= 0) {
						if (!grid[from + EAST]) { possiblemoves[cmove] = move(from, from + EAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + EAST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) {
					++threatened[WHITE][from + NORTHWEST];
					if (!threatened[BLACK][from + NORTHWEST] && grid[from + NORTHWEST] <= 0) {
						if (!grid[from + NORTHWEST]) { possiblemoves[cmove] = move(from, from + NORTHWEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTHWEST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + WEST) % WIDTH < from % WIDTH && from > 0) {
					++threatened[WHITE][from + WEST];
					if (!threatened[BLACK][from + WEST] && grid[from + WEST] <= 0) {
						if (!grid[from + WEST]) { possiblemoves[cmove] = move(from, from + WEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + WEST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) {
					++threatened[WHITE][from + SOUTHWEST];
					if (!threatened[BLACK][from + SOUTHWEST] && grid[from + SOUTHWEST] >= 0) {
						if (!grid[from + SOUTHWEST]) { possiblemoves[cmove] = move(from, from + SOUTHWEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTHWEST, CAPTURE); }
						++cmove;
					}
				}
				if (from < 56) {
					++threatened[WHITE][from + SOUTH];
					if (!threatened[BLACK][from + SOUTH] && grid[from + SOUTH] <= 0) {
						if (!grid[from + SOUTH]) { possiblemoves[cmove] = move(from, from + SOUTH, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTH, CAPTURE); }
						++cmove;
					}
				}
				if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) {
					++threatened[WHITE][from + NORTHEAST];
					if (!threatened[BLACK][from + NORTHEAST] && grid[from + NORTHEAST] <= 0) {
						if (!grid[from + NORTHEAST]) { possiblemoves[cmove] = move(from, from + NORTHEAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTHEAST, CAPTURE); }
						++cmove;
					}
				}
				if (from > 7) {
					++threatened[WHITE][from + NORTH];
					if (!threatened[BLACK][from + NORTH] && grid[from + NORTH] <= 0) {
						if (!grid[from + NORTH]) { possiblemoves[cmove] = move(from, from + NORTH, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTH, CAPTURE); }
						++cmove;
					}
				}
				if (from == 60 && !grid[61] && !grid[62] && !threatened[BLACK][60] && !threatened[BLACK][61] && !threatened[BLACK][62]) {
					for (i = 0; i < cturn; ++i) { if (mHist[i].getFrom() == 60 || mHist[i].getFrom() == 63 || mHist[i].getTo() == 63) { break; } }
					if (i == cturn) {
						possiblemoves[cmove] = move(60, 62, KCASTLE);
						cmove++;
					}
				}
				if (from == 60 && !grid[59] && !grid[58] && !grid[57] && !threatened[BLACK][60] && !threatened[BLACK][59] && !threatened[BLACK][58]) {
					for (i = 0; i < cturn; ++i) { if (mHist[i].getFrom() == 60 || mHist[i].getFrom() == 56 || mHist[i].getTo() == 56) { break; } }
					if (i == cturn) {
						possiblemoves[cmove] = move(60, 58, QCASTLE);
						cmove++;
					}
				}
			}
			else {
				if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) { ++threatened[WHITE][from + SOUTHEAST]; }
				if ((from + EAST) % WIDTH > from % WIDTH) { ++threatened[WHITE][from + EAST]; }
				if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) { ++threatened[WHITE][from + NORTHWEST]; }
				if ((from + WEST) % WIDTH < from % WIDTH && from > 0) { ++threatened[WHITE][from + WEST]; }
				if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) { ++threatened[WHITE][from + SOUTHWEST]; }
				if (from < 56) { ++threatened[WHITE][from + SOUTH]; }
				if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) { ++threatened[WHITE][from + NORTHEAST]; }
				if (from > 7) { ++threatened[WHITE][from + NORTH]; }
			}
			return;
		case -KING:
			if (turn == us) {
				if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) {
					++threatened[BLACK][from + SOUTHEAST];
					if (!threatened[WHITE][from + SOUTHEAST] && grid[from + SOUTHEAST] >= 0) {
						if (!grid[from + SOUTHEAST]) { possiblemoves[cmove] = move(from, from + SOUTHEAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTHEAST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + EAST) % WIDTH > from % WIDTH) {
					++threatened[BLACK][from + EAST];
					if (!threatened[WHITE][from + EAST] && grid[from + EAST] >= 0) {
						if (!grid[from + EAST]) { possiblemoves[cmove] = move(from, from + EAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + EAST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) {
					++threatened[BLACK][from + NORTHWEST];
					if (!threatened[WHITE][from + NORTHWEST] && grid[from + NORTHWEST] >= 0) {
						if (!grid[from + NORTHWEST]) { possiblemoves[cmove] = move(from, from + NORTHWEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTHWEST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + WEST) % WIDTH < from % WIDTH && from > 0) {
					++threatened[BLACK][from + WEST];
					if (!threatened[WHITE][from + WEST] && grid[from + WEST] >= 0) {
						if (!grid[from + WEST]) { possiblemoves[cmove] = move(from, from + WEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + WEST, CAPTURE); }
						++cmove;
					}
				}
				if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) {
					++threatened[BLACK][from + SOUTHWEST];
					if (!threatened[WHITE][from + SOUTHWEST] && grid[from + SOUTHWEST] >= 0) {
						if (!grid[from + SOUTHWEST]) { possiblemoves[cmove] = move(from, from + SOUTHWEST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTHWEST, CAPTURE); }
						++cmove;
					}
				}
				if (from < 56) {
					++threatened[BLACK][from + SOUTH];
					if (!threatened[WHITE][from + SOUTH] && grid[from + SOUTH] >= 0) {
						if (!grid[from + SOUTH]) { possiblemoves[cmove] = move(from, from + SOUTH, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + SOUTH, CAPTURE); }
						++cmove;
					}
				}
				if ((from + NORTHEAST) % WIDTH > from % WIDTH && from >= 7) {
					++threatened[BLACK][from + NORTHEAST];
					if (!threatened[WHITE][from + NORTHEAST] && grid[from + NORTHEAST] >= 0) {
						if (!grid[from + NORTHEAST]) { possiblemoves[cmove] = move(from, from + NORTHEAST, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTHEAST, CAPTURE); }
						++cmove;
					}
				}
				if (from > 7) {
					++threatened[BLACK][from + NORTH];
					if (!threatened[WHITE][from + NORTH] && grid[from + NORTH] >= 0) {
						if (!grid[from + NORTH]) { possiblemoves[cmove] = move(from, from + NORTH, STANDARD); }
						else { possiblemoves[cmove] = move(from, from + NORTH, CAPTURE); }
						++cmove;
					}
				}
				if (from == 4 && !grid[5] && !grid[6] && !threatened[WHITE][4] && !threatened[WHITE][5] && !threatened[WHITE][6]) {
					for (i = 0; i < cturn; ++i) { if (mHist[i].getFrom() == 4 || mHist[i].getFrom() == 7 || mHist[i].getTo() == 7) { break; } }
					if (i == cturn) {
						possiblemoves[cmove] = move(4, 6, KCASTLE);
						cmove++;
					}
				}
				if (from == 4 && !grid[3] && !grid[2] && !grid[1] && !threatened[WHITE][4] && !threatened[WHITE][3] && !threatened[WHITE][2]) {
					for (i = 0; i < cturn; ++i) { if (mHist[i].getFrom() == 4 || mHist[i].getFrom() == 0 || mHist[i].getTo() == 0) { break; } }
					if (i == cturn) {
						possiblemoves[cmove] = move(4, 2, QCASTLE);
						cmove++;
					}
				}
			}
			else {
				if ((from + SOUTHEAST) % WIDTH > from % WIDTH && from < 55) { ++threatened[BLACK][from + SOUTHEAST]; }
				if ((from + EAST) % WIDTH > from % WIDTH) { ++threatened[BLACK][from + EAST]; }
				if ((from + NORTHWEST) % WIDTH < from % WIDTH && from > 8) { ++threatened[BLACK][from + NORTHWEST]; }
				if ((from + WEST) % WIDTH < from % WIDTH && from > 0) { ++threatened[BLACK][from + WEST]; }
				if ((from + SOUTHWEST) % WIDTH < from % WIDTH && from < 57) { ++threatened[BLACK][from + SOUTHWEST]; }
				if (from < 56) { ++threatened[BLACK][from + SOUTH]; }
				if ((from + NORTHEAST) % WIDTH > from % WIDTH && from > 6) { ++threatened[BLACK][from + NORTHEAST]; }
				if (from > 7) { ++threatened[BLACK][from + NORTH]; }
			}
			return;
		case PAWN:
			if (turn == us) {
				if (from % WIDTH) {
					++threatened[WHITE][from + NORTHWEST];
					if (grid[from + NORTHWEST] < 0) {
						if (from > 15) { possiblemoves[cmove] = move(from, from + NORTHWEST, CAPTURE); }
						else { 
							possiblemoves[cmove] = move(from, from + NORTHWEST, NPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + NORTHWEST, BPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + NORTHWEST, RPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + NORTHWEST, QPROMOTEC);
						}
						++cmove;
					}
				}
				if (from % WIDTH != 7) {
					++threatened[WHITE][from + NORTHEAST];
					if (grid[from + NORTHEAST] < 0) {
						if (from > 15) { possiblemoves[cmove] = move(from, from + NORTHEAST, CAPTURE); }
						else { 
							possiblemoves[cmove] = move(from, from + NORTHEAST, NPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + NORTHEAST, BPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + NORTHEAST, RPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + NORTHEAST, QPROMOTEC);
						}
						++cmove;
					}
				}
				if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && ((mHist[cturn - 1].getTo() == from + EAST || mHist[cturn - 1].getTo() == from + WEST))) {
					++threatened[WHITE][mHist[cturn - 1].getTo()];
					possiblemoves[cmove] = move(from, mHist[cturn - 1].getTo() + NORTH, ENPASSANT);
					++cmove;
				}
				if (!grid[from + NORTH]) {
					if (from > 15) { possiblemoves[cmove] = move(from, from + NORTH, STANDARD); }
					else { 
						possiblemoves[cmove] = move(from, from + NORTH, NPROMOTE);
						++cmove;
						possiblemoves[cmove] = move(from, from + NORTH, BPROMOTE);
						++cmove;
						possiblemoves[cmove] = move(from, from + NORTH, RPROMOTE);
						++cmove;
						possiblemoves[cmove] = move(from, from + NORTH, QPROMOTE);
					}
					++cmove;
					if (from > 47 && !grid[from + 2 * NORTH]) {
						possiblemoves[cmove] = move(from, from + 2 * NORTH, DOUBLEPUSH);
						++cmove;
					}
				}
			}
			else {
				if (from % WIDTH) { ++threatened[WHITE][from + NORTHWEST]; }
				if (from % WIDTH != 7) { ++threatened[WHITE][from + NORTHEAST]; }
			}
			return;
		case -PAWN:
			if (turn == us) {
				if (from % WIDTH) {
					++threatened[BLACK][from + SOUTHWEST];
					if (grid[from + SOUTHWEST] > 0) {
						if (from < 48) { possiblemoves[cmove] = move(from, from + SOUTHWEST, CAPTURE); }
						else { 
							possiblemoves[cmove] = move(from, from + SOUTHWEST, NPROMOTEC); 
							++cmove;
							possiblemoves[cmove] = move(from, from + SOUTHWEST, BPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + SOUTHWEST, RPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + SOUTHWEST, QPROMOTEC);
						}
						++cmove;
					}
				}
				if (from % WIDTH != 7) {
					++threatened[BLACK][from + SOUTHEAST];
					if (grid[from + SOUTHEAST] > 0) {
						if (from < 48) { possiblemoves[cmove] = move(from, from + SOUTHEAST, CAPTURE); }
						else { 
							possiblemoves[cmove] = move(from, from + SOUTHEAST, NPROMOTEC); 
							++cmove;
							possiblemoves[cmove] = move(from, from + SOUTHEAST, BPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + SOUTHEAST, RPROMOTEC);
							++cmove;
							possiblemoves[cmove] = move(from, from + SOUTHEAST, QPROMOTEC);
						}
						++cmove;
					}
				}
				if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && ((mHist[cturn - 1].getTo() == from + EAST || mHist[cturn - 1].getTo() == from + WEST))) {
					++threatened[BLACK][mHist[cturn - 1].getTo()];
					possiblemoves[cmove] = move(from, mHist[cturn - 1].getTo() + SOUTH, ENPASSANT);
					++cmove;
				}
				if (!grid[from + SOUTH]) {
					if (from < 48) { possiblemoves[cmove] = move(from, from + SOUTH, STANDARD); }
					else { 
						possiblemoves[cmove] = move(from, from + SOUTH, NPROMOTE); 
						++cmove;
						possiblemoves[cmove] = move(from, from + SOUTH, BPROMOTE);
						++cmove;
						possiblemoves[cmove] = move(from, from + SOUTH, RPROMOTE);
						++cmove;
						possiblemoves[cmove] = move(from, from + SOUTH, QPROMOTE);
					}
					++cmove;
					if (from < 16 && !grid[from + 2 * SOUTH]) {
						possiblemoves[cmove] = move(from, from + 2 * SOUTH, DOUBLEPUSH);
						++cmove;
					}
				}
			}
			else {
				if (from % WIDTH) { ++threatened[BLACK][from + SOUTHWEST]; }
				if (from % WIDTH != 7) { ++threatened[BLACK][from + SOUTHEAST]; }
			}
			return;
		case KNIGHT:
		case -KNIGHT:
			if (turn == us) {
				if ((from + 10) % WIDTH > from % WIDTH && from < 54) {
					++threatened[us][from + 10];
					if (!grid[from + 10]) { 
						possiblemoves[cmove] = move(from, from + 10, STANDARD); 
						++cmove;
					}
					else if ((grid[from + 10] < 0 && us) || (grid[from + 10] > 0 && !us)) { 
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
					else if ((grid[from + 17] < 0 && us) || (grid[from + 17] > 0 && !us)) { 
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
					else if ((grid[from - 10] < 0 && us) || (grid[from - 10] > 0 && !us)) { 
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
					else if((grid[from - 17] < 0 && us) || (grid[from - 17] > 0 && !us)) { 
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
					else if((grid[from + 6] < 0 && us) || (grid[from + 6] > 0 && !us)){ 
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
					else if((grid[from + 15] < 0 && us) || (grid[from + 15] > 0 && !us)){ 
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
					else if((grid[from - 6] < 0 && us) || (grid[from - 6] > 0 && !us)){ 
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
					else if((grid[from - 15] < 0 && us) || (grid[from - 15] > 0 && !us)){ 
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
		case -QUEEN:
		case ROOK:
		case -ROOK:
			if (turn == us) {
				for (i = from + NORTH; i >= 0; i += NORTH) {
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((grid[i] < 0 && us) || (grid[i] > 0 && !us)) {
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
						if ((grid[i] < 0 && us) || (grid[i] > 0 && !us)) {
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
						if ((grid[i] < 0 && us) || (grid[i] > 0 && !us)) {
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
						if ((grid[i] < 0 && us) || (grid[i] > 0 && !us)) {
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
		case -BISHOP:
			if (turn == us) {
				for (i = from + NORTHEAST; i % WIDTH > from % WIDTH; i += NORTHEAST) {
					if (i < 0) { break; }
					++threatened[us][i];
					if (!grid[i]) {
						possiblemoves[cmove] = move(from, i, STANDARD);
						++cmove;
					}
					else {
						if ((grid[i] < 0 && us) || (grid[i] > 0 && !us)) {
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
						if ((grid[i] < 0 && us) || (grid[i] > 0 && !us)) {
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
						if ((grid[i] < 0 && us) || (grid[i] > 0 && !us)) {
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
						if ((grid[i] < 0 && us) || (grid[i] > 0 && !us)) {
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

	bool board::checkMate() {//looks for checkmate or draw
		if (checkTeam(!turn) || !cmove) { return true; }
		return false;
	}
	
	bool board::checkTeam(bool team) {//looks for check
		uint8_t to;
		for (to = 0; to < SPACES; ++to) { if ((team && grid[to] == KING) || (!team && grid[to] == -KING)) { break; } }
		if (threatened[!team][to]) { return true; }
		else { return false; }
	}
}