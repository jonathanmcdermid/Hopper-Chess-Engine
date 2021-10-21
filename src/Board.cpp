#include <stdlib.h>
#include <cstring>
#include <iostream>
#include "Board.h"

namespace Hopper {

	Board::Board()
	{
		fenSet((const char*)STARTFEN);
	}

	void Board::drawBoard()
	{//prints board in cmd
		char letter;
		std::cout << "\n  a   b   c   d   e   f   g   h";
		for (unsigned i = 0; i < WIDTH; ++i) {
			std::cout << "\n---------------------------------\n|";
			for (unsigned j = 0; j < WIDTH; ++j) {
				switch (grid[i * WIDTH + j]) {
				case WHITE_PAWN:
					letter = 'P'; break;
				case WHITE_ROOK:
					letter = 'R'; break;
				case WHITE_KNIGHT:
					letter = 'N'; break;
				case WHITE_BISHOP:
					letter = 'B'; break;
				case WHITE_QUEEN:
					letter = 'Q'; break;
				case WHITE_KING:
					letter = 'K'; break;
				case BLACK_PAWN:
					letter = 'p'; break;
				case BLACK_ROOK:
					letter = 'r'; break;
				case BLACK_KNIGHT:
					letter = 'n'; break;
				case BLACK_BISHOP:
					letter = 'b'; break;
				case BLACK_QUEEN:
					letter = 'q'; break;
				case BLACK_KING:
					letter = 'k'; break;
				case EMPTY:
					letter = ' '; break;
				default:
					letter = '@';
				}
				std::cout << " " << letter << " |";
			}
			std::cout << " " << WIDTH - i;
		}
		std::cout << "\n---------------------------------\n";
	}

	void Board::fenSet(const char* fs)
	{//sets board to state outlined in FEN string
		halfMoveClock = 0;
		memset(roleCounts, 0, sizeof(roleCounts));
		memset(threatened, 0, sizeof(threatened));
		memset(pinnedPieces, 0, sizeof(pinnedPieces));
		memset(attackers, 0, sizeof(attackers));
		unsigned index = 0, counter = 0, helper;
		while (fs[index] != ' ') {
			switch (fs[index]) {
			case 'P':
				grid[counter++] = WHITE_PAWN;
				++roleCounts[WHITE_PAWN];
				break;
			case 'R':
				grid[counter++] = WHITE_ROOK;
				++roleCounts[WHITE_ROOK];
				break;
			case 'N':
				grid[counter++] = WHITE_KNIGHT;
				++roleCounts[WHITE_KNIGHT];
				break;
			case 'B':
				grid[counter++] = WHITE_BISHOP;
				++roleCounts[WHITE_BISHOP];
				break;
			case 'Q':
				grid[counter++] = WHITE_QUEEN;
				++roleCounts[WHITE_QUEEN];
				break;
			case 'K':
				kingPos[WHITE] = counter;
				grid[counter++] = WHITE_KING;
				break;
			case 'p':
				grid[counter++] = BLACK_PAWN;
				++roleCounts[BLACK_PAWN];
				break;
			case 'r':
				grid[counter++] = BLACK_ROOK;
				++roleCounts[BLACK_ROOK];
				break;
			case 'n':
				grid[counter++] = BLACK_KNIGHT;
				++roleCounts[BLACK_KNIGHT];
				break;
			case 'b':
				grid[counter++] = BLACK_BISHOP;
				++roleCounts[BLACK_BISHOP];
				break;
			case 'q':
				grid[counter++] = BLACK_QUEEN;
				++roleCounts[BLACK_QUEEN];
				break;
			case 'k':
				kingPos[BLACK] = counter;
				grid[counter++] = BLACK_KING;
				break;
			case '/':
				break;
			default:
				helper = fs[index] - '0';
				while (helper--) { grid[counter++] = EMPTY; }
			}
			++index;
		}
		turn = (fs[++index] == 'b');
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
			int from = (turn) ? fs[index] - '0' + WIDTH : fs[index] - '0' - WIDTH;
			int to = (turn) ? fs[index] - '0' - WIDTH : fs[index] - '0' + WIDTH;
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
		scoredMove nextMove[28];
		unsigned moveCount;
		for (unsigned i = 0; i < SPACES; ++i) {
			if (validPiece(grid[i], turn)) {
				moveCount = genAllMovesAt(nextMove, i);
				if (moveCount) {
					if (removeIllegalMoves(nextMove, moveCount)) { return false; }
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
		for (unsigned i = 4; i < myHistory[halfMoveClock].fHist; i += 4) {
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
		for (unsigned i = 4; i < myHistory[halfMoveClock].fHist; i += 4) {
			if (myHistory[halfMoveClock - i].zHist == myHistory[halfMoveClock].zHist)
				return true;
		}
		return (myHistory[halfMoveClock].fHist >= 100);
	}

	bool Board::isMaterialDraw()const
	{
		return (roleCounts[WHITE_PAWN]	|| roleCounts[BLACK_PAWN]		||
				roleCounts[WHITE_QUEEN] || roleCounts[BLACK_QUEEN]		||
				roleCounts[WHITE_ROOK]  || roleCounts[BLACK_ROOK]		||
				roleCounts[WHITE_BISHOP] > 1 ||
				roleCounts[WHITE_KNIGHT] > 2 ||
				roleCounts[WHITE_BISHOP] + roleCounts[WHITE_KNIGHT] > 1 ||
				roleCounts[BLACK_BISHOP] > 1 ||
				roleCounts[BLACK_KNIGHT] > 2 ||
				roleCounts[BLACK_BISHOP] + roleCounts[BLACK_KNIGHT] > 1) == false;
	}

	static int gamephaseInc[12] = { 0,0,1,1,1,1,2,2,4,4,0,0 };

	unsigned Board::getGamePhase()const
	{
		return 
			roleCounts[WHITE_KNIGHT] * gamephaseInc[WHITE_KNIGHT] +
			roleCounts[BLACK_KNIGHT] * gamephaseInc[BLACK_KNIGHT] +
			roleCounts[WHITE_BISHOP] * gamephaseInc[WHITE_BISHOP] +
			roleCounts[BLACK_BISHOP] * gamephaseInc[BLACK_BISHOP] +
			roleCounts[WHITE_ROOK]   * gamephaseInc[WHITE_ROOK]   +
			roleCounts[BLACK_ROOK]   * gamephaseInc[BLACK_ROOK]   +
			roleCounts[WHITE_QUEEN]  * gamephaseInc[WHITE_QUEEN]  +
			roleCounts[BLACK_QUEEN]  * gamephaseInc[BLACK_QUEEN];
	}

	void Board::movePiece(Move nextMove)
	{//executes a move if legal
		myHistory[halfMoveClock + 1] = historyInfo(
			(nextMove.getFlags() == STANDARD && (grid[nextMove.getFrom()] != WHITE_PAWN && grid[nextMove.getTo()] != BLACK_PAWN)) ? 
			myHistory[halfMoveClock].fHist + 1 : 0,
			myHistory[halfMoveClock].cHist,
			myHistory[halfMoveClock].vHist,
			(myHistory[halfMoveClock].mHist.getFlags() == DOUBLEPUSH) ? 
			myHistory[halfMoveClock].zHist ^ myZobrist.sideAt() ^ myZobrist.enPassantAt(myHistory[halfMoveClock].mHist.getTo()) :
			myHistory[halfMoveClock].zHist ^ myZobrist.sideAt(),
			myHistory[halfMoveClock].pHist,
			nextMove
		);
		++halfMoveClock;
		switch (nextMove.getFlags()) {
		case STANDARD:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getTo());
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getFrom()] = EMPTY;
			if (grid[nextMove.getTo()] == WHITE_KING || grid[nextMove.getTo()] == BLACK_KING)
				kingPos[turn] = nextMove.getTo();
			else if (grid[nextMove.getTo()] == WHITE_PAWN || grid[nextMove.getTo()] == BLACK_PAWN) {
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getFrom());
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			}
			break;
		case DOUBLEPUSH:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.enPassantAt(nextMove.getTo());
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getFrom()] = EMPTY;
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getFrom());
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			break;
		case KCASTLE:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo() + 1], nextMove.getTo() + 1);
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo() + 1], nextMove.getTo() - 1);
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getTo() - 1] = grid[nextMove.getTo() + 1];
			grid[nextMove.getTo() + 1] = EMPTY;
			grid[nextMove.getFrom()] = EMPTY;
			kingPos[turn] = nextMove.getTo();
			break;
		case QCASTLE:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo() - 2], nextMove.getTo() - 2);
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo() - 2], nextMove.getTo() + 1);
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getTo() + 1] = grid[nextMove.getTo() - 2];
			grid[nextMove.getTo() - 2] = EMPTY;
			grid[nextMove.getFrom()] = EMPTY;
			kingPos[turn] = nextMove.getTo();
			break;
		case CAPTURE:
			--roleCounts[grid[nextMove.getTo()]];
			if (grid[nextMove.getTo()] == WHITE_PAWN || grid[nextMove.getTo()] == BLACK_PAWN) 
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].vHist -= grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getTo());
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			grid[nextMove.getFrom()] = EMPTY;
			if (grid[nextMove.getTo()] == WHITE_PAWN || grid[nextMove.getTo()] == BLACK_PAWN) {
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getFrom());
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			}
			else if (grid[nextMove.getTo()] == WHITE_KING || grid[nextMove.getTo()] == BLACK_KING) { kingPos[turn] = nextMove.getTo(); }
			break;
		case ENPASSANT:
			--roleCounts[WHITE_PAWN + turn];
			myHistory[halfMoveClock].vHist -= (turn == BLACK) ? WHITE_PAWN : BLACK_PAWN;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= (turn) ? myZobrist.piecesAt(WHITE_PAWN, nextMove.getTo() + BOARD_NORTH) : myZobrist.piecesAt(BLACK_PAWN, nextMove.getTo() + BOARD_SOUTH);
			grid[nextMove.getTo()] = grid[nextMove.getFrom()];
			if (turn == BLACK)
				grid[nextMove.getTo() + BOARD_NORTH] = EMPTY;
			else 
				grid[nextMove.getTo() + BOARD_SOUTH] = EMPTY;
			grid[nextMove.getFrom()] = EMPTY;
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getFrom());
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].pHist ^= (turn == BLACK) ? 
				myZobrist.piecesAt(WHITE_PAWN, nextMove.getTo() + BOARD_NORTH) : 
				myZobrist.piecesAt(BLACK_PAWN, nextMove.getTo() + BOARD_SOUTH);
			break;
		case NPROMOTE:
			--roleCounts[grid[nextMove.getFrom()]];
			++roleCounts[grid[nextMove.getFrom()] + WHITE_KNIGHT];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_KNIGHT;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()] + WHITE_KNIGHT, nextMove.getTo());
			grid[nextMove.getTo()] = (role_enum)(grid[nextMove.getFrom()] + WHITE_KNIGHT);
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case BPROMOTE:
			--roleCounts[grid[nextMove.getFrom()]];
			++roleCounts[grid[nextMove.getFrom()] + WHITE_BISHOP];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_BISHOP;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()] + WHITE_BISHOP, nextMove.getTo());
			grid[nextMove.getTo()] = (role_enum)(grid[nextMove.getFrom()] + WHITE_BISHOP);
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case RPROMOTE:
			--roleCounts[grid[nextMove.getFrom()]];
			++roleCounts[grid[nextMove.getFrom()] + WHITE_ROOK];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_ROOK;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()] + WHITE_ROOK, nextMove.getTo());
			grid[nextMove.getTo()] = (role_enum)(grid[nextMove.getFrom()] + WHITE_ROOK);
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case QPROMOTE:
			--roleCounts[grid[nextMove.getFrom()]];
			++roleCounts[grid[nextMove.getFrom()] + WHITE_QUEEN];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_QUEEN;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()] + WHITE_QUEEN, nextMove.getTo());
			grid[nextMove.getTo()] = (role_enum)(grid[nextMove.getFrom()] + WHITE_QUEEN);
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case NPROMOTEC:
			--roleCounts[grid[nextMove.getTo()]];
			--roleCounts[grid[nextMove.getFrom()]];
			++roleCounts[grid[nextMove.getFrom()] + WHITE_KNIGHT];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_KNIGHT - grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()] + WHITE_KNIGHT, nextMove.getTo());
			grid[nextMove.getTo()] = (role_enum)(grid[nextMove.getFrom()] + WHITE_KNIGHT);
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			--roleCounts[grid[nextMove.getTo()]];
			--roleCounts[grid[nextMove.getFrom()]];
			++roleCounts[grid[nextMove.getFrom()] + WHITE_BISHOP];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_BISHOP - grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()] + WHITE_BISHOP, nextMove.getTo());
			grid[nextMove.getTo()] = (role_enum)(grid[nextMove.getFrom()] + WHITE_BISHOP);
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			--roleCounts[grid[nextMove.getTo()]];
			--roleCounts[grid[nextMove.getFrom()]];
			++roleCounts[grid[nextMove.getFrom()] + WHITE_ROOK];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_ROOK - grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()] + WHITE_ROOK, nextMove.getTo());
			grid[nextMove.getTo()] = (role_enum)(grid[nextMove.getFrom()] + WHITE_ROOK);
			grid[nextMove.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			--roleCounts[grid[nextMove.getTo()]];
			--roleCounts[grid[nextMove.getFrom()]];
			++roleCounts[grid[nextMove.getFrom()] + WHITE_QUEEN];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_QUEEN - grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(grid[nextMove.getFrom()] + WHITE_QUEEN, nextMove.getTo());
			grid[nextMove.getTo()] = (role_enum) (grid[nextMove.getFrom()] + WHITE_QUEEN);
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
		turn = !turn;
		allThreats();
	}

	void Board::unmovePiece()
	{//unmakes a move
		turn = !turn;
		switch (myHistory[halfMoveClock].mHist.getFlags()) {
		case STANDARD:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = grid[myHistory[halfMoveClock].mHist.getTo()];
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			if (grid[myHistory[halfMoveClock].mHist.getFrom()] == WHITE_KING || grid[myHistory[halfMoveClock].mHist.getFrom()] == BLACK_KING) 
				kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom(); 
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
			if (grid[myHistory[halfMoveClock].mHist.getFrom()] == WHITE_KING || grid[myHistory[halfMoveClock].mHist.getFrom()] == BLACK_KING) 
				kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom();
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getTo()]];
			break;
		case ENPASSANT:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = grid[myHistory[halfMoveClock].mHist.getTo()];
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			if (turn == BLACK) {
				grid[myHistory[halfMoveClock].mHist.getTo() + BOARD_NORTH] = WHITE_PAWN;
				++roleCounts[grid[myHistory[halfMoveClock].mHist.getTo() + BOARD_NORTH]];
			}
			else {
				grid[myHistory[halfMoveClock].mHist.getTo() + BOARD_SOUTH] = BLACK_PAWN;
				++roleCounts[grid[myHistory[halfMoveClock].mHist.getTo() + BOARD_SOUTH]];

			}
			break;
		case NPROMOTE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_KNIGHT];
			break;
		case BPROMOTE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_BISHOP];
			break;
		case RPROMOTE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_ROOK];
			break;
		case QPROMOTE:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_QUEEN];
			break;
		case NPROMOTEC:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + WHITE_KNIGHT);
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getTo()]];
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_KNIGHT];
			break;
		case BPROMOTEC:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + WHITE_BISHOP);
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getTo()]];
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_BISHOP];
			break;
		case RPROMOTEC:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + WHITE_ROOK);
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getTo()]];
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_ROOK];
			break;
		case QPROMOTEC:
			grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + WHITE_QUEEN);
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getTo()]];
			++roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_QUEEN];
			break;
		}
		--halfMoveClock;
		allThreats();
	}

	void Board::allThreats()
	{
		pinCount = 0;
		memset(threatened, 0, sizeof(threatened));
		for (unsigned from = 0; from < SPACES; ++from) {
			if (grid[from] != EMPTY)
				pieceThreats(from);
		}
	}

	void Board::pieceThreats(int from)
	{//generates all pseudo legal moves for one piece
		int i, j;
		bool us = grid[from] & 1;
		switch (grid[from]) {
		case WHITE_KING:
		case BLACK_KING:
			if ((from + BOARD_SOUTHEAST) % WIDTH > from % WIDTH && from < 55)
				attackers[us][threatened[us][from + BOARD_SOUTHEAST]++][from + BOARD_SOUTHEAST] = from;
			if ((from + BOARD_EAST) % WIDTH > from % WIDTH)
				attackers[us][threatened[us][from + BOARD_EAST]++][from + BOARD_EAST] = from;
			if ((from + BOARD_NORTHWEST) % WIDTH < from % WIDTH && from > 8)
				attackers[us][threatened[us][from + BOARD_NORTHWEST]++][from + BOARD_NORTHWEST] = from;
			if ((from + BOARD_WEST) % WIDTH < from % WIDTH && from > 0)
				attackers[us][threatened[us][from + BOARD_WEST]++][from + BOARD_WEST] = from;
			if ((from + BOARD_SOUTHWEST) % WIDTH < from % WIDTH && from < 57)
				attackers[us][threatened[us][from + BOARD_SOUTHWEST]++][from + BOARD_SOUTHWEST] = from;
			if (from < 56)
				attackers[us][threatened[us][from + BOARD_SOUTH]++][from + BOARD_SOUTH] = from;
			if ((from + BOARD_NORTHEAST) % WIDTH > from % WIDTH && from > 6)
				attackers[us][threatened[us][from + BOARD_NORTHEAST]++][from + BOARD_NORTHEAST] = from;
			if (from > 7)
				attackers[us][threatened[us][from + BOARD_NORTH]++][from + BOARD_NORTH] = from;
			return;
		case WHITE_PAWN:
		case BLACK_PAWN:
			i = (us == BLACK) ? BOARD_SOUTH : BOARD_NORTH;
			if (from % WIDTH)
				attackers[us][threatened[us][from + i + BOARD_WEST]++][from + i + BOARD_WEST] = from;
			if (from % WIDTH != 7)
				attackers[us][threatened[us][from + i + BOARD_EAST]++][from + i + BOARD_EAST] = from;
			break;
		case WHITE_KNIGHT:
		case BLACK_KNIGHT:
			if ((from + 10) % WIDTH > from % WIDTH && from < 54)
				attackers[us][threatened[us][from + 10]++][from + 10] = from;
			if ((from + 17) % WIDTH > from % WIDTH && from < 47)
				attackers[us][threatened[us][from + 17]++][from + 17] = from;
			if ((from - 10) % WIDTH < from % WIDTH && from > 9)
				attackers[us][threatened[us][from - 10]++][from - 10] = from;
			if ((from - 17) % WIDTH < from % WIDTH && from > 16)
				attackers[us][threatened[us][from - 17]++][from - 17] = from;
			if ((from + 6) % WIDTH < from % WIDTH && from < 58)
				attackers[us][threatened[us][from + 6]++][from + 6] = from;
			if ((from + 15) % WIDTH < from % WIDTH && from < 49)
				attackers[us][threatened[us][from + 15]++][from + 15] = from;
			if ((from - 6) % WIDTH > from % WIDTH && from > 5)
				attackers[us][threatened[us][from - 6]++][from - 6] = from;
			if ((from - 15) % WIDTH > from % WIDTH && from > 14)
				attackers[us][threatened[us][from - 15]++][from - 15] = from;
			break;
		case WHITE_QUEEN:
		case WHITE_ROOK:
		case BLACK_QUEEN:
		case BLACK_ROOK:
			for (i = from + BOARD_NORTH; i >= 0; i += BOARD_NORTH) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i] != EMPTY) {
					if (NSslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTH; j != kingPos[turn]; j += BOARD_NORTH) {
							if (grid[j] != EMPTY) { goto failN; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_NORTH;
					}
					break;
				}
			}
		failN:
			for (i = from + BOARD_SOUTH; i < SPACES; i += BOARD_SOUTH) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i] != EMPTY) {
					if (NSslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTH; j != kingPos[turn]; j += BOARD_SOUTH) {
							if (grid[j] != EMPTY) { goto failS; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_SOUTH;
					}
					break;
				}
			}
		failS:
			for (i = from + BOARD_EAST; i % WIDTH; i += BOARD_EAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i] != EMPTY) {
					if (EWslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_EAST; j != kingPos[turn]; j += BOARD_EAST) {
							if (grid[j] != EMPTY) { goto failE; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_EAST;
					}
					break;
				}
			}
		failE:
			for (i = from + BOARD_WEST; i % WIDTH != 7 && i >= 0; i += BOARD_WEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i] != EMPTY) {
					if (EWslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_WEST; j != kingPos[turn]; j += BOARD_WEST) {
							if (grid[j] != EMPTY) { goto failW; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_WEST;
					}
					break;
				}
			}
		failW:
			if (grid[from] < WHITE_QUEEN) { break; }
		case WHITE_BISHOP:
		case BLACK_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i] != EMPTY) {
					if (DIAGslide(i, kingPos[turn]) && NESWslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTHEAST; j != kingPos[turn]; j += BOARD_NORTHEAST) {
							if (grid[j] != EMPTY) { goto failNE; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_NORTHEAST;
					}
					break;
				}
			}
		failNE:
			for (i = from + BOARD_NORTHWEST; i % WIDTH < from % WIDTH && i >= 0; i += BOARD_NORTHWEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i] != EMPTY) {
					if (DIAGslide(i, kingPos[turn]) && NWSEslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTHWEST; j != kingPos[turn]; j += BOARD_NORTHWEST) {
							if (grid[j] != EMPTY) { goto failNW; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_NORTHWEST;
					}
					break;
				}
			}
		failNW:
			for (i = from + BOARD_SOUTHEAST; i % WIDTH > from % WIDTH && i < SPACES; i += BOARD_SOUTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i] != EMPTY) {
					if (DIAGslide(i, kingPos[turn]) && NWSEslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHEAST; j != kingPos[turn]; j += BOARD_SOUTHEAST) {
							if (grid[j] != EMPTY) { goto failSE; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_SOUTHEAST;
					}
					break;
				}
			}
		failSE:
			for (i = from + BOARD_SOUTHWEST; i % WIDTH < from % WIDTH && i < SPACES; i += BOARD_SOUTHWEST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (grid[i] != EMPTY) {
					if (DIAGslide(i, kingPos[turn]) && NESWslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHWEST; j != kingPos[turn]; j += BOARD_SOUTHWEST) {
							if (grid[j] != EMPTY) { goto failSW; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_SOUTHWEST;
					}
					break;
				}
			}
		failSW:
			break;
		}
	}
}
