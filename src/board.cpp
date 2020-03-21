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
			case 'P': { grid[counter] = PAWN; counter++; break; }
			case 'R': { grid[counter] = ROOK; counter++; break; }
			case 'N': { grid[counter] = KNIGHT; counter++; break; }
			case 'B': { grid[counter] = BISHOP; counter++; break; }
			case 'Q': { grid[counter] = QUEEN; counter++; break; }
			case 'K': { grid[counter] = KING; counter++; break; }
			case 'p': { grid[counter] = -PAWN; counter++; break; }
			case 'r': { grid[counter] = -ROOK; counter++; break; }
			case 'n': { grid[counter] = -KNIGHT; counter++; break; }
			case 'b': { grid[counter] = -BISHOP; counter++; break; }
			case 'q': { grid[counter] = -QUEEN; counter++; break; }
			case 'k': { grid[counter] = -KING; counter++; break; }
			case '/': { break; }
			default:
				helper = fs[index] - '0';
				while (helper) {
					grid[counter] = EMPTY;
					counter++;
					helper--;
				}
			}
			index++;
		}
		index++;
		turn = (fs[index] == 'w') ? WHITE : BLACK;
		index++;
		bool castled[4] = { true,true,true,true };
		do{
			switch (fs[index]) {
			case 'K': {castled[3] = false; break; }
			case 'Q': {castled[2] = false; break; }
			case 'k': {castled[1] = false; break; }
			case 'q': {castled[0] = false; break; }
			}
			index++;
		} while (fs[index] != ' ');
		if (castled[0]) { mHist[cturn] = move(0, 0, NULLMOVE); cturn++; }
		if (castled[1]) { mHist[cturn] = move(WIDTH-1, 0, NULLMOVE); cturn++; }
		if (castled[2]) { mHist[cturn] = move(SPACES - WIDTH, 0, NULLMOVE); cturn++; }
		if (castled[3]) { mHist[cturn] = move(SPACES - 1, 0, NULLMOVE); cturn++; }
		index++;
		if(fs[index]!='-'){
			uint8_t from = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
			uint8_t to   = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			mHist[cturn] = move(from, to, DOUBLEPUSH);
			cturn++;
		}
		check = checkTurn();
		index++;
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
		cturn++;
		if (checkTurn()) { 
			turn = (turn) ? BLACK : WHITE;
			unmovePiece();
			return false; 
		}
		else {
			turn = (turn) ? BLACK : WHITE;
			check = checkTurn();
			return true;
		}
	}

	void board::unmovePiece() {//unmakes a move
		turn = (turn) ? BLACK : WHITE;
		zHist[cturn] = 0;
		cturn--;
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
		check = checkTurn();
		vHist[cturn] = 0;
		mHist[cturn] = move();
	}

	move board::createMove(uint8_t from, uint8_t to) {//generates a pseudo legal move from start and endpoint (cannot create nullmove)
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
					for (i = 0; i < cturn; ++i) {
						if (mHist[i].getFrom() == from || mHist[i].getFrom() == from - 4 || mHist[i].getTo() == from - 4) { return move(); }
					}
					if (i == cturn) { return move(from, to, QCASTLE); }
				}
				else if (from + 2 == to && !grid[from + 1] && !grid[from + 2]) {
					for (i = 0; i < cturn; ++i) {
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

	int16_t board::negaEval() {//evaluates for negamax function, uses material weight and estimated mobility
		int8_t msum = 0;
		for (uint8_t from = 0; from < SPACES; ++from) {
			if (grid[from]) { msum = (grid[from] > 0) ? msum + moveTotal(from) : msum - moveTotal(from); }
		}
		return (turn) ? vHist[cturn - 1] + 10*msum : -vHist[cturn - 1] - 10*msum; 
	}

	uint8_t board::moveTotal(uint8_t from) {//estimates mobility of one piece, omits king moves
		int8_t end, i;
		int8_t direction;
		uint8_t mcount = 0;
		switch (abs(grid[from])) {
		case KING:
			return mcount;
		case PAWN:
			direction = (grid[from] > 0) ? NORTH : SOUTH;
			end = (grid[from] > 0) ? 0 : WIDTH - 1;
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
				if ((grid[from + 10] <= 0 && grid[from] > 0) || (grid[from + 10] >= 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from + 17) % WIDTH > from % WIDTH && from + 17 < SPACES){
				if ((grid[from + 17] <= 0 && grid[from] > 0) || (grid[from + 17] >= 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from - 10) % WIDTH < from % WIDTH && from - 10 > 0){
				if ((grid[from - 10] <= 0 && grid[from] > 0) || (grid[from - 10] >= 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from - 17) % WIDTH < from % WIDTH && from - 17 > 0){
				if ((grid[from - 17] <= 0 && grid[from] > 0) || (grid[from - 17] >= 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from + 6)	% WIDTH > from % WIDTH && from + 6	< SPACES){
				if ((grid[from + 6] <= 0 && grid[from] > 0) || (grid[from + 6] >= 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from + 15) % WIDTH < from % WIDTH && from + 15 < SPACES){
				if ((grid[from + 15] <= 0 && grid[from] > 0) || (grid[from + 15] >= 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from - 6)	% WIDTH < from % WIDTH && from - 6	> 0){
				if ((grid[from - 6] <= 0 && grid[from] > 0) || (grid[from - 6] >= 0 && grid[from] < 0)) { mcount++; }
			}
			if ((from - 15) % WIDTH > from % WIDTH && from - 15 > 0){
				if ((grid[from - 15] <= 0 && grid[from] > 0) || (grid[from - 15] >= 0 && grid[from] < 0)) { mcount++; }
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
	
	bool board::checkTurn() {//looks for check
		uint8_t to;
		for (to = 0; to < SPACES; ++to) { if ((turn && grid[to] == KING) || (!turn && grid[to] == -KING)) { break; } }
		turn = (turn) ? BLACK : WHITE;
		for (uint8_t from = 0; from < SPACES; ++from) {
			if (createMove(from, to).getFlags() != FAIL) { turn = (turn) ? 0 : 1; return true; }
		}
		turn = (turn) ? BLACK : WHITE;
		return false;
	}
}