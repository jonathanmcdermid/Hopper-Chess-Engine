#include "board.h"

namespace Chess {

	board::board() {//sets board to starting state
		z = zobrist();
		fenSet(STARTFEN);
	}

	void board::fenSet(std::string fs) {//sets board to state outlined in FEN string, no 50 move rule implementation
		for (uint8_t i = 0; i < MEMORY; ++i) { mHist[i] = move(); vHist[i] = 0; zHist[i] = 0; }
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
		check = checkTeam(turn);
		++index;
		zHist[cturn] = z.newKey(this);
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
		allAttacked();
		if (checkTeam(turn)) { 
			turn = (turn) ? BLACK : WHITE;
			unmovePiece();
			return false; 
		}
		else {
			turn = (turn) ? BLACK : WHITE;
			check = checkTeam(turn);
			return true;
		}
	}

	void board::unmovePiece() {//unmakes a move
		turn = (turn) ? BLACK : WHITE;
		zHist[cturn] = 0;
		--cturn;
		bool enemy = (turn) ? BLACK : WHITE;
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
		allAttacked();
		check = checkTeam(turn);
		vHist[cturn] = 0;
		mHist[cturn] = move();
	}

	move board::createMove(uint8_t from, uint8_t to) {//generates a pseudo legal move from start and endpoint (cannot create nullmove)
		if ((turn && (grid[from] <= 0 || grid[to] > 0)) || (!turn && (grid[from] >= 0 || grid[to] < 0))) { return move(); }
		uint8_t i;
		int8_t direction;
		switch (grid[from]) {
		case KING:
			if (abs(from / WIDTH - to / WIDTH) > 1 || abs(from % WIDTH - to % WIDTH) > 2 || threatened[BLACK][to]) { return move(); }
			if (abs(from % WIDTH - to % WIDTH) < 2) {
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
			if (from == 60 && !check) {
				if (to == 58 && !grid[59] && !grid[58] && !grid[57]) {
					for (i = 0; i < cturn; ++i) {
						if (mHist[i].getFrom() == 60 || mHist[i].getFrom() == 56 || mHist[i].getTo() == 56) { return move(); }
					}
					return move(from, to, QCASTLE);
				}
				else if (to == 63 && !grid[61] && !grid[62]) {
					for (i = 0; i < cturn; ++i) {
						if (mHist[i].getFrom() == 60 || mHist[i].getFrom() == 63 || mHist[i].getTo() == 63) { return move(); }
					}
					return move(from, to, KCASTLE);
				}
			}
			return move();
		case -KING:
			if (abs(from / WIDTH - to / WIDTH) > 1 || abs(from % WIDTH - to % WIDTH) > 2 || threatened[WHITE][to]) { return move(); }
			if (abs(from % WIDTH - to % WIDTH) < 2) {
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
			if (from == 4 && !check) {
				if (to == 2 && !grid[3] && !grid[2] && !grid[1]) {
					for (i = 0; i < cturn; ++i) {
						if (mHist[i].getFrom() == 4 || mHist[i].getFrom() == 0 || mHist[i].getTo() == 0) { return move(); }
					}
					return move(from, to, QCASTLE);
				}
				else if (to == 6 && !grid[5] && !grid[6]) {
					for (i = 0; i < cturn; ++i) {
						if (mHist[i].getFrom() == 4 || mHist[i].getFrom() == 7 || mHist[i].getTo() == 7) { return move(); }
					}
					return move(from, to, KCASTLE);
				}
			}
			return move();
		case PAWN:
			if (abs(to % WIDTH - from % WIDTH) > 1 || abs(to / WIDTH - from / WIDTH) > 2 || to > from) { return move(); }
			if (!grid[from + NORTH] && to % WIDTH == from % WIDTH) {
				if (from + NORTH == to) {
					if (to < WIDTH) { return move(from, to, QPROMOTE); }
					else { return move(from, to, STANDARD); }
				}
				else if (from > 47 && !grid[to]) { return move(from, to, DOUBLEPUSH); }
			}
			else if (from % WIDTH && from + NORTHWEST == to) {
				if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && mHist[cturn - 1].getTo() == from + WEST) { return move(from, to, ENPASSANT); }
				else if (grid[to]) {
					if (to < WIDTH) { return move(from, to, QPROMOTEC); }
					else { return move(from, to, CAPTURE); }
				}
			}
			else if (from % WIDTH != 7 && from + NORTHEAST == to) {
				if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && mHist[cturn - 1].getTo() == from + EAST) { return move(from, to, ENPASSANT); }
				else if (grid[to]) {
					if (to < WIDTH) { return move(from, to, QPROMOTEC); }
					else { return move(from, to, CAPTURE); }
				}
			}
			return move();
		case -PAWN:
			if (abs(to % WIDTH - from % WIDTH) > 1 || abs(to / WIDTH - from / WIDTH) > 2 || to < from) { return move(); }
			if (!grid[from + SOUTH] && to % WIDTH == from % WIDTH) {
				if (from + SOUTH == to) {
					if (to > 55) { return move(from, to, QPROMOTE); }
					else { return move(from, to, STANDARD); }
				}
				else if ( from < 16 && !grid[to]) { return move(from, to, DOUBLEPUSH); }
			}
			else if (from % WIDTH && from + SOUTHWEST == to) {
				if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && mHist[cturn - 1].getTo() == from + WEST) { return move(from, to, ENPASSANT); }
				else if (grid[to]) {
					if (to > 55) { return move(from, to, QPROMOTEC); }
					else { return move(from, to, CAPTURE); }
				}
			}
			else if (from % WIDTH != 7 && from + SOUTHEAST == to) {
				if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && mHist[cturn - 1].getTo() == from + EAST) { return move(from, to, ENPASSANT); }
				else if (grid[to]) {
					if (to > 55) { return move(from, to, QPROMOTEC); }
					else { return move(from, to, CAPTURE); }
				}
			}
			return move();
		case KNIGHT:
		case -KNIGHT:
			if ((abs(from / WIDTH - to / WIDTH) == 2 && abs(from % WIDTH - to % WIDTH) == 1 || abs(from / WIDTH - to / WIDTH) == 1 && abs(from % WIDTH - to % WIDTH) == 2)) {
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
			return move();
		case QUEEN:
		case -QUEEN:
			if (from / WIDTH != to / WIDTH && from % WIDTH != to % WIDTH && abs(from / WIDTH - to / WIDTH) != abs(from % WIDTH - to % WIDTH)) { return move(); }
			if (from / WIDTH == to / WIDTH || from % WIDTH == to % WIDTH) {
				if (from < to) {
					if (from % WIDTH < to % WIDTH) { direction = EAST; }
					else { direction = SOUTH; }
				}
				else if (from % WIDTH > to % WIDTH) { direction = WEST; }
				else { direction = NORTH; }
				for (i = from + direction; i != to; i += direction) {
					if (grid[i]) { return move(); }
				}
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
			else {
				if (from < to) {
					if (from % WIDTH > to % WIDTH) { direction = SOUTHWEST; }
					else { direction = SOUTHEAST; }
				}
				else if (from % WIDTH > to % WIDTH) { direction = NORTHWEST; }
				else { direction = NORTHEAST; }
				for (i = from + direction; i != to; i += direction) {
					if (grid[i]) { return move(); }
				}
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
		case ROOK:
		case -ROOK:
			if (from / WIDTH != to / WIDTH && from % WIDTH != to % WIDTH) { return move(); }
			if (from < to) {
				if (from % WIDTH < to % WIDTH) { direction = EAST; }
				else { direction = SOUTH; }
			}
			else if (from % WIDTH > to % WIDTH) { direction = WEST; }
			else { direction = NORTH; }
			for (i = from + direction; i != to; i += direction) {
				if (grid[i]) { return move(); }
			}
			if (grid[to]) { return move(from, to, CAPTURE); }
			else { return move(from, to, STANDARD); }
		case BISHOP:
		case -BISHOP:
			if (abs(from / WIDTH - to / WIDTH) != abs(from % WIDTH - to % WIDTH)) { return move(); }
			if (from < to) {
				if (from % WIDTH > to % WIDTH) { direction = SOUTHWEST; }
				else { direction = SOUTHEAST; }
			}
			else if (from % WIDTH > to % WIDTH) { direction = NORTHWEST; }
			else { direction = NORTHEAST; }
			for (i = from + direction; i != to; i += direction) {
				if (grid[i]) { return move(); }
			}
			if (grid[to]) { return move(from, to, CAPTURE); }
			else { return move(from, to, STANDARD); }
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

	void board::allAttacked() {
		for (uint8_t i = 0; i < SPACES; ++i) {
			threatened[WHITE][i] = 0;
			threatened[BLACK][i] = 0;
		}
		for (uint8_t from = 0; from < SPACES; ++from) {
			if (grid[from]) { threatenedSquares(from); }
		}
	}

	void board::threatenedSquares(uint8_t from) {//estimates offensive mobility of one piece, omits king moves for performance
		int8_t i;
		bool us = (grid[from] > 0) ? WHITE : BLACK;
		switch (grid[from]) {
		case KING:
		case -KING:
			return;
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
			if (from % WIDTH) { ++threatened[WHITE][from + NORTHWEST]; }
			if (from % WIDTH != 7) { ++threatened[WHITE][from + NORTHEAST]; }
			return;
		case -PAWN:
			if (from % WIDTH) { ++threatened[BLACK][from + SOUTHWEST]; }
			if (from % WIDTH != 7) { ++threatened[BLACK][from + SOUTHEAST]; }
			return;
		case KNIGHT:
		case -KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54) { ++threatened[us][from + 10]; }
			if ((from + 17) % WIDTH > from % WIDTH && from < 47) { ++threatened[us][from + 17]; }
			if ((from - 10) % WIDTH < from % WIDTH && from > 9) { ++threatened[us][from - 10]; }
			if ((from - 17) % WIDTH < from % WIDTH && from > 16) { ++threatened[us][from - 17]; }
			if ((from + 6) % WIDTH < from % WIDTH && from < 58) { ++threatened[us][from + 6]; }
			if ((from + 15) % WIDTH < from % WIDTH && from < 49) { ++threatened[us][from + 15]; }
			if ((from - 6) % WIDTH > from % WIDTH && from > 5) { ++threatened[us][from - 6]; }
			if ((from - 15) % WIDTH > from % WIDTH && from > 14) { ++threatened[us][from - 15]; }
			return;
		case QUEEN:
		case -QUEEN:
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
			return;
		case ROOK:
		case -ROOK:
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
			return;
		case BISHOP:
		case -BISHOP:
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
			return;
		}
	}

	bool board::checkMate() {//looks for checkmate or draw
		for (uint8_t from = 0; from < SPACES; ++from) {
			if ((grid[from] > 0 && turn) || (grid[from] < 0 && !turn)) {
				for (uint8_t to = 0; to < SPACES; ++to) {
					move m = createMove(from, to);
					if (movePiece(m)) { unmovePiece(); return false; }
				}
			}
		}
		return true;
	}
	
	bool board::checkTeam(bool team) {//looks for check
		uint8_t to;
		bool enemy = (team) ? BLACK : WHITE;
		for (to = 0; to < SPACES; ++to) { if ((team && grid[to] == KING) || (!team && grid[to] == -KING)) { break; } }
		if (threatened[enemy][to]) { return true; }
		else { return false; }
	}
}