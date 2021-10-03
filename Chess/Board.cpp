#include "Board.h"
#include <cmath>
#include <string>

namespace Hopper {

	Board::Board()
	{
		fenSet((const char*)STARTFEN);
	}

	void Board::fenSet(const char* fs)
	{//sets board to state outlined in FEN string
		halfMoveClock = 0;
		memset(roleCounts, 0, sizeof(roleCounts));
		memset(threatened, 0, sizeof(threatened));
		memset(pinnedPieces, 0, sizeof(pinnedPieces));
		memset(attackers, 0, sizeof(attackers));
		int index = 0, counter = 0, helper;
		while (fs[index] != ' ') {
			switch (fs[index]) {
			case 'P':
				grid[counter++] = W_PAWN;
				++roleCounts[KINDEX + PINDEX];
				break;
			case 'R':
				grid[counter++] = W_ROOK;
				++roleCounts[KINDEX + RINDEX];
				break;
			case 'N':
				grid[counter++] = W_KNIGHT;
				++roleCounts[KINDEX + NINDEX];
				break;
			case 'B':
				grid[counter++] = W_BISHOP;
				++roleCounts[KINDEX + BINDEX];
				break;
			case 'Q':
				grid[counter++] = W_QUEEN;
				++roleCounts[KINDEX + QINDEX];
				break;
			case 'K':
				kingPos[WHITE] = counter;
				grid[counter++] = W_KING;
				break;
			case 'p':
				grid[counter++] = B_PAWN;
				++roleCounts[PINDEX];
				break;
			case 'r':
				grid[counter++] = B_ROOK;
				++roleCounts[RINDEX];
				break;
			case 'n':
				grid[counter++] = B_KNIGHT;
				++roleCounts[NINDEX];
				break;
			case 'b':
				grid[counter++] = B_BISHOP;
				++roleCounts[BINDEX];
				break;
			case 'q':
				grid[counter++] = B_QUEEN;
				++roleCounts[QINDEX];
				break;
			case 'k':
				kingPos[BLACK] = counter;
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
		turn = (fs[++index] == 'w');
		++index;
		myHistory[halfMoveClock].cHist = 0;
		do {
			switch (fs[index++]) {
			case 'K':
				myHistory[halfMoveClock].cHist |= 1 << 0;
				break;
			case 'Q':
				myHistory[halfMoveClock].cHist |= 1 << 1;
				break;
			case 'k':
				myHistory[halfMoveClock].cHist |= 1 << 2;
				break;
			case 'q':
				myHistory[halfMoveClock].cHist |= 1 << 3;
				break;
			}
		} while (fs[index] != ' ');
		if (fs[++index] != '-') {
			int from = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
			int to = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			myHistory[halfMoveClock].mHist = Move(from, to, DOUBLEPUSH);
		}
		index += 2;
		myHistory[halfMoveClock].fHist = fs[index] - '0';
		myHistory[halfMoveClock].vHist = 0;
		for (int i = 0; i < SPACES; ++i)
			myHistory[halfMoveClock].vHist += grid[i];
		myHistory[halfMoveClock].zHist = myZobrist.newKey(this);
		myHistory[halfMoveClock].pHist = myZobrist.newPawnKey(this);
		allThreats();
	}

	bool Board::isCheckMate()
	{
		Move nextMove[28];
		int moveCount;
		for (int i = 0; i < SPACES; ++i) {
			if ((turn && grid[i] > 0) || (!turn && grid[i] < 0)) {
				moveCount = genAllMovesAt(nextMove, i);
				if (moveCount) {
					moveCount = removeIllegalMoves(nextMove, moveCount);
					if (moveCount) { return false; }
				}
			}
		}
		return true;
	}

	bool Board::isRepititionDraw()
	{
		if (myHistory[halfMoveClock].fHist >= 100)
			return true;
		bool once = false;
		for (int i = 4; i < myHistory[halfMoveClock].fHist; i += 4) {
			if (myHistory[halfMoveClock - i].zHist == myHistory[halfMoveClock].zHist) {
				if (once)
					return true;
				else
					once = true;
			}
		}
		return false;
	}

	bool Board::isPseudoRepititionDraw()
	{
		if (myHistory[halfMoveClock].fHist >= 100)
			return true;
		for (int i = 4; i < myHistory[halfMoveClock].fHist; i += 4) {
			if (myHistory[halfMoveClock - i].zHist == myHistory[halfMoveClock].zHist)
				return true;
		}
		return false;
	}

	bool Board::isMaterialDraw()
	{
		if (roleCounts[KINDEX + PINDEX] || roleCounts[PINDEX] || roleCounts[KINDEX + QINDEX] || roleCounts[QINDEX]) { return false; }
		int helper;
		for (int i = 0; i < 2; ++i) {
			helper = 0;
			for (int j = NINDEX; j < QINDEX; ++j)
				helper += j * roleCounts[i * KINDEX + j];
			if (helper > BINDEX)
				return false;
		}
		return true;
	}

	bool Board::isEndgame()
	{
		for (int i = 0; i < 2; ++i) {
			if (roleCounts[KINDEX * i + QINDEX]) {
				for (int j = NINDEX; j < QINDEX; ++j) {
					if (roleCounts[KINDEX * i + j])
						return false;
				}
				if (roleCounts[KINDEX * i + QINDEX] > 1)
					return false;
			}
		}
		return true;
	}

	void Board::movePiece(Move nextMove)
	{//executes a move if legal
		myHistory[halfMoveClock + 1] = historyInfo(
			(nextMove.getFlags() == STANDARD && abs(grid[nextMove.getFrom()]) != W_PAWN) ? myHistory[halfMoveClock].fHist + 1 : 0,
			myHistory[halfMoveClock].cHist,
			myHistory[halfMoveClock].vHist,
			(myHistory[halfMoveClock].mHist.getFlags() == DOUBLEPUSH) ? myHistory[halfMoveClock].zHist ^ myZobrist.sideAt() ^ myZobrist.enPassantAt(myHistory[halfMoveClock].mHist.getTo()) : myHistory[halfMoveClock].zHist ^ myZobrist.sideAt(),
			myHistory[halfMoveClock].pHist,
			nextMove
		);
		++halfMoveClock;
		switch (nextMove.getFlags()) {
		case STANDARD:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getFrom()]) % 10, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getFrom()]) % 10, turn, nextMove.getTo());
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getFrom()] = EMPTY;
			if (abs(grid[nextMove.getTo()]) == W_KING) {
				kingPos[turn] = nextMove.getTo();
			}
			else if (abs(grid[nextMove.getTo()]) == W_PAWN) {
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getTo());
			}
			break;
		case DOUBLEPUSH:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.enPassantAt(nextMove.getTo());
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getFrom()] = EMPTY;
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getTo());
			break;
		case KCASTLE:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(KINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(KINDEX, turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(RINDEX, turn, nextMove.getTo() + 1);
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(RINDEX, turn, nextMove.getTo() - 1);
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getTo() - 1] = grid[nextMove.getTo() + 1];
			grid[nextMove.getTo() + 1] = EMPTY;
			grid[nextMove.getFrom()] = EMPTY;
			kingPos[turn] = nextMove.getTo();
			break;
		case QCASTLE:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(KINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(KINDEX, turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(RINDEX, turn, nextMove.getTo() - 2);
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(RINDEX, turn, nextMove.getTo() + 1);
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getTo() + 1] = grid[nextMove.getTo() - 2];
			grid[nextMove.getTo() - 2] = EMPTY;
			grid[nextMove.getFrom()] = EMPTY;
			kingPos[turn] = nextMove.getTo();
			break;
		case CAPTURE:
			--roleCounts[!turn * KINDEX + abs(grid[nextMove.getTo()]) % 10];
			if (abs(grid[nextMove.getTo()]) == W_PAWN) { myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, !turn, nextMove.getTo()); }
			myHistory[halfMoveClock].vHist -= grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getFrom()]) % 10, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getTo()]) % 10, !turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getFrom()]) % 10, turn, nextMove.getTo());
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getFrom()] = EMPTY;
			if (abs(grid[nextMove.getTo()]) == W_PAWN) {
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getTo());
			}
			else if (abs(grid[nextMove.getTo()]) == W_KING) { kingPos[turn] = nextMove.getTo(); }
			break;
		case ENPASSANT:
			--roleCounts[!turn * KINDEX + PINDEX];
			myHistory[halfMoveClock].vHist += (turn) ? W_PAWN : B_PAWN;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= (turn) ? myZobrist.piecesAt(PINDEX, BLACK, nextMove.getTo() + BOARD_SOUTH) : myZobrist.piecesAt(PINDEX, WHITE, nextMove.getTo() + BOARD_NORTH);
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			(turn) ? grid[nextMove.getTo() + BOARD_SOUTH] = EMPTY : grid[nextMove.getTo() + BOARD_NORTH] = EMPTY;
			grid[nextMove.getFrom()] = EMPTY;
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getTo());
			myHistory[halfMoveClock].pHist ^= (turn) ? myZobrist.piecesAt(PINDEX, BLACK, nextMove.getTo() + BOARD_SOUTH) : myZobrist.piecesAt(PINDEX, WHITE, nextMove.getTo() + BOARD_NORTH);
			break;
		case NPROMOTE:
			--roleCounts[turn * KINDEX + PINDEX];
			++roleCounts[turn * KINDEX + NINDEX];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].vHist += (turn) ? B_PAWN + W_KNIGHT : W_PAWN + B_KNIGHT;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(NINDEX, turn, nextMove.getTo());
			grid[nextMove.getTo()] = (turn) ? W_KNIGHT : B_KNIGHT;
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case BPROMOTE:
			--roleCounts[turn * KINDEX + PINDEX];
			++roleCounts[turn * KINDEX + BINDEX];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].vHist += (turn) ? B_PAWN + W_BISHOP : W_PAWN + B_BISHOP;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(BINDEX, turn, nextMove.getTo());
			grid[nextMove.getTo()] = (turn) ? W_BISHOP : B_BISHOP;
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case RPROMOTE:
			--roleCounts[turn * KINDEX + PINDEX];
			++roleCounts[turn * KINDEX + RINDEX];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].vHist += (turn) ? B_PAWN + W_ROOK : W_PAWN + B_ROOK;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(RINDEX, turn, nextMove.getTo());
			grid[nextMove.getTo()] = (turn) ? W_ROOK : B_ROOK;
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case QPROMOTE:
			--roleCounts[turn * KINDEX + PINDEX];
			++roleCounts[turn * KINDEX + QINDEX];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].vHist += (turn) ? B_PAWN + W_QUEEN : W_PAWN - W_QUEEN;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(QINDEX, turn, nextMove.getTo());
			grid[nextMove.getTo()] = (turn) ? W_QUEEN : B_QUEEN;
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case NPROMOTEC:
			--roleCounts[!turn * KINDEX + abs(grid[nextMove.getTo()]) % 10];
			--roleCounts[turn * KINDEX + PINDEX];
			++roleCounts[turn * KINDEX + NINDEX];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].vHist += (turn) ? B_PAWN + W_KNIGHT - grid[nextMove.getTo()] : W_PAWN + B_KNIGHT - grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getTo()]) % 10, !turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(NINDEX, turn, nextMove.getTo());
			grid[nextMove.getTo()] = (turn) ? W_KNIGHT : B_KNIGHT;
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			--roleCounts[!turn * KINDEX + abs(grid[nextMove.getTo()]) % 10];
			--roleCounts[turn * KINDEX + PINDEX];
			++roleCounts[turn * KINDEX + BINDEX];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].vHist += (turn) ? B_PAWN + W_BISHOP - grid[nextMove.getTo()] : W_PAWN + B_BISHOP - grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getTo()]) % 10, !turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(BINDEX, turn, nextMove.getTo());
			grid[nextMove.getTo()] = (turn) ? W_BISHOP : B_BISHOP;
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			--roleCounts[!turn * KINDEX + abs(grid[nextMove.getTo()]) % 10];
			--roleCounts[turn * KINDEX + PINDEX];
			++roleCounts[turn * KINDEX + RINDEX];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].vHist += (turn) ? B_PAWN + W_ROOK - grid[nextMove.getTo()] : W_PAWN + B_ROOK - grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getTo()]) % 10, !turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(RINDEX, turn, nextMove.getTo());
			grid[nextMove.getTo()] = (turn) ? W_ROOK : B_ROOK;
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			--roleCounts[!turn * KINDEX + abs(grid[nextMove.getTo()]) % 10];
			--roleCounts[turn * KINDEX + PINDEX];
			++roleCounts[turn * KINDEX + QINDEX];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].vHist += (turn) ? B_PAWN + W_QUEEN - grid[nextMove.getTo()] : W_PAWN - W_QUEEN - grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(PINDEX, turn, nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(abs(grid[nextMove.getTo()]) % 10, !turn, nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(QINDEX, turn, nextMove.getTo());
			grid[nextMove.getTo()] = (turn) ? W_QUEEN : B_QUEEN;
			grid[nextMove.getFrom()] = EMPTY;
			break;
		}
		if (myHistory[halfMoveClock].cHist & 1 << 0 && (nextMove.getTo() == 63 || nextMove.getFrom() == 63)) {
			myHistory[halfMoveClock].cHist &= ~(1 << 0);
			myHistory[halfMoveClock].zHist ^= myZobrist.castleAt(WKINGSIDE);
		}
		if (myHistory[halfMoveClock].cHist & 1 << 1 && (nextMove.getTo() == 56 || nextMove.getFrom() == 56)) {
			myHistory[halfMoveClock].cHist &= ~(1 << 1);
			myHistory[halfMoveClock].zHist ^= myZobrist.castleAt(WQUEENSIDE);
		}
		if (nextMove.getFrom() == 60) {
			if (myHistory[halfMoveClock].cHist & 1 << 0) {
				myHistory[halfMoveClock].cHist &= ~(1 << 0);
				myHistory[halfMoveClock].zHist ^= myZobrist.castleAt(WKINGSIDE);
			}
			if (myHistory[halfMoveClock].cHist & 1 << 1) {
				myHistory[halfMoveClock].cHist &= ~(1 << 1);
				myHistory[halfMoveClock].zHist ^= myZobrist.castleAt(WQUEENSIDE);
			}
		}
		if (myHistory[halfMoveClock].cHist & 1 << 2 && (nextMove.getTo() == 7 || nextMove.getFrom() == 7)) {
			myHistory[halfMoveClock].cHist &= ~(1 << 2);
			myHistory[halfMoveClock].zHist ^= myZobrist.castleAt(BKINGSIDE);
		}
		if (myHistory[halfMoveClock].cHist & 1 << 3 && (nextMove.getTo() == 0 || nextMove.getFrom() == 0)) {
			myHistory[halfMoveClock].cHist &= ~(1 << 3);
			myHistory[halfMoveClock].zHist ^= myZobrist.castleAt(BQUEENSIDE);
		}
		if (nextMove.getFrom() == 4) {
			if (myHistory[halfMoveClock].cHist & 1 << 2) {
				myHistory[halfMoveClock].cHist &= ~(1 << 2);
				myHistory[halfMoveClock].zHist ^= myZobrist.castleAt(BKINGSIDE);
			}
			if (myHistory[halfMoveClock].cHist & 1 << 3) {
				myHistory[halfMoveClock].cHist &= ~(1 << 3);
				myHistory[halfMoveClock].zHist ^= myZobrist.castleAt(BQUEENSIDE);
			}
		}
		turn = (!turn);
		allThreats();
	}

	void Board::unmovePiece()
	{//unmakes a move
		turn = (!turn);
		switch (myHistory[halfMoveClock].mHist.getFlags()) {
		case STANDARD:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = grid[myHistory[halfMoveClock].mHist.getTo()];
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			if (grid[myHistory[halfMoveClock].mHist.getFrom()] == W_KING || grid[myHistory[halfMoveClock].mHist.getFrom()] == B_KING) { kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom(); }
			break;
		case DOUBLEPUSH:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = grid[myHistory[halfMoveClock].mHist.getTo()];
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			break;
		case KCASTLE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = grid[myHistory[halfMoveClock].mHist.getTo()];
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			grid[myHistory[halfMoveClock].mHist.getTo() + 1] = grid[myHistory[halfMoveClock].mHist.getTo() - 1];
			grid[myHistory[halfMoveClock].mHist.getTo() - 1] = EMPTY;
			kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom();
			break;
		case QCASTLE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = grid[myHistory[halfMoveClock].mHist.getTo()];
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			grid[myHistory[halfMoveClock].mHist.getTo() - 2] = grid[myHistory[halfMoveClock].mHist.getTo() + 1];
			grid[myHistory[halfMoveClock].mHist.getTo() + 1] = EMPTY;
			kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom();
			break;
		case CAPTURE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = grid[myHistory[halfMoveClock].mHist.getTo()];
			grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist);
			if (abs(grid[myHistory[halfMoveClock].mHist.getFrom()]) == W_KING) { kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom(); }
			++roleCounts[!turn * KINDEX + abs(grid[myHistory[halfMoveClock].mHist.getTo()]) % 10];
			break;
		case ENPASSANT:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			if (turn) {
				grid[myHistory[halfMoveClock].mHist.getTo() + 8] = B_PAWN;
			}
			else {
				grid[myHistory[halfMoveClock].mHist.getTo() - 8] = W_PAWN;
			}
			++roleCounts[!turn * KINDEX + PINDEX];
			break;
		case NPROMOTE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[turn * KINDEX + PINDEX];
			--roleCounts[turn * KINDEX + NINDEX];
			break;
		case BPROMOTE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[turn * KINDEX + PINDEX];
			--roleCounts[turn * KINDEX + BINDEX];
			break;
		case RPROMOTE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[turn * KINDEX + PINDEX];
			--roleCounts[turn * KINDEX + RINDEX];
			break;
		case QPROMOTE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[turn * KINDEX + PINDEX];
			--roleCounts[turn * KINDEX + QINDEX];
			break;
		case NPROMOTEC:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = (turn) ? (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + W_KNIGHT - W_PAWN) : (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + B_KNIGHT + W_PAWN);
			++roleCounts[!turn * KINDEX + abs(grid[myHistory[halfMoveClock].mHist.getTo()]) % 10];
			++roleCounts[turn * KINDEX + PINDEX];
			--roleCounts[turn * KINDEX + NINDEX];
			break;
		case BPROMOTEC:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = (turn) ? (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + W_BISHOP - W_PAWN) : (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + B_BISHOP + W_PAWN);
			++roleCounts[!turn * KINDEX + abs(grid[myHistory[halfMoveClock].mHist.getTo()]) % 10];
			++roleCounts[turn * KINDEX + PINDEX];
			--roleCounts[turn * KINDEX + BINDEX];
			break;
		case RPROMOTEC:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = (turn) ? (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + W_ROOK - W_PAWN) : (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + B_ROOK + W_PAWN);
			++roleCounts[!turn * KINDEX + abs(grid[myHistory[halfMoveClock].mHist.getTo()]) % 10];
			++roleCounts[turn * KINDEX + PINDEX];
			--roleCounts[turn * KINDEX + RINDEX];
			break;
		case QPROMOTEC:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? W_PAWN : B_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = (turn) ? (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + W_QUEEN - W_PAWN) : (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist - W_QUEEN + W_PAWN);
			++roleCounts[!turn * KINDEX + abs(grid[myHistory[halfMoveClock].mHist.getTo()]) % 10];
			++roleCounts[turn * KINDEX + PINDEX];
			--roleCounts[turn * KINDEX + QINDEX];
			break;
		}
		--halfMoveClock;
		allThreats();
	}

	void Board::allThreats()
	{
		pinCount = 0;
		memset(threatened, 0, sizeof(threatened));
		for (int from = 0; from < SPACES; ++from) {
			if (grid[from])
				pieceThreats(from);
		}
	}

	void Board::pieceThreats(int from)
	{//generates all pseudo legal moves for one piece
		int i, j;
		bool us = (grid[from] > 0) ? WHITE : BLACK;
		switch (grid[from]) {
		case W_KING:
		case B_KING:
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55)
				attackers[us][threatened[us * SPACES + from + BOARD_SOUTHEAST]++][from + BOARD_SOUTHEAST] = from;
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH)
				attackers[us][threatened[us * SPACES + from + BOARD_EAST]++][from + BOARD_EAST] = from;
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8)
				attackers[us][threatened[us * SPACES + from + BOARD_NORTHWEST]++][from + BOARD_NORTHWEST] = from;
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0)
				attackers[us][threatened[us * SPACES + from + BOARD_WEST]++][from + BOARD_WEST] = from;
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57)
				attackers[us][threatened[us * SPACES + from + BOARD_SOUTHWEST]++][from + BOARD_SOUTHWEST] = from;
			if (from < 56)
				attackers[us][threatened[us * SPACES + from + BOARD_SOUTH]++][from + BOARD_SOUTH] = from;
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6)
				attackers[us][threatened[us * SPACES + from + BOARD_NORTHEAST]++][from + BOARD_NORTHEAST] = from;
			if (from > 7)
				attackers[us][threatened[us * SPACES + from + BOARD_NORTH]++][from + BOARD_NORTH] = from;
			return;
		case W_PAWN:
		case B_PAWN:
			i = (us) ? BOARD_NORTH : BOARD_SOUTH;
			if (from % WIDTH)
				attackers[us][threatened[us * SPACES + from + i + BOARD_WEST]++][from + i + BOARD_WEST] = from;
			if (from % WIDTH != 7)
				attackers[us][threatened[us * SPACES + from + i + BOARD_EAST]++][from + i + BOARD_EAST] = from;
			break;
		case W_KNIGHT:
		case B_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54)
				attackers[us][threatened[us * SPACES + from + 10]++][from + 10] = from;
			if ((from + 17) % WIDTH > from % WIDTH && from < 47)
				attackers[us][threatened[us * SPACES + from + 17]++][from + 17] = from;
			if ((from - 10) % WIDTH < from % WIDTH && from > 9)
				attackers[us][threatened[us * SPACES + from - 10]++][from - 10] = from;
			if ((from - 17) % WIDTH < from % WIDTH && from > 16)
				attackers[us][threatened[us * SPACES + from - 17]++][from - 17] = from;
			if ((from + 6) % WIDTH < from % WIDTH && from < 58)
				attackers[us][threatened[us * SPACES + from + 6]++][from + 6] = from;
			if ((from + 15) % WIDTH < from % WIDTH && from < 49)
				attackers[us][threatened[us * SPACES + from + 15]++][from + 15] = from;
			if ((from - 6) % WIDTH > from % WIDTH && from > 5)
				attackers[us][threatened[us * SPACES + from - 6]++][from - 6] = from;
			if ((from - 15) % WIDTH > from % WIDTH && from > 14)
				attackers[us][threatened[us * SPACES + from - 15]++][from - 15] = from;
			break;
		case W_QUEEN:
		case W_ROOK:
		case B_QUEEN:
		case B_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
				attackers[us][threatened[us * SPACES + i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTH; j != kingPos[turn]; j += BOARD_NORTH) {
							if (grid[j]) { goto failN; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[KINDEX + pinCount++] = BOARD_NORTH;
					}
					break;
				}
			}
		failN:
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
				attackers[us][threatened[us * SPACES + i]++][i] = from;
				if (grid[i]) {
					if (NSslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTH; j != kingPos[turn]; j += BOARD_SOUTH) {
							if (grid[j]) { goto failS; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[KINDEX + pinCount++] = BOARD_SOUTH;
					}
					break;
				}
			}
		failS:
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
				attackers[us][threatened[us * SPACES + i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_EAST; j != kingPos[turn]; j += BOARD_EAST) {
							if (grid[j]) { goto failE; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[KINDEX + pinCount++] = BOARD_EAST;
					}
					break;
				}
			}
		failE:
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
				attackers[us][threatened[us * SPACES + i]++][i] = from;
				if (grid[i]) {
					if (EWslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_WEST; j != kingPos[turn]; j += BOARD_WEST) {
							if (grid[j]) { goto failW; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[KINDEX + pinCount++] = BOARD_WEST;
					}
					break;
				}
			}
		failW:
			if (grid[from] < W_QUEEN && grid[from] > B_QUEEN) { break; }
		case W_BISHOP:
		case B_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
				attackers[us][threatened[us * SPACES + i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kingPos[turn]) && NESWslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTHEAST; j != kingPos[turn]; j += BOARD_NORTHEAST) {
							if (grid[j]) { goto failNE; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[KINDEX + pinCount++] = BOARD_NORTHEAST;
					}
					break;
				}
			}
		failNE:
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
				attackers[us][threatened[us * SPACES + i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kingPos[turn]) && NWSEslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTHWEST; j != kingPos[turn]; j += BOARD_NORTHWEST) {
							if (grid[j]) { goto failNW; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[KINDEX + pinCount++] = BOARD_NORTHWEST;
					}
					break;
				}
			}
		failNW:
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
				attackers[us][threatened[us * SPACES + i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kingPos[turn]) && NWSEslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHEAST; j != kingPos[turn]; j += BOARD_SOUTHEAST) {
							if (grid[j]) { goto failSE; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[KINDEX + pinCount++] = BOARD_SOUTHEAST;
					}
					break;
				}
			}
		failSE:
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
				attackers[us][threatened[us * SPACES + i]++][i] = from;
				if (grid[i]) {
					if (DIAGslide(i, kingPos[turn]) && NESWslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHWEST; j != kingPos[turn]; j += BOARD_SOUTHWEST) {
							if (grid[j]) { goto failSW; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[KINDEX + pinCount++] = BOARD_SOUTHWEST;
					}
					break;
				}
			}
		failSW:
			break;
		}
	}
}
