#include "Board.h"
#include <cmath>

namespace Hopper {

	Board::Board() 
	{
		fenSet((const char*)STARTFEN);
	}

	void Board::fenSet(const char* fs) 
	{//sets board to state outlined in FEN string
		cturn = 1;
		for (int i = 0; i < MEMORY; ++i) {
			fHist[i] = 0;
			cHist[i] = 0;
			vHist[i] = 0;
			mHist[i] = NULLMOVE;
			zHist[i] = 0;
			pHist[i] = 0;
		}
		for (int i = PINDEX; i < KINDEX; ++i) {
			roles[BLACK][i] = 0;
			roles[WHITE][i] = 0;
		}
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 5; ++j) { pins[i][j] = 0; }
		}
		int index = 0, counter = 0, helper;
		while (fs[index] != ' ') {
			switch (fs[index]) {
			case 'P':  
				grid[counter++] = W_PAWN; 
				++roles[WHITE][PINDEX];
				break; 
			case 'R': 
				grid[counter++] = W_ROOK;
				++roles[WHITE][RINDEX];
				break;
			case 'N':  
				grid[counter++] = W_KNIGHT; 
				++roles[WHITE][NINDEX];
				break;
			case 'B':  
				grid[counter++] = W_BISHOP; 
				++roles[WHITE][BINDEX];
				break;
			case 'Q':  
				grid[counter++] = W_QUEEN; 
				++roles[WHITE][QINDEX];
				break;
			case 'K':  
				kpos[WHITE] = counter;
				grid[counter++] = W_KING; 
				break; 
			case 'p':  
				grid[counter++] = B_PAWN; 
				++roles[BLACK][PINDEX];
				break;
			case 'r':  
				grid[counter++] = B_ROOK; 
				++roles[BLACK][RINDEX];
				break;
			case 'n':  
				grid[counter++] = B_KNIGHT; 
				++roles[BLACK][NINDEX];
				break;
			case 'b':
				grid[counter++] = B_BISHOP; 
				++roles[BLACK][BINDEX];
				break;
			case 'q':  
				grid[counter++] = B_QUEEN; 
				++roles[BLACK][QINDEX];
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
			mHist[cturn - 1] = Move(from, to, DOUBLEPUSH);
		}
		index += 2;
		fHist[cturn - 1] = fs[index] - '0';
		vHist[cturn - 1] = 0;
		for (int i = 0; i < SPACES; ++i) { vHist[cturn - 1] += grid[i]; }
		zHist[cturn - 1] = z.newKey(this);
		pHist[cturn - 1] = z.newPawnKey(this);
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

	bool Board::isDraw() 
	{
		if (fHist[cturn - 1] >= 100) { return true; }
		bool once = false;
		for (int i = 4; i < fHist[cturn - 1]; i += 4) {
			if (zHist[cturn - i - 1] == zHist[cturn - 1]) { 
				if (once) { return true; }
				else { once = true; }
			}
		}
		return false;
	}

	bool Board::insufficientMaterial() 
	{
		if (roles[WHITE][PINDEX] || roles[BLACK][PINDEX] || roles[WHITE][QINDEX] || roles[BLACK][QINDEX]) { return false; }
		int helper;
		for (int i = 0; i < 2; ++i) {
			helper = 0;
			for (int j = NINDEX; j < QINDEX; ++j) { helper += j * roles[i][j]; }
			if (helper > BINDEX) { return false; }
		}
		return true;
	}

	bool Board::isEndgame() 
	{
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

	void Board::movePiece(Move m) 
	{//executes a move if legal
		cHist[cturn] = cHist[cturn - 1];
		vHist[cturn] = vHist[cturn - 1];
		mHist[cturn] = m;
		fHist[cturn] = (m.getFlags() == STANDARD && abs(grid[m.getFrom()]) != W_PAWN) ? fHist[cturn - 1] + 1 : 0;
		zHist[cturn] = (mHist[cturn - 1].getFlags() == DOUBLEPUSH) ? zHist[cturn - 1] ^ z.side ^ z.enpassant[mHist[cturn - 1].getTo()] : zHist[cturn - 1] ^ z.side;
		pHist[cturn] = pHist[cturn - 1];
		switch (m.getFlags()) {
		case STANDARD:
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getFrom()]) % 10, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getFrom()]) % 10, turn, m.getTo());
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			if (abs(grid[m.getTo()]) == W_KING) { kpos[turn] = m.getTo(); }
			else if (abs(grid[m.getTo()]) == W_PAWN) {
				pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
				pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getTo());
			}
			break;
		case DOUBLEPUSH:
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getTo());
			zHist[cturn] ^= z.enpassant[m.getTo()];
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getTo());
			break;
		case KCASTLE:
			zHist[cturn] ^= z.piecesAt(KINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(KINDEX, turn, m.getTo());
			zHist[cturn] ^= z.piecesAt(RINDEX, turn, m.getTo() + 1);
			zHist[cturn] ^= z.piecesAt(RINDEX, turn, m.getTo() - 1);
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() - 1] = grid[m.getTo() + 1];
			grid[m.getTo() + 1] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			kpos[turn] = m.getTo();
			break;
		case QCASTLE:
			zHist[cturn] ^= z.piecesAt(KINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(KINDEX, turn, m.getTo());
			zHist[cturn] ^= z.piecesAt(RINDEX, turn, m.getTo() - 2);
			zHist[cturn] ^= z.piecesAt(RINDEX, turn, m.getTo() + 1);
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getTo() + 1] = grid[m.getTo() - 2];
			grid[m.getTo() - 2] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			kpos[turn] = m.getTo();
			break;
		case ENPASSANT:
			--roles[!turn][PINDEX];
			vHist[cturn] += (turn) ? W_PAWN : B_PAWN;
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getTo());
			zHist[cturn] ^= (turn) ? z.piecesAt(PINDEX, BLACK, m.getTo() + BOARD_SOUTH) : z.piecesAt(PINDEX, WHITE, m.getTo() + BOARD_NORTH);
			grid[m.getTo()] = grid[m.getFrom()];
			(turn) ? grid[m.getTo() + BOARD_SOUTH] = EMPTY : grid[m.getTo() + BOARD_NORTH] = EMPTY;
			grid[m.getFrom()] = EMPTY;
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getTo());
			pHist[cturn] ^= (turn) ? z.piecesAt(PINDEX, BLACK, m.getTo() + BOARD_SOUTH) : z.piecesAt(PINDEX, WHITE, m.getTo() + BOARD_NORTH);
			break;
		case CAPTURE:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			if (abs(grid[m.getTo()]) == W_PAWN) { pHist[cturn] ^= z.piecesAt(PINDEX, !turn, m.getTo()); }
			vHist[cturn] -= grid[m.getTo()];
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getFrom()]) % 10, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getFrom()]) % 10, turn, m.getTo());
			grid[m.getTo()] = grid[m.getFrom()];
			grid[m.getFrom()] = EMPTY;
			if (abs(grid[m.getTo()]) == W_PAWN) {
				pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
				pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getTo());
			}
			else if (abs(grid[m.getTo()]) == W_KING) { kpos[turn] = m.getTo(); }
			break;
		case NPROMOTE:
			--roles[turn][PINDEX];
			++roles[turn][NINDEX];
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			vHist[cturn] += (turn) ? B_PAWN + W_KNIGHT : W_PAWN + B_KNIGHT;
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(NINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_KNIGHT : B_KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTE:
			--roles[turn][PINDEX];
			++roles[turn][BINDEX];
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			vHist[cturn] += (turn) ? B_PAWN + W_BISHOP : W_PAWN + B_BISHOP;
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(BINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_BISHOP : B_BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTE:
			--roles[turn][PINDEX];
			++roles[turn][RINDEX];
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			vHist[cturn] += (turn) ? B_PAWN + W_ROOK : W_PAWN + B_ROOK;
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(RINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_ROOK : B_ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTE:
			--roles[turn][PINDEX];
			++roles[turn][QINDEX];
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			vHist[cturn] += (turn) ? B_PAWN + W_QUEEN : W_PAWN - W_QUEEN;
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(QINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_QUEEN : B_QUEEN;
			grid[m.getFrom()] = EMPTY;
			break;
		case NPROMOTEC:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			--roles[turn][PINDEX];
			++roles[turn][NINDEX];
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			vHist[cturn] += (turn) ? B_PAWN + W_KNIGHT - grid[m.getTo()] : W_PAWN + B_KNIGHT - grid[m.getTo()];
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			zHist[cturn] ^= z.piecesAt(NINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_KNIGHT : B_KNIGHT;
			grid[m.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			--roles[turn][PINDEX];
			++roles[turn][BINDEX];
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			vHist[cturn] += (turn) ? B_PAWN + W_BISHOP - grid[m.getTo()] : W_PAWN + B_BISHOP - grid[m.getTo()];
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			zHist[cturn] ^= z.piecesAt(BINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_BISHOP : B_BISHOP;
			grid[m.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			--roles[turn][PINDEX];
			++roles[turn][RINDEX];
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			vHist[cturn] += (turn) ? B_PAWN + W_ROOK - grid[m.getTo()] : W_PAWN + B_ROOK - grid[m.getTo()];
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			zHist[cturn] ^= z.piecesAt(RINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_ROOK : B_ROOK;
			grid[m.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			--roles[!turn][abs(grid[m.getTo()]) % 10];
			--roles[turn][PINDEX];
			++roles[turn][QINDEX];
			pHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			vHist[cturn] += (turn) ? B_PAWN + W_QUEEN - grid[m.getTo()] : +W_PAWN - W_QUEEN - grid[m.getTo()];
			zHist[cturn] ^= z.piecesAt(PINDEX, turn, m.getFrom());
			zHist[cturn] ^= z.piecesAt(abs(grid[m.getTo()]) % 10, !turn, m.getTo());
			zHist[cturn] ^= z.piecesAt(QINDEX, turn, m.getTo());
			grid[m.getTo()] = (turn) ? W_QUEEN : B_QUEEN;
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
		turn = (side_enum)(!turn);
		allThreats();
	}

	void Board::unmovePiece() 
	{//unmakes a move
		turn = (side_enum)(!turn);
		--cturn;
		switch (mHist[cturn].getFlags()) {
		case STANDARD:
			grid[mHist[cturn].getFrom()] = grid[mHist[cturn].getTo()];
			grid[mHist[cturn].getTo()] = EMPTY;
			if (abs(grid[mHist[cturn].getFrom()]) == W_KING) { kpos[turn] = mHist[cturn].getFrom(); }
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
			grid[mHist[cturn].getTo()] = (role_enum) (vHist[cturn - 1] - vHist[cturn]);
			if (abs(grid[mHist[cturn].getFrom()]) == W_KING) { kpos[turn] = mHist[cturn].getFrom(); }
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			break;
		case ENPASSANT:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			grid[mHist[cturn - 1].getTo()] = (turn) ? B_PAWN : W_PAWN;
			++roles[!turn][PINDEX];
			break;
		case NPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			++roles[turn][PINDEX];
			--roles[turn][NINDEX];
			break;
		case BPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			++roles[turn][PINDEX];
			--roles[turn][BINDEX];
			break;
		case RPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			++roles[turn][PINDEX];
			--roles[turn][RINDEX];
			break;
		case QPROMOTE:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = EMPTY;
			++roles[turn][PINDEX];
			--roles[turn][QINDEX];
			break;
		case NPROMOTEC:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? (role_enum)( vHist[cturn - 1] - vHist[cturn] + W_KNIGHT - W_PAWN) : (role_enum)(vHist[cturn - 1] - vHist[cturn] + B_KNIGHT + W_PAWN);
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			++roles[turn][PINDEX];
			--roles[turn][NINDEX];
			break;
		case BPROMOTEC:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? (role_enum) (vHist[cturn - 1] - vHist[cturn] + W_BISHOP - W_PAWN) : (role_enum)(vHist[cturn - 1] - vHist[cturn] + B_BISHOP + W_PAWN);
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			++roles[turn][PINDEX];
			--roles[turn][BINDEX];
			break;
		case RPROMOTEC:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? (role_enum)(vHist[cturn - 1] - vHist[cturn] + W_ROOK - W_PAWN) : (role_enum)(vHist[cturn - 1] - vHist[cturn] + B_ROOK + W_PAWN);
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			++roles[turn][PINDEX];
			--roles[turn][RINDEX];
			break;
		case QPROMOTEC:
			grid[mHist[cturn].getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[mHist[cturn].getTo()] = (turn) ? (role_enum)(vHist[cturn - 1] - vHist[cturn] + W_QUEEN - W_PAWN) : (role_enum)(vHist[cturn - 1] - vHist[cturn] - W_QUEEN + W_PAWN);
			++roles[!turn][abs(grid[mHist[cturn].getTo()]) % 10];
			++roles[turn][PINDEX];
			--roles[turn][QINDEX];
			break;
		}
		allThreats();
	}

	void Board::allThreats() 
	{
		cpins = 0;
		for (int i = 0; i < SPACES; ++i) {
			threatened[BLACK][i] = 0;
			threatened[WHITE][i] = 0;
		}
		for (int from = 0; from < SPACES; ++from) {
			if (grid[from]) { pieceThreats(from); }
		}
	}

	void Board::pieceThreats(int from) 
	{//generates all pseudo legal moves for one piece
		int i, j;
		enum::side_enum us = (grid[from] > 0) ? WHITE : BLACK;
		switch (abs(grid[from])) {
		case W_KING:
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55) { attackers[us][threatened[us][from + BOARD_SOUTHEAST]++][from + BOARD_SOUTHEAST] = from; }
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH) { attackers[us][threatened[us][from + BOARD_EAST]++][from + BOARD_EAST] = from; }
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8) { attackers[us][threatened[us][from + BOARD_NORTHWEST]++][from + BOARD_NORTHWEST] = from; }
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0) { attackers[us][threatened[us][from + BOARD_WEST]++][from + BOARD_WEST] = from; }
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57) { attackers[us][threatened[us][from + BOARD_SOUTHWEST]++][from + BOARD_SOUTHWEST] = from; }
			if (from < 56) { attackers[us][threatened[us][from + BOARD_SOUTH]++][from + BOARD_SOUTH] = from; }
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6) { attackers[us][threatened[us][from + BOARD_NORTHEAST]++][from + BOARD_NORTHEAST] = from; }
			if (from > 7) { attackers[us][threatened[us][from + BOARD_NORTH]++][from + BOARD_NORTH] = from; }
			return;
		case W_PAWN:
			i = (us) ? BOARD_NORTH : BOARD_SOUTH;
			if (from % WIDTH) { attackers[us][threatened[us][from + i + BOARD_WEST]++][from + i + BOARD_WEST] = from; }
			if (from % WIDTH != 7) { attackers[us][threatened[us][from + i + BOARD_EAST]++][from + i + BOARD_EAST] = from; }
			break;
		case W_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54) { attackers[us][threatened[us][from + 10]++][from + 10] = from; }
			if ((from + 17) % WIDTH > from % WIDTH && from < 47) { attackers[us][threatened[us][from + 17]++][from + 17] = from; }
			if ((from - 10) % WIDTH < from % WIDTH && from > 9) { attackers[us][threatened[us][from - 10]++][from - 10] = from; }
			if ((from - 17) % WIDTH < from % WIDTH && from > 16) { attackers[us][threatened[us][from - 17]++][from - 17] = from; }
			if ((from + 6) % WIDTH < from % WIDTH && from < 58) { attackers[us][threatened[us][from + 6]++][from + 6] = from; }
			if ((from + 15) % WIDTH < from % WIDTH && from < 49) { attackers[us][threatened[us][from + 15]++][from + 15] = from; }
			if ((from - 6) % WIDTH > from % WIDTH && from > 5) { attackers[us][threatened[us][from - 6]++][from - 6] = from; }
			if ((from - 15) % WIDTH > from % WIDTH && from > 14) { attackers[us][threatened[us][from - 15]++][from - 15] = from; }
			break;
		case W_QUEEN:
		case W_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + BOARD_NORTH; j != kpos[turn]; j += BOARD_NORTH) {
							if (grid[j]) { goto failN; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = BOARD_NORTH;
					}
					break;
				}
			}
		failN:
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + BOARD_SOUTH; j != kpos[turn]; j += BOARD_SOUTH) {
							if (grid[j]) { goto failS; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = BOARD_SOUTH;
					}
					break;
				}
			}
		failS:
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + BOARD_EAST; j != kpos[turn]; j += BOARD_EAST) {
							if (grid[j]) { goto failE; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = BOARD_EAST;
					}
					break;
				}
			}
		failE:
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + BOARD_WEST; j != kpos[turn]; j += BOARD_WEST) {
							if (grid[j]) { goto failW; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = BOARD_WEST;
					}
					break;
				}
			}
		failW:
			if (abs(grid[from]) != W_QUEEN) { break; }
		case W_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + BOARD_NORTHEAST; j != kpos[turn]; j += BOARD_NORTHEAST) {
							if (grid[j]) { goto failNE; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = BOARD_NORTHEAST;
					}
					break;
				}
			}
		failNE:
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i > kpos[turn] && turn != us) {
						for (j = i + BOARD_NORTHWEST; j != kpos[turn]; j += BOARD_NORTHWEST) {
							if (grid[j]) { goto failNW; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = BOARD_NORTHWEST;
					}
					break;
				}
			}
		failNW:
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NWSEslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHEAST; j != kpos[turn]; j += BOARD_SOUTHEAST) {
							if (grid[j]) { goto failSE; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = BOARD_SOUTHEAST;
					}
					break;
				}
			}
		failSE:
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kpos[turn]) && NESWslide(i, kpos[turn]) && i < kpos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHWEST; j != kpos[turn]; j += BOARD_SOUTHWEST) {
							if (grid[j]) { goto failSW; }
						}
						pins[cpins][0] = i;
						pins[cpins++][1] = BOARD_SOUTHWEST;
					}
					break;
				}
			}
		failSW:
			break;
		}
	}
}
