#include "board.h"

namespace Chess {

	board::board() {
		z = zobrist();
		cturn  = 0;
		check  = 0;
		turn   = 1;
		for (uint8_t i =  0; i < MEMORY;	i++) { mHist[i] = move(); vHist[i] = 0; }
		for (uint8_t i =  0; i < SPACES;	i++) { grid[i]  = EMPTY; }
		for (uint8_t i =  0; i < 2 * WIDTH; i++) { grid[i]  = -PAWN; }
		for (uint8_t i = 48; i < SPACES;	i++) { grid[i]  =  PAWN; }
		grid[0]	 = -ROOK;
		grid[1]  = -KNIGHT;
		grid[2]  = -BISHOP;
		grid[3]  = -QUEEN;
		grid[4]  = -KING;
		grid[5]  = -BISHOP;
		grid[6]	 = -KNIGHT;
		grid[7]	 = -ROOK;
		grid[56] = ROOK;
		grid[57] = KNIGHT;
		grid[58] = BISHOP;
		grid[59] = QUEEN;
		grid[60] = KING;
		grid[61] = BISHOP;
		grid[62] = KNIGHT;
		grid[63] = ROOK;
		zkey = z.newKey(this);
	}

	int16_t board::abs(int16_t a) {
		int16_t s = a >> 15;
		a ^= s;
		a -= s;
		return a;
	}

	bool board::movePiece(move m) {
		bool enemy = (turn) ? 0 : 1;
		switch (m.getFlags()) {
		case STANDARD:
			zkey ^= z.pieces[abs(grid[m.getFrom()]) % 100][turn][m.getFrom()];
			zkey ^= z.pieces[abs(grid[m.getFrom()]) % 100][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()]; 
			break;
		case DOUBLEPUSH:
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[PINDEX][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()]; 
			break;
		case KCASTLE:
			zkey ^= z.pieces[KINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[KINDEX][turn][m.getTo()];
			zkey ^= z.pieces[RINDEX][turn][m.getTo()+1];
			zkey ^= z.pieces[RINDEX][turn][m.getTo()-1];
			grid[m.getTo()] = grid[m.getFrom()]; 
			grid[m.getTo() - 1] = grid[m.getTo() + 1]; 
			grid[m.getTo() + 1] = EMPTY; 
			break; 
		case QCASTLE:	
			zkey ^= z.pieces[KINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[KINDEX][turn][m.getTo()];
			zkey ^= z.pieces[RINDEX][turn][m.getTo() - 2];
			zkey ^= z.pieces[RINDEX][turn][m.getTo() + 1];
			grid[m.getTo()] = grid[m.getFrom()]; 
			grid[m.getTo() + 1] = grid[m.getTo() - 2]; 
			grid[m.getTo() - 2] = EMPTY; 
			break; 
		case ENPASSANT:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[PINDEX][enemy][m.getTo()];
			zkey ^= z.pieces[PINDEX][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()]; 
			grid[mHist[cturn - 1].getTo()] = EMPTY; 
			break; 
		case CAPTURE:
			zkey ^= z.pieces[abs(grid[m.getFrom()]) % 100][turn][m.getFrom()];
			zkey ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zkey ^= z.pieces[abs(grid[m.getFrom()]) % 100][turn][m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()]; 
			break; 
		case NPROMOTE:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[NINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT	: -KNIGHT	; 
			break; 
		case BPROMOTE:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			break; 
		case RPROMOTE:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			break; 
		case QPROMOTE:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			break; 
		case NPROMOTEC:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zkey ^= z.pieces[KINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? KNIGHT : -KNIGHT;
			break; 
		case BPROMOTEC:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zkey ^= z.pieces[BINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? BISHOP : -BISHOP;
			break; 
		case RPROMOTEC:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zkey ^= z.pieces[RINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? ROOK : -ROOK;
			break; 
		case QPROMOTEC:	
			zkey ^= z.pieces[PINDEX][turn][m.getFrom()];
			zkey ^= z.pieces[abs(grid[m.getTo()]) % 100][enemy][m.getTo()];
			zkey ^= z.pieces[QINDEX][turn][m.getTo()];
			grid[m.getTo()] = (turn) ? QUEEN : -QUEEN;
			break; 
		case FAIL:		{return false; }
		}
		if (m.getFlags() != NULLMOVE) { grid[m.getFrom()] = EMPTY; }
		for (uint8_t i = 0; i < SPACES; i++) 
			vHist[cturn] += grid[i];
		mHist[cturn] = m;
		cturn++;
		if (checkTurn()) { turn = (turn) ? 0 : 1; unmovePiece(); return false; }
		turn = (turn) ? 0 : 1;
		check = checkTurn();
		return true;
	}

	void board::unmovePiece() {
		turn = (turn) ? 0 : 1;
		cturn--;
		bool enemy = (turn) ? 0 : 1;
		switch (mHist[cturn].getFlags()) {
		case STANDARD:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			zkey ^= z.pieces[abs(grid[mHist[cturn].getFrom()]) % 100][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[abs(grid[mHist[cturn].getFrom()]) % 100][turn][mHist[cturn].getFrom()];
			break;
		case DOUBLEPUSH:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break;
		case KCASTLE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			grid[mHist[cturn].getTo() + 1]= grid[mHist[cturn].getTo() - 1]; 
			grid[mHist[cturn].getTo() - 1] = EMPTY; 
			zkey ^= z.pieces[KINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[KINDEX][turn][mHist[cturn].getFrom()];
			zkey ^= z.pieces[RINDEX][turn][mHist[cturn].getTo() - 1];
			zkey ^= z.pieces[RINDEX][turn][mHist[cturn].getTo() + 1];
			break; 
		case QCASTLE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			grid[mHist[cturn].getTo() - 2]= grid[mHist[cturn].getTo() + 1]; 
			grid[mHist[cturn].getTo() + 1] = EMPTY; 
			zkey ^= z.pieces[KINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[KINDEX][turn][mHist[cturn].getFrom()];
			zkey ^= z.pieces[RINDEX][turn][mHist[cturn].getTo() + 1];
			zkey ^= z.pieces[RINDEX][turn][mHist[cturn].getTo() - 2];
			break; 
		case CAPTURE:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = vHist[cturn - 1] - vHist[cturn];
			zkey ^= z.pieces[abs(grid[mHist[cturn].getFrom()]) % 100][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[abs(grid[mHist[cturn].getTo()]) % 100][enemy][mHist[cturn].getTo()];
			zkey ^= z.pieces[abs(grid[mHist[cturn].getFrom()]) % 100][turn][mHist[cturn].getFrom()];
			break; 
		case ENPASSANT:	
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn - 1].getTo()] = (!turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][enemy][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break; 
		case NPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			zkey ^= z.pieces[KINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break; 
		case BPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			zkey ^= z.pieces[BINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break; 
		case RPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			zkey ^= z.pieces[RINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break; 
		case QPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			zkey ^= z.pieces[QINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break; 
		case NPROMOTEC: 
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + KNIGHT - PAWN : vHist[cturn - 1] - vHist[cturn] - KNIGHT + PAWN;
			zkey ^= z.pieces[KINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[abs(grid[mHist[cturn].getTo()]) % 100][enemy][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break; 
		case BPROMOTEC: 
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + BISHOP - PAWN : vHist[cturn - 1] - vHist[cturn] - BISHOP + PAWN;
			zkey ^= z.pieces[BINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[abs(grid[mHist[cturn].getTo()]) % 100][enemy][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break; 
		case RPROMOTEC: 
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + ROOK - PAWN : vHist[cturn - 1] - vHist[cturn] - ROOK + PAWN;
			zkey ^= z.pieces[RINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[abs(grid[mHist[cturn].getTo()]) % 100][enemy][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break; 
		case QPROMOTEC: 
			grid[mHist[cturn].getFrom()] = (turn) ? PAWN : -PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? vHist[cturn - 1] - vHist[cturn] + QUEEN - PAWN : vHist[cturn - 1] - vHist[cturn] - QUEEN + PAWN;
			zkey ^= z.pieces[QINDEX][turn][mHist[cturn].getTo()];
			zkey ^= z.pieces[abs(grid[mHist[cturn].getTo()]) % 100][enemy][mHist[cturn].getTo()];
			zkey ^= z.pieces[PINDEX][turn][mHist[cturn].getFrom()];
			break;
		}
		check = checkTurn();
		vHist[cturn] = 0;
		mHist[cturn] = move();
	}

	move board::createMove(uint8_t from, uint8_t to) {
		if ((turn && (grid[from] <= 0 || grid[to] > 0)) || (!turn && (grid[from] >= 0 || grid[to] < 0))) { return move(); }
		uint8_t end, i;
		int8_t direction;
		switch (abs(grid[from])) {
		case KING:
			if (abs(from / WIDTH - to / WIDTH) > 1 || abs(from % WIDTH - to % WIDTH) > 2) { return move(); }
			if (abs(from % WIDTH - to % WIDTH) < 2) {
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
			end = (!turn) ? 0 : WIDTH - 1;
			if (from % WIDTH == 4 && from / WIDTH == to / WIDTH && from / WIDTH == end && !check) {
				if (from - 2 == to && !grid[from - 1] && !grid[from - 2] && !grid[from - 3]) {
					for (i = 0; i < cturn; i++) {
						if (mHist[i].getFrom() == from || mHist[i].getFrom() == from - 4 || mHist[i].getTo() == from - 4) { return move(); }
					}
					if (i == cturn) { return move(from, to, QCASTLE); }
				}
				else if (from + 2 == to && !grid[from + 1] && !grid[from + 2]) {
					for (i = 0; i < cturn; i++) {
						if (mHist[i].getFrom() == from || mHist[i].getFrom() == from + 3 || mHist[i].getTo() == from + 3) { return move(); }
					}
					if (i == cturn) { return move(from, to, KCASTLE); }
				}
			}
			return move();
		case PAWN:
			if (abs(to % WIDTH - from % WIDTH) > 1 || abs(to / WIDTH - from / WIDTH) > 2 || to > from&& turn || to < from && !turn) { return move(); }
			direction = (turn) ? NORTH : SOUTH;
			end = (turn) ? 0 : WIDTH - 1;
			if (!grid[from + direction] && to % WIDTH == from % WIDTH) {
				if (from + direction == to) {
					if (end == to / WIDTH) { return move(from, to, QPROMOTE); }
					else { return move(from, to, STANDARD); }
				}
				else if ((from - direction) / WIDTH == WIDTH - 1 - end && !grid[to]) { return move(from, to, DOUBLEPUSH); }
			}
			else if (from % WIDTH && from + direction - 1 == to) {
				if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && mHist[cturn - 1].getTo() == from - 1) { return move(from, to, ENPASSANT); }
				else if (grid[to]) {
					if (end == to / WIDTH) { return move(from, to, QPROMOTEC); }
					else { return move(from, to, CAPTURE); }
				}
			}
			else if (from % WIDTH != WIDTH - 1 && from + direction + 1 == to) {
				if (mHist[cturn - 1].getFlags() == DOUBLEPUSH && mHist[cturn - 1].getTo() == from + 1) { return move(from, to, ENPASSANT); }
				else if (grid[to]) {
					if (end == to / WIDTH) { return move(from, to, QPROMOTEC); }
					else { return move(from, to, CAPTURE); }
				}
			}
			return move();
		case KNIGHT:
			if ((abs(from / WIDTH - to / WIDTH) == 2 && abs(from % WIDTH - to % WIDTH) == 1 || abs(from / WIDTH - to / WIDTH) == 1 && abs(from % WIDTH - to % WIDTH) == 2)) {
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
			return move();
		case QUEEN:
			if (from / WIDTH != to / WIDTH && from % WIDTH != to % WIDTH && abs(from / WIDTH - to / WIDTH) != abs(from % WIDTH - to % WIDTH)) { return move(); }
			if (from / WIDTH == to / WIDTH || from % WIDTH == to % WIDTH) {
				if (from < to) {
					if (from % WIDTH < to % WIDTH) { direction = EAST; }
					else { direction = SOUTH; }
				}
				else if (from % WIDTH > to% WIDTH) { direction = WEST; }
				else { direction = NORTH; }
				for (i = from + direction; i != to; i += direction) {
					if (grid[i]) { return move(); }
				}
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
			else {
				if (from < to) {
					if (from % WIDTH > to% WIDTH) { direction = SOUTHWEST; }
					else { direction = SOUTHEAST; }
				}
				else if (from % WIDTH > to% WIDTH) { direction = NORTHWEST; }
				else { direction = NORTHEAST; }
				for (i = from + direction; i != to; i += direction) {
					if (grid[i]) { return move(); }
				}
				if (grid[to]) { return move(from, to, CAPTURE); }
				else { return move(from, to, STANDARD); }
			}
		case ROOK:
			if (from / WIDTH != to / WIDTH && from % WIDTH != to % WIDTH) { return move(); }
			if (from < to) {
				if (from % WIDTH < to % WIDTH) { direction = EAST; }
				else { direction = SOUTH; }
			}
			else if (from % WIDTH > to% WIDTH) { direction = WEST; }
			else { direction = NORTH; }
			for (i = from + direction; i != to; i += direction) {
				if (grid[i]) { return move(); }
			}
			if (grid[to]) { return move(from, to, CAPTURE); }
			else { return move(from, to, STANDARD); }
		case BISHOP:
			if (abs(from / WIDTH - to / WIDTH) != abs(from % WIDTH - to % WIDTH)) { return move(); }
			if (from < to) {
				if (from % WIDTH > to% WIDTH) { direction = SOUTHWEST; }
				else { direction = SOUTHEAST; }
			}
			else if (from % WIDTH > to% WIDTH) { direction = NORTHWEST; }
			else { direction = NORTHEAST; }
			for (i = from + direction; i != to; i += direction) {
				if (grid[i]) { return move(); }
			}
			if (grid[to]) { return move(from, to, CAPTURE); }
			else { return move(from, to, STANDARD); }
		}
		return move();
	}

	int16_t board::negaEval() {
		int8_t msum = 0;
		for (uint8_t from = 0; from < SPACES; from++) {
			if (grid[from]) { msum = (grid[from] > 0) ? msum + moveTotal(from) : msum - moveTotal(from); }
		}
		return (turn) ? vHist[cturn - 1] + 10*msum : -vHist[cturn - 1] - 10*msum; 
	}

	uint8_t board::moveTotal(uint8_t from) {
		int8_t end, i;
		int8_t direction;
		uint8_t mcount = 0;
		switch (abs(grid[from])) {
		case KING:
			return mcount;
		case PAWN:
			direction = (turn) ? NORTH : SOUTH;
			end = (turn) ? 0 : WIDTH - 1;
			if (!grid[from + direction]) {
				mcount++;
				if ((from - direction) / WIDTH == WIDTH - 1 - end && !grid[from + 2 * direction]) { mcount++; }
			}
			if (from % WIDTH && ((grid[from + direction - 1] > 0 && grid[from] < 0) || (grid[from + direction - 1] < 0 && grid[from] > 0))) { mcount++; }
			if (from % WIDTH != WIDTH - 1 && ((grid[from + direction + 1] > 0 && grid[from] < 0) || (grid[from + direction + 1] < 0 && grid[from] > 0))) { mcount++; }
			if (mHist[cturn - 1].getFlags() == DOUBLEPUSH&&((mHist[cturn - 1].getTo() == from + 1 || mHist[cturn - 1].getTo() == from - 1))) { mcount++; }
			return mcount;
		case KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from + 10 < SPACES){
				if ((grid[from + 10] < 0 && grid[from]>0) || (grid[from + 10] > 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from + 17) % WIDTH > from % WIDTH && from + 17 < SPACES){
				if ((grid[from + 17] < 0 && grid[from]>0) || (grid[from + 17] > 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from - 10) % WIDTH < from % WIDTH && from - 10 > 0){
				if ((grid[from - 10] < 0 && grid[from]>0) || (grid[from - 10] > 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from - 17) % WIDTH < from % WIDTH && from - 17 > 0){
				if ((grid[from - 17] < 0 && grid[from]>0) || (grid[from - 17] > 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from + 6)	% WIDTH > from % WIDTH && from + 6	< SPACES){
				if ((grid[from + 6] < 0 && grid[from]>0) || (grid[from + 6] > 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from + 15) % WIDTH > from % WIDTH && from + 15 < SPACES){
				if ((grid[from + 15] < 0 && grid[from]>0) || (grid[from + 15] > 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from - 6)	% WIDTH < from % WIDTH && from - 6	> 0){
				if ((grid[from - 6] < 0 && grid[from]>0) || (grid[from - 6] > 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from - 15) % WIDTH < from % WIDTH && from - 15 > 0){
				if ((grid[from - 15] < 0 && grid[from]>0) || (grid[from - 15] > 0 && grid[from] < 0)) { mcount++; }
			}
			return mcount;
		case QUEEN:
			for (i = from + NORTH; i > 0; i += NORTH) {
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from]>0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from]>0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from]>0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + WEST; i % WIDTH != WIDTH - 1; i += WEST) {
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from]>0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + NORTHEAST; i % WIDTH > from% WIDTH; i += NORTHEAST) {
				if (i < 0) { break; }
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from] > 0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH; i += NORTHWEST) {
				if (i < 0) { break; }
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from] > 0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + SOUTHEAST; i % WIDTH > from% WIDTH; i += SOUTHEAST) {
				if (i > SPACES) { break; }
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from] > 0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH; i += SOUTHWEST) {
				if (i > SPACES) { break; }
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from] > 0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			return mcount;
		case ROOK:
			for (i = from + NORTH; i > 0; i += NORTH) {
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from]>0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + SOUTH; i < SPACES; i += SOUTH) {
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from]>0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + EAST; i % WIDTH; i += EAST) {
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from]>0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + WEST; i % WIDTH != WIDTH - 1; i += WEST) {
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from]>0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			return mcount;
		case BISHOP:
			for (i = from + NORTHEAST; i % WIDTH > from % WIDTH; i += NORTHEAST) {
				if (i < 0) { break; }
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from] > 0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + NORTHWEST; i % WIDTH < from % WIDTH; i += NORTHWEST) {
				if (i < 0) { break; }
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from] > 0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + SOUTHEAST; i % WIDTH > from % WIDTH; i += SOUTHEAST) {
				if (i > SPACES) { break; }
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from] > 0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			for (i = from + SOUTHWEST; i % WIDTH < from % WIDTH; i += SOUTHWEST) {
				if (i > SPACES) { break; }
				if (grid[i]) {
					if ((grid[i] < 0 && grid[from] > 0) || (grid[i] > 0 && grid[from] < 0)) { mcount++; }
					break;
				}
				else { mcount++; }
			}
			return mcount;
		}
		return mcount;
	}

	bool board::checkMate() {
		for (uint8_t from = 0; from < SPACES; from++) {
			if ((grid[from] > 0 && turn) || (grid[from] < 0 && !turn)) {
				for (uint8_t to = 0; to < SPACES; to++) {
					move m = createMove(from, to);
					if (movePiece(m)) { unmovePiece(); return false; }
				}
			}
		}
		return true;
	}
	
	bool board::checkTurn() {
		uint8_t to;
		for (to = 0; to < SPACES; to++) { if ((turn && grid[to] == KING) || (!turn && grid[to] == -KING)) { break; } }
		turn = (turn) ? 0 : 1;
		for (uint8_t from = 0; from < SPACES; from++) {
			if (createMove(from, to).getFlags() != FAIL) { turn = (turn) ? 0 : 1; return true; }
		}
		turn = (turn) ? 0 : 1;
		return false;
	}
}