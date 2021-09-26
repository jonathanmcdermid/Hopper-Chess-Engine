#include "Board.h"
#include <cmath>

namespace Hopper {

	Board::Board() 
	{
		fenSet((const char*)STARTFEN);
	}

	void Board::fenSet(const char* fs) 
	{//sets board to state outlined in FEN string
		hist.push_back(historyInfo(0,0,0,0,0,NULLMOVE));
		for (int i = PINDEX; i < KINDEX * 2; ++i) {
			roles[i] = 0;
			pins[i] = 0;
		}
		int index = 0, counter = 0, helper;
		while (fs[index] != ' ') {
			switch (fs[index]) {
			case 'P':  
				grid[counter++] = W_PAWN; 
				++roles[KINDEX + PINDEX];
				break; 
			case 'R': 
				grid[counter++] = W_ROOK;
				++roles[KINDEX + RINDEX];
				break;
			case 'N':  
				grid[counter++] = W_KNIGHT; 
				++roles[KINDEX + NINDEX];
				break;
			case 'B':  
				grid[counter++] = W_BISHOP; 
				++roles[KINDEX + BINDEX];
				break;
			case 'Q':  
				grid[counter++] = W_QUEEN; 
				++roles[KINDEX + QINDEX];
				break;
			case 'K':  
				kpos[WHITE] = counter;
				grid[counter++] = W_KING; 
				break; 
			case 'p':  
				grid[counter++] = B_PAWN; 
				++roles[PINDEX];
				break;
			case 'r':  
				grid[counter++] = B_ROOK; 
				++roles[RINDEX];
				break;
			case 'n':  
				grid[counter++] = B_KNIGHT; 
				++roles[NINDEX];
				break;
			case 'b':
				grid[counter++] = B_BISHOP; 
				++roles[BINDEX];
				break;
			case 'q':  
				grid[counter++] = B_QUEEN; 
				++roles[QINDEX];
				break;
			case 'k':  
				kpos[BLACK] = counter;
				grid[counter++] = B_KING;
				break; 
			case '/':  
				break; 
			default:
				helper = fs[index] - '0';
				while (helper--) { grid[counter++] = EMPTY; }
			}
			++index;
		}
		turn = (side_enum) (fs[++index] == 'w');
		++index;
		hist.back().cHist = 0;
		do {
			switch (fs[index++]) {
			case 'K': 
				hist.back().cHist |= 1 << 0; 
				break;
			case 'Q': 
				hist.back().cHist |= 1 << 1; 
				break;
			case 'k': 
				hist.back().cHist |= 1 << 2; 
				break;
			case 'q': 
				hist.back().cHist |= 1 << 3; 
				break;
			}
		} while (fs[index] != ' ');
		if (fs[++index] != '-') {
			int from = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
			int to = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			hist.back().mHist = Move(from, to, DOUBLEPUSH);
		}
		index += 2;
		hist.back().fHist = fs[index] - '0';
		hist.back().vHist = 0;
		for (int i = 0; i < SPACES; ++i) 
		{ 
			hist.back().vHist += grid[i]; 
		}
		hist.back().zHist = z.newKey(this);
		hist.back().pHist = z.newPawnKey(this);
		allThreats();
	}

	bool Board::isCheckMate() 
	{
		Move m[28];
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

	bool Board::isRepititionDraw() 
	{
		if (hist.back().fHist >= 100) { return true; }
		bool once = false;
		for (int i = 4; i < hist.back().fHist; i += 4) {
			if (hist[hist.size() - i - 1].zHist == hist.back().zHist) {
				if (once) { 
					return true; 
				}
				else { 
					once = true; 
				}
			}
		}
		return false;
	}

	bool Board::isMaterialDraw()
	{
		if (roles[KINDEX + PINDEX] || roles[PINDEX] || roles[KINDEX + QINDEX] || roles[QINDEX]) { return false; }
		int helper;
		for (int i = 0; i < 2; ++i) {
			helper = 0;
			for (int j = NINDEX; j < QINDEX; ++j)
				helper += j * roles[i + j];
			if (helper > BINDEX)
				return false;
		}
		return true;
	}

	bool Board::isEndgame() 
	{
		for (int i = 0; i < 2; ++i) {
			if (roles[i + QINDEX]) {
				for (int j = NINDEX; j < QINDEX; ++j) {
					if (roles[i + j]) 
						return false;
				}
				if (roles[i + QINDEX] > 1) 
					return false;
			}
		}
		return true;
	}

	void Board::movePiece(Move m)
	{//executes a move if legal
		hist.push_back(historyInfo(
			(m.getFlags() == STANDARD && abs(grid[m.getFrom()]) != W_PAWN) ? hist.back().fHist + 1 : 0,
			hist.back().cHist,
			hist.back().vHist,
			(hist.back().mHist.getFlags() == DOUBLEPUSH) ? hist.back().zHist ^ z.sideAt() ^ z.enPassantAt(hist.back().mHist.getTo()) : hist.back().zHist ^ z.sideAt(),
			hist.back().pHist,
			m
		));
		switch (m.getFlags()) {
		case STANDARD:
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getFrom()]) % 10, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getFrom()]) % 10, turn, m.getTo());
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			if (abs(grid[m.getTo()]) == W_KING) { 
				kpos[turn] = m.getTo(); 
			}
			else if (abs(grid[m.getTo()]) == W_PAWN) {
				hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
				hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getTo());
			}
			break;
		case DOUBLEPUSH:
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getTo());
			hist.back().zHist ^= z.enPassantAt(m.getTo());
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getTo());
			break;
		case KCASTLE:
			hist.back().zHist ^= z.piecesAt(KINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(KINDEX, turn, m.getTo());
			hist.back().zHist ^= z.piecesAt(RINDEX, turn, m.getTo() + 1);
			hist.back().zHist ^= z.piecesAt(RINDEX, turn, m.getTo() - 1);
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() - 1] = grid[m.getTo() + 1];
			grid[m.getTo() + 1] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			kpos[turn] = m.getTo();
			break;
		case QCASTLE:
			hist.back().zHist ^= z.piecesAt(KINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(KINDEX, turn, m.getTo());
			hist.back().zHist ^= z.piecesAt(RINDEX, turn, m.getTo() - 2);
			hist.back().zHist ^= z.piecesAt(RINDEX, turn, m.getTo() + 1);
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() + 1] = grid[m.getTo() - 2];
			grid[m.getTo() - 2] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			kpos[turn] = m.getTo();
			break;
		case CAPTURE:
			--roles[!turn * KINDEX + abs(grid[m.getTo()]) % 10];
			if (abs(grid[m.getTo()]) == W_PAWN) { hist.back().pHist ^= z.piecesAt(PINDEX, !turn, m.getTo()); }
			hist.back().vHist -= grid[m.getTo()];
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getFrom()]) % 10, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getFrom()]) % 10, turn, m.getTo());
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			if (abs(grid[m.getTo()]) == W_PAWN) {
				hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
				hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getTo());
			}
			else if (abs(grid[m.getTo()]) == W_KING) { kpos[turn] = m.getTo(); }
			break;
		case ENPASSANT:
			--roles[!turn * KINDEX + PINDEX];
			hist.back().vHist += (turn) ? W_PAWN : B_PAWN;
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getTo());
			hist.back().zHist ^= (turn) ? z.piecesAt(PINDEX, BLACK, m.getTo() + BOARD_SOUTH) : z.piecesAt(PINDEX, WHITE, m.getTo() + BOARD_NORTH);
			grid[m.getTo()] = grid[m.getFrom()];
			(turn) ? grid[m.getTo() + BOARD_SOUTH] = EMPTY : grid[m.getTo() + BOARD_NORTH] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getTo());
			hist.back().pHist ^= (turn) ? z.piecesAt(PINDEX, BLACK, m.getTo() + BOARD_SOUTH) : z.piecesAt(PINDEX, WHITE, m.getTo() + BOARD_NORTH);
			break;
		case NPROMOTE:
			--roles[turn * KINDEX + PINDEX];
			++roles[turn * KINDEX + NINDEX];
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().vHist += (turn) ? B_PAWN + W_KNIGHT : W_PAWN + B_KNIGHT;
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(NINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_KNIGHT : B_KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTE:
			--roles[turn * KINDEX + PINDEX];
			++roles[turn * KINDEX + BINDEX];
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().vHist += (turn) ? B_PAWN + W_BISHOP : W_PAWN + B_BISHOP;
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(BINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_BISHOP : B_BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTE:
			--roles[turn * KINDEX + PINDEX];
			++roles[turn * KINDEX + RINDEX];
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().vHist += (turn) ? B_PAWN + W_ROOK : W_PAWN + B_ROOK;
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(RINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_ROOK : B_ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTE:
			--roles[turn * KINDEX + PINDEX];
			++roles[turn * KINDEX + QINDEX];
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().vHist += (turn) ? B_PAWN + W_QUEEN : W_PAWN - W_QUEEN;
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(QINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_QUEEN : B_QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		case NPROMOTEC:
			--roles[!turn * KINDEX + abs(grid[m.getTo()]) % 10];
			--roles[turn * KINDEX + PINDEX];
			++roles[turn * KINDEX + NINDEX];
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().vHist += (turn) ? B_PAWN + W_KNIGHT - grid[m.getTo()] : W_PAWN + B_KNIGHT - grid[m.getTo()];
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			hist.back().zHist ^= z.piecesAt(NINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_KNIGHT : B_KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			--roles[!turn * KINDEX + abs(grid[m.getTo()]) % 10];
			--roles[turn * KINDEX + PINDEX];
			++roles[turn * KINDEX + BINDEX];
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().vHist += (turn) ? B_PAWN + W_BISHOP - grid[m.getTo()] : W_PAWN + B_BISHOP - grid[m.getTo()];
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			hist.back().zHist ^= z.piecesAt(BINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_BISHOP : B_BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			--roles[!turn * KINDEX + abs(grid[m.getTo()]) % 10];
			--roles[turn * KINDEX + PINDEX];
			++roles[turn * KINDEX + RINDEX];
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().vHist += (turn) ? B_PAWN + W_ROOK - grid[m.getTo()] : W_PAWN + B_ROOK - grid[m.getTo()];
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			hist.back().zHist ^= z.piecesAt(RINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_ROOK : B_ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			--roles[!turn * KINDEX + abs(grid[m.getTo()]) % 10];
			--roles[turn * KINDEX + PINDEX];
			++roles[turn * KINDEX + QINDEX];
			hist.back().pHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().vHist += (turn) ? B_PAWN + W_QUEEN - grid[m.getTo()] : W_PAWN - W_QUEEN - grid[m.getTo()];
			hist.back().zHist ^= z.piecesAt(PINDEX, turn, m.getFrom());
			hist.back().zHist ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			hist.back().zHist ^= z.piecesAt(QINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_QUEEN : B_QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		}
		if (hist.back().cHist & 1 << 0 && (m.getTo() == 63 || m.getFrom() == 63)) {
			hist.back().cHist &= ~(1 << 0);
			hist.back().zHist ^= z.castleAt(WKINGSIDE);
		}
		if (hist.back().cHist & 1 << 1 && (m.getTo() == 56 || m.getFrom() == 56)) {
			hist.back().cHist &= ~(1 << 1);
			hist.back().zHist ^= z.castleAt(WQUEENSIDE);
		}
		if (m.getFrom() == 60) {
			if (hist.back().cHist & 1 << 0) { 
				hist.back().cHist &= ~(1 << 0); 
				hist.back().zHist ^= z.castleAt(WKINGSIDE); 
			}
			if (hist.back().cHist & 1 << 1) { 
				hist.back().cHist &= ~(1 << 1); 
				hist.back().zHist ^= z.castleAt(WQUEENSIDE); 
			}
		}
		if (hist.back().cHist & 1 << 2 && (m.getTo() == 7 || m.getFrom() == 7)) {
			hist.back().cHist &= ~(1 << 2);
			hist.back().zHist ^= z.castleAt(BKINGSIDE);
		}
		if (hist.back().cHist & 1 << 3 && (m.getTo() == 0 || m.getFrom() == 0)) {
			hist.back().cHist &= ~(1 << 3);
			hist.back().zHist ^= z.castleAt(BQUEENSIDE);
		}
		if (m.getFrom() == 4) {
			if (hist.back().cHist & 1 << 2) {
				hist.back().cHist &= ~(1 << 2); 
				hist.back().zHist ^= z.castleAt(BKINGSIDE); 
			}
			if (hist.back().cHist & 1 << 3) { 
				hist.back().cHist &= ~(1 << 3); 
				hist.back().zHist ^= z.castleAt(BQUEENSIDE); 
			}
		}
		turn = (side_enum)(!turn);
		allThreats();
	}

	void Board::unmovePiece() 
	{//unmakes a move
		turn = (side_enum)(!turn);
		switch (hist.back().mHist.getFlags()) {
		case STANDARD:
			grid[hist.back().mHist.getFrom()] = grid[hist.back().mHist.getTo()];
			grid[hist.back().mHist.getTo()] = EMPTY;
			if (abs(grid[hist.back().mHist.getFrom()]) == W_KING) { kpos[turn] = hist.back().mHist.getFrom(); }
			break;
		case DOUBLEPUSH:
			grid[hist.back().mHist.getFrom()] = grid[hist.back().mHist.getTo()];
			grid[hist.back().mHist.getTo()] = EMPTY;
			break;
		case KCASTLE:
			grid[hist.back().mHist.getFrom()] = grid[hist.back().mHist.getTo()];
			grid[hist.back().mHist.getTo()] = EMPTY;
			grid[hist.back().mHist.getTo() + 1] = grid[hist.back().mHist.getTo() - 1];
			grid[hist.back().mHist.getTo() - 1] = EMPTY;
			kpos[turn] = hist.back().mHist.getFrom();
			break;
		case QCASTLE:
			grid[hist.back().mHist.getFrom()] = grid[hist.back().mHist.getTo()];
			grid[hist.back().mHist.getTo()] = EMPTY;
			grid[hist.back().mHist.getTo() - 2] = grid[hist.back().mHist.getTo() + 1];
			grid[hist.back().mHist.getTo() + 1] = EMPTY;
			kpos[turn] = hist.back().mHist.getFrom();
			break;
		case CAPTURE:
			grid[hist.back().mHist.getFrom()] = grid[hist.back().mHist.getTo()];
			grid[hist.back().mHist.getTo()] = (role_enum) (hist[hist.size() - 2].vHist - hist.back().vHist);
			if (abs(grid[hist.back().mHist.getFrom()]) == W_KING) { kpos[turn] = hist.back().mHist.getFrom(); }
			++roles[!turn * KINDEX + abs(grid[hist.back().mHist.getTo()]) % 10];
			break;
		case ENPASSANT:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = EMPTY;
			if (turn) {
				grid[hist.back().mHist.getTo() + 8] = B_PAWN;
			}
			else {
				grid[hist.back().mHist.getTo() - 8] = W_PAWN;
			}
			++roles[!turn * KINDEX + PINDEX];
			break;
		case NPROMOTE:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = EMPTY;
			++roles[turn * KINDEX + PINDEX];
			--roles[turn * KINDEX + NINDEX];
			break;
		case BPROMOTE:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = EMPTY;
			++roles[turn * KINDEX + PINDEX];
			--roles[turn * KINDEX + BINDEX];
			break;
		case RPROMOTE:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = EMPTY;
			++roles[turn * KINDEX + PINDEX];
			--roles[turn * KINDEX + RINDEX];
			break;
		case QPROMOTE:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = EMPTY;
			++roles[turn * KINDEX + PINDEX];
			--roles[turn * KINDEX + QINDEX];
			break;
		case NPROMOTEC:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = (turn) ? (role_enum)( hist[hist.size() - 2].vHist - hist.back().vHist + W_KNIGHT - W_PAWN) : (role_enum)(hist[hist.size() - 2].vHist - hist.back().vHist + B_KNIGHT + W_PAWN);
			++roles[!turn * KINDEX + abs(grid[hist.back().mHist.getTo()]) % 10];
			++roles[turn * KINDEX + PINDEX];
			--roles[turn * KINDEX + NINDEX];
			break;
		case BPROMOTEC:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = (turn) ? (role_enum) (hist[hist.size() - 2].vHist - hist.back().vHist + W_BISHOP - W_PAWN) : (role_enum)(hist[hist.size() - 2].vHist - hist.back().vHist + B_BISHOP + W_PAWN);
			++roles[!turn * KINDEX + abs(grid[hist.back().mHist.getTo()]) % 10];
			++roles[turn * KINDEX + PINDEX];
			--roles[turn * KINDEX + BINDEX];
			break;
		case RPROMOTEC:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = (turn) ? (role_enum)(hist[hist.size() - 2].vHist - hist.back().vHist + W_ROOK - W_PAWN) : (role_enum)(hist[hist.size() - 2].vHist - hist.back().vHist + B_ROOK + W_PAWN);
			++roles[!turn * KINDEX + abs(grid[hist.back().mHist.getTo()]) % 10];
			++roles[turn * KINDEX + PINDEX];
			--roles[turn * KINDEX + RINDEX];
			break;
		case QPROMOTEC:
			grid[hist.back().mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[hist.back().mHist.getTo()] = (turn) ? (role_enum)(hist[hist.size() - 2].vHist - hist.back().vHist + W_QUEEN - W_PAWN) : (role_enum)(hist[hist.size() - 2].vHist - hist.back().vHist - W_QUEEN + W_PAWN);
			++roles[!turn * KINDEX + abs(grid[hist.back().mHist.getTo()]) % 10];
			++roles[turn * KINDEX + PINDEX];
			--roles[turn * KINDEX + QINDEX];
			break;
		}
		hist.pop_back();
		allThreats();
	}

	void Board::allThreats() 
	{
		cpins = 0;
		for (int i = 0; i < SPACES * 2; ++i)
			threatened[i] = 0;
		for (int from = 0; from < SPACES; ++from) {
			if (grid[from]) 
			{ 
				pieceThreats(from); 
			}
		}
	}

	void Board::pieceThreats(int from) 
	{//generates all pseudo legal moves for one piece
		int i, j;
		enum::side_enum us = (grid[from] > 0) ? WHITE : BLACK;
		switch (grid[from]) {
		case W_KING:
		case B_KING:
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + BOARD_SOUTHEAST]++][from + BOARD_SOUTHEAST] = from; 
			}
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + BOARD_EAST]++][from + BOARD_EAST] = from; 
			}
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + BOARD_NORTHWEST]++][from + BOARD_NORTHWEST] = from; 
			}
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + BOARD_WEST]++][from + BOARD_WEST] = from; 
			}
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + BOARD_SOUTHWEST]++][from + BOARD_SOUTHWEST] = from; 
			}
			if (from < 56) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + BOARD_SOUTH]++][from + BOARD_SOUTH] = from; 
			}
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + BOARD_NORTHEAST]++][from + BOARD_NORTHEAST] = from; 
			}
			if (from > 7) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + BOARD_NORTH]++][from + BOARD_NORTH] = from; 
			}
			return;
		case W_PAWN:
		case B_PAWN:
			i = (us) ? BOARD_NORTH : BOARD_SOUTH;
			if (from % WIDTH) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + i + BOARD_WEST]++][from + i + BOARD_WEST] = from; 
			}
			if (from % WIDTH != 7) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + i + BOARD_EAST]++][from + i + BOARD_EAST] = from; 
			}
			break;
		case W_KNIGHT:
		case B_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + 10]++][from + 10] = from; 
			}
			if ((from + 17) % WIDTH > from % WIDTH && from < 47) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + 17]++][from + 17] = from; 
			}
			if ((from - 10) % WIDTH < from % WIDTH && from > 9) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from - 10]++][from - 10] = from; 
			}
			if ((from - 17) % WIDTH < from % WIDTH && from > 16) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from - 17]++][from - 17] = from; 
			}
			if ((from + 6) % WIDTH < from % WIDTH && from < 58) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + 6]++][from + 6] = from; 
			}
			if ((from + 15) % WIDTH < from % WIDTH && from < 49) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from + 15]++][from + 15] = from; 
			}
			if ((from - 6) % WIDTH > from % WIDTH && from > 5) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from - 6]++][from - 6] = from; 
			}
			if ((from - 15) % WIDTH > from % WIDTH && from > 14) 
			{ 
				attackers[us * WIDTH  + threatened[us * SPACES  + from - 15]++][from - 15] = from; 
			}
			break;
		case W_QUEEN:
		case W_ROOK:
		case B_QUEEN:
		case B_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
				attackers[us * WIDTH  + threatened[us * SPACES  + i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + BOARD_NORTH; j != kpos[turn]; j += BOARD_NORTH) {
							if (grid[j]) { goto failN; }
						}
						pins[cpins] = i;
						pins[5 + cpins++] = BOARD_NORTH;
					}
					break;
				}
			}
		failN:
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
				attackers[us * WIDTH  + threatened[us * SPACES  + i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + BOARD_SOUTH; j != kpos[turn]; j += BOARD_SOUTH) {
							if (grid[j]) { goto failS; }
						}
						pins[cpins] = i;
						pins[5 + cpins++] = BOARD_SOUTH;
					}
					break;
				}
			}
		failS:
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
				attackers[us * WIDTH  + threatened[us * SPACES  + i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + BOARD_EAST; j != kpos[turn]; j += BOARD_EAST) {
							if (grid[j]) { goto failE; }
						}
						pins[cpins] = i;
						pins[5 + cpins++] = BOARD_EAST;
					}
					break;
				}
			}
		failE:
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
				attackers[us * WIDTH  + threatened[us * SPACES  + i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + BOARD_WEST; j != kpos[turn]; j += BOARD_WEST) {
							if (grid[j]) { goto failW; }
						}
						pins[cpins] = i;
						pins[5 + cpins++] = BOARD_WEST;
					}
					break;
				}
			}
		failW:
			if (grid[from] < W_QUEEN && grid[from] > B_QUEEN) { break; }
		case W_BISHOP:
		case B_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
				attackers[us * WIDTH  + threatened[us * SPACES  + i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + BOARD_NORTHEAST; j != kpos[turn]; j += BOARD_NORTHEAST) {
							if (grid[j]) { goto failNE; }
						}
						pins[cpins] = i;
						pins[5 + cpins++] = BOARD_NORTHEAST;
					}
					break;
				}
			}
		failNE:
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
				attackers[us * WIDTH  + threatened[us * SPACES  + i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + BOARD_NORTHWEST; j != kpos[turn]; j += BOARD_NORTHWEST) {
							if (grid[j]) { goto failNW; }
						}
						pins[cpins] = i;
						pins[5 + cpins++] = BOARD_NORTHWEST;
					}
					break;
				}
			}
		failNW:
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
				attackers[us * WIDTH  + threatened[us * SPACES  + i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHEAST; j != kpos[turn]; j += BOARD_SOUTHEAST) {
							if (grid[j]) { goto failSE; }
						}
						pins[cpins] = i;
						pins[5 + cpins++] = BOARD_SOUTHEAST;
					}
					break;
				}
			}
		failSE:
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
				attackers[us * WIDTH  + threatened[us * SPACES  + i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHWEST; j != kpos[turn]; j += BOARD_SOUTHWEST) {
							if (grid[j]) { goto failSW; }
						}
						pins[cpins] = i;
						pins[5 + cpins++] = BOARD_SOUTHWEST;
					}
					break;
				}
			}
		failSW:
			break;
		}
	}
}
