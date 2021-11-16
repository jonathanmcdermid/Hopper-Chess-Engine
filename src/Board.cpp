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
				switch (myPosition.grid[i * WIDTH + j]) {
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
		myPosition = position(fs);
		unsigned index = 0, counter = 0;
		while (fs[index] != ' ') {
			switch (fs[index]) {
			case 'P':
				++roleCounts[WHITE_PAWN];
				++counter;
				break;
			case 'R':
				++roleCounts[WHITE_ROOK];
				++counter;
				break;
			case 'N':
				++roleCounts[WHITE_KNIGHT];
				++counter;
				break;
			case 'B':
				++roleCounts[WHITE_BISHOP];
				++counter;
				break;
			case 'Q':
				++roleCounts[WHITE_QUEEN];
				++counter;
				break;
			case 'K':
				kingPos[WHITE] = counter;
				++counter;
				break;
			case 'p':
				++roleCounts[BLACK_PAWN];
				++counter;
				break;
			case 'r':
				++roleCounts[BLACK_ROOK];
				++counter;
				break;
			case 'n':
				++roleCounts[BLACK_KNIGHT];
				++counter;
				break;
			case 'b':
				++roleCounts[BLACK_BISHOP];
				++counter;
				break;
			case 'q':
				++roleCounts[BLACK_QUEEN];
				++counter;
				break;
			case 'k':
				kingPos[BLACK] = counter;
				++counter;
				break;
			case '/':
				break;
			default:
				++counter;
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
			myHistory[halfMoveClock].vHist += myPosition.grid[i];
		myHistory[halfMoveClock].zHist = myZobrist.newKey(this);
		myHistory[halfMoveClock].pHist = myZobrist.newPawnKey(this);
		allThreats();
	}

	bool Board::isCheckMate()
	{
		scoredMove nextMove[28];
		unsigned moveCount;
		for (unsigned i = 0; i < SPACES; ++i) {
			if (validPiece(myPosition.grid[i], turn)) {
				moveCount = genAllMovesAt(nextMove, i);
				if (moveCount) {
					if (removeIllegalMoves(nextMove, moveCount)) { return false; }
				}
			}
		}
		return true;
	}

	bool Board::isRepititionDraw()const
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

	bool Board::isPseudoRepititionDraw()const
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
				roleCounts[WHITE_KNIGHT] > 2 ||
				roleCounts[WHITE_BISHOP] + roleCounts[WHITE_KNIGHT] / 2 > 1 ||
				roleCounts[BLACK_KNIGHT] > 2 ||
				roleCounts[BLACK_BISHOP] + roleCounts[BLACK_KNIGHT] / 2 > 1) == false;
	}

	void Board::movePiece(Move nextMove)
	{//executes a move if legal
		myHistory[halfMoveClock + 1] = historyInfo(
			(nextMove.getFlags() == STANDARD && (myPosition.grid[nextMove.getFrom()] != WHITE_PAWN && myPosition.grid[nextMove.getTo()] != BLACK_PAWN)) ? 
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
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = myPosition.grid[nextMove.getFrom()];
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			if (myPosition.grid[nextMove.getTo()] == WHITE_KING || myPosition.grid[nextMove.getTo()] == BLACK_KING)
				kingPos[turn] = nextMove.getTo();
			else if (myPosition.grid[nextMove.getTo()] == WHITE_PAWN || myPosition.grid[nextMove.getTo()] == BLACK_PAWN) {
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getFrom());
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			}
			break;
		case DOUBLEPUSH:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.enPassantAt(nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = myPosition.grid[nextMove.getFrom()];
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getFrom());
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			break;
		case KCASTLE:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo() + 1], nextMove.getTo() + 1);
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo() + 1], nextMove.getTo() - 1);
			myPosition.grid[nextMove.getTo()] = myPosition.grid[nextMove.getFrom()];
			myPosition.grid[nextMove.getTo() - 1] = myPosition.grid[nextMove.getTo() + 1];
			myPosition.grid[nextMove.getTo() + 1] = EMPTY;
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			kingPos[turn] = nextMove.getTo();
			break;
		case QCASTLE:
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo() - 2], nextMove.getTo() - 2);
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo() - 2], nextMove.getTo() + 1);
			myPosition.grid[nextMove.getTo()] = myPosition.grid[nextMove.getFrom()];
			myPosition.grid[nextMove.getTo() + 1] = myPosition.grid[nextMove.getTo() - 2];
			myPosition.grid[nextMove.getTo() - 2] = EMPTY;
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			kingPos[turn] = nextMove.getTo();
			break;
		case CAPTURE:
			--roleCounts[myPosition.grid[nextMove.getTo()]];
			if (myPosition.grid[nextMove.getTo()] == WHITE_PAWN || myPosition.grid[nextMove.getTo()] == BLACK_PAWN) 
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].vHist -= myPosition.grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = myPosition.grid[nextMove.getFrom()];
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			if (myPosition.grid[nextMove.getTo()] == WHITE_PAWN || myPosition.grid[nextMove.getTo()] == BLACK_PAWN) {
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getFrom());
				myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			}
			else if (myPosition.grid[nextMove.getTo()] == WHITE_KING || myPosition.grid[nextMove.getTo()] == BLACK_KING) { kingPos[turn] = nextMove.getTo(); }
			break;
		case ENPASSANT:
			--roleCounts[WHITE_PAWN + turn];
			myHistory[halfMoveClock].vHist -= (turn == BLACK) ? WHITE_PAWN : BLACK_PAWN;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= (turn) ? myZobrist.piecesAt(WHITE_PAWN, nextMove.getTo() + BOARD_NORTH) : myZobrist.piecesAt(BLACK_PAWN, nextMove.getTo() + BOARD_SOUTH);
			myPosition.grid[nextMove.getTo()] = myPosition.grid[nextMove.getFrom()];
			if (turn == BLACK)
				myPosition.grid[nextMove.getTo() + BOARD_NORTH] = EMPTY;
			else 
				myPosition.grid[nextMove.getTo() + BOARD_SOUTH] = EMPTY;
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getFrom());
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].pHist ^= (turn == BLACK) ? 
				myZobrist.piecesAt(WHITE_PAWN, nextMove.getTo() + BOARD_NORTH) : 
				myZobrist.piecesAt(BLACK_PAWN, nextMove.getTo() + BOARD_SOUTH);
			break;
		case NPROMOTE:
			--roleCounts[myPosition.grid[nextMove.getFrom()]];
			++roleCounts[myPosition.grid[nextMove.getFrom()] + WHITE_KNIGHT];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_KNIGHT;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()] + WHITE_KNIGHT, nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = (role_enum)(myPosition.grid[nextMove.getFrom()] + WHITE_KNIGHT);
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			break;
		case BPROMOTE:
			--roleCounts[myPosition.grid[nextMove.getFrom()]];
			++roleCounts[myPosition.grid[nextMove.getFrom()] + WHITE_BISHOP];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_BISHOP;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()] + WHITE_BISHOP, nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = (role_enum)(myPosition.grid[nextMove.getFrom()] + WHITE_BISHOP);
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			break;
		case RPROMOTE:
			--roleCounts[myPosition.grid[nextMove.getFrom()]];
			++roleCounts[myPosition.grid[nextMove.getFrom()] + WHITE_ROOK];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_ROOK;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()] + WHITE_ROOK, nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = (role_enum)(myPosition.grid[nextMove.getFrom()] + WHITE_ROOK);
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			break;
		case QPROMOTE:
			--roleCounts[myPosition.grid[nextMove.getFrom()]];
			++roleCounts[myPosition.grid[nextMove.getFrom()] + WHITE_QUEEN];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_QUEEN;
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()] + WHITE_QUEEN, nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = (role_enum)(myPosition.grid[nextMove.getFrom()] + WHITE_QUEEN);
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			break;
		case NPROMOTEC:
			--roleCounts[myPosition.grid[nextMove.getTo()]];
			--roleCounts[myPosition.grid[nextMove.getFrom()]];
			++roleCounts[myPosition.grid[nextMove.getFrom()] + WHITE_KNIGHT];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_KNIGHT - myPosition.grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()] + WHITE_KNIGHT, nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = (role_enum)(myPosition.grid[nextMove.getFrom()] + WHITE_KNIGHT);
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			break;
		case BPROMOTEC:
			--roleCounts[myPosition.grid[nextMove.getTo()]];
			--roleCounts[myPosition.grid[nextMove.getFrom()]];
			++roleCounts[myPosition.grid[nextMove.getFrom()] + WHITE_BISHOP];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_BISHOP - myPosition.grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()] + WHITE_BISHOP, nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = (role_enum)(myPosition.grid[nextMove.getFrom()] + WHITE_BISHOP);
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			break;
		case RPROMOTEC:
			--roleCounts[myPosition.grid[nextMove.getTo()]];
			--roleCounts[myPosition.grid[nextMove.getFrom()]];
			++roleCounts[myPosition.grid[nextMove.getFrom()] + WHITE_ROOK];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_ROOK - myPosition.grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()] + WHITE_ROOK, nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = (role_enum)(myPosition.grid[nextMove.getFrom()] + WHITE_ROOK);
			myPosition.grid[nextMove.getFrom()] = EMPTY;
			break;
		case QPROMOTEC:
			--roleCounts[myPosition.grid[nextMove.getTo()]];
			--roleCounts[myPosition.grid[nextMove.getFrom()]];
			++roleCounts[myPosition.grid[nextMove.getFrom()] + WHITE_QUEEN];
			myHistory[halfMoveClock].pHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].vHist += WHITE_QUEEN - myPosition.grid[nextMove.getTo()];
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()], nextMove.getFrom());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getTo()], nextMove.getTo());
			myHistory[halfMoveClock].zHist ^= myZobrist.piecesAt(myPosition.grid[nextMove.getFrom()] + WHITE_QUEEN, nextMove.getTo());
			myPosition.grid[nextMove.getTo()] = (role_enum) (myPosition.grid[nextMove.getFrom()] + WHITE_QUEEN);
			myPosition.grid[nextMove.getFrom()] = EMPTY;
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
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = myPosition.grid[myHistory[halfMoveClock].mHist.getTo()];
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			if (myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] == WHITE_KING || myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] == BLACK_KING) 
				kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom(); 
			break;
		case DOUBLEPUSH:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = myPosition.grid[myHistory[halfMoveClock].mHist.getTo()];
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			break;
		case KCASTLE:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = myPosition.grid[myHistory[halfMoveClock].mHist.getTo()];
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo() + 1] = myPosition.grid[myHistory[halfMoveClock].mHist.getTo() - 1];
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo() - 1] = EMPTY;
			kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom();
			break;
		case QCASTLE:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = myPosition.grid[myHistory[halfMoveClock].mHist.getTo()];
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo() - 2] = myPosition.grid[myHistory[halfMoveClock].mHist.getTo() + 1];
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo() + 1] = EMPTY;
			kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom();
			break;
		case CAPTURE:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = myPosition.grid[myHistory[halfMoveClock].mHist.getTo()];
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist);
			if (myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] == WHITE_KING || myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] == BLACK_KING) 
				kingPos[turn] = myHistory[halfMoveClock].mHist.getFrom();
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getTo()]];
			break;
		case ENPASSANT:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = myPosition.grid[myHistory[halfMoveClock].mHist.getTo()];
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			if (turn == BLACK) {
				myPosition.grid[myHistory[halfMoveClock].mHist.getTo() + BOARD_NORTH] = WHITE_PAWN;
				++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getTo() + BOARD_NORTH]];
			}
			else {
				myPosition.grid[myHistory[halfMoveClock].mHist.getTo() + BOARD_SOUTH] = BLACK_PAWN;
				++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getTo() + BOARD_SOUTH]];

			}
			break;
		case NPROMOTE:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_KNIGHT];
			break;
		case BPROMOTE:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_BISHOP];
			break;
		case RPROMOTE:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_ROOK];
			break;
		case QPROMOTE:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = EMPTY;
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_QUEEN];
			break;
		case NPROMOTEC:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + WHITE_KNIGHT);
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getTo()]];
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_KNIGHT];
			break;
		case BPROMOTEC:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + WHITE_BISHOP);
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getTo()]];
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_BISHOP];
			break;
		case RPROMOTEC:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + WHITE_ROOK);
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getTo()]];
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_ROOK];
			break;
		case QPROMOTEC:
			myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] = (turn) ? BLACK_PAWN : WHITE_PAWN;
			myPosition.grid[myHistory[halfMoveClock].mHist.getTo()] = (role_enum)(myHistory[halfMoveClock - 1].vHist - myHistory[halfMoveClock].vHist + WHITE_QUEEN);
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getTo()]];
			++roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()]];
			--roleCounts[myPosition.grid[myHistory[halfMoveClock].mHist.getFrom()] + WHITE_QUEEN];
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
			if (myPosition.grid[from] != EMPTY)
				pieceThreats(from);
		}
	}

	void Board::pieceThreats(int from)
	{//generates all pseudo legal moves for one piece
		int i, j;
		bool us = myPosition.grid[from] & 1;
		switch (myPosition.grid[from]) {
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
				if (myPosition.grid[i] != EMPTY) {
					if (NSslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTH; j != kingPos[turn]; j += BOARD_NORTH) {
							if (myPosition.grid[j] != EMPTY) { goto failN; }
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
				if (myPosition.grid[i] != EMPTY) {
					if (NSslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTH; j != kingPos[turn]; j += BOARD_SOUTH) {
							if (myPosition.grid[j] != EMPTY) { goto failS; }
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
				if (myPosition.grid[i] != EMPTY) {
					if (EWslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_EAST; j != kingPos[turn]; j += BOARD_EAST) {
							if (myPosition.grid[j] != EMPTY) { goto failE; }
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
				if (myPosition.grid[i] != EMPTY) {
					if (EWslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_WEST; j != kingPos[turn]; j += BOARD_WEST) {
							if (myPosition.grid[j] != EMPTY) { goto failW; }
						}
						pinnedPieces[pinCount] = i;
						pinnedPieces[5 + pinCount++] = BOARD_WEST;
					}
					break;
				}
			}
		failW:
			if (myPosition.grid[from] < WHITE_QUEEN) { break; }
		case WHITE_BISHOP:
		case BLACK_BISHOP:
			for (i = from + BOARD_NORTHEAST; i % WIDTH > from % WIDTH && i >= 0; i += BOARD_NORTHEAST) {
				attackers[us][threatened[us][i]++][i] = from;
				if (myPosition.grid[i] != EMPTY) {
					if (DIAGslide(i, kingPos[turn]) && NESWslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTHEAST; j != kingPos[turn]; j += BOARD_NORTHEAST) {
							if (myPosition.grid[j] != EMPTY) { goto failNE; }
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
				if (myPosition.grid[i] != EMPTY) {
					if (DIAGslide(i, kingPos[turn]) && NWSEslide(i, kingPos[turn]) && i > kingPos[turn] && turn != us) {
						for (j = i + BOARD_NORTHWEST; j != kingPos[turn]; j += BOARD_NORTHWEST) {
							if (myPosition.grid[j] != EMPTY) { goto failNW; }
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
				if (myPosition.grid[i] != EMPTY) {
					if (DIAGslide(i, kingPos[turn]) && NWSEslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHEAST; j != kingPos[turn]; j += BOARD_SOUTHEAST) {
							if (myPosition.grid[j] != EMPTY) { goto failSE; }
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
				if (myPosition.grid[i] != EMPTY) {
					if (DIAGslide(i, kingPos[turn]) && NESWslide(i, kingPos[turn]) && i < kingPos[turn] && turn != us) {
						for (j = i + BOARD_SOUTHWEST; j != kingPos[turn]; j += BOARD_SOUTHWEST) {
							if (myPosition.grid[j] != EMPTY) { goto failSW; }
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
