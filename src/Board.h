#pragma once

#include <cstring>
#include "Zobrist.h"
#include "Move.h"

namespace Hopper
{
	typedef struct historyInfo {
		unsigned fHist = 0;
		unsigned cHist = 0;
		int vHist = 0;
		U64 zHist = 0;
		U64 pHist = 0;
		Move mHist = NULLMOVE;
		historyInfo(unsigned f = 0, unsigned c = 0, int v = 0, U64 z = 0, U64 p = 0, Move m = NULLMOVE) {
			this->fHist = f;
			this->cHist = c;
			this->vHist = v;
			this->zHist = z;
			this->pHist = p;
			this->mHist = m;
		}
		void operator=(historyInfo h) {
			this->fHist = h.fHist;
			this->cHist = h.cHist;
			this->vHist = h.vHist;
			this->zHist = h.zHist;
			this->pHist = h.pHist;
			this->mHist = h.mHist;
		}
	}historyInfo;

	typedef struct position {
		position(const char* fs = STARTFEN) {
			unsigned index = 0;
			unsigned counter = 0;
			unsigned helper = 0;
			while (fs[index] != ' ') {
				switch (fs[index]) {
				case 'P':grid[counter++] = WHITE_PAWN; break;
				case 'R':grid[counter++] = WHITE_ROOK; break;
				case 'N':grid[counter++] = WHITE_KNIGHT; break;
				case 'B':grid[counter++] = WHITE_BISHOP; break;
				case 'Q':grid[counter++] = WHITE_QUEEN; break;
				case 'K':grid[counter++] = WHITE_KING; break;
				case 'p':grid[counter++] = BLACK_PAWN; break;
				case 'r':grid[counter++] = BLACK_ROOK; break;
				case 'n':grid[counter++] = BLACK_KNIGHT; break;
				case 'b':grid[counter++] = BLACK_BISHOP; break;
				case 'q':grid[counter++] = BLACK_QUEEN; break;
				case 'k':grid[counter++] = BLACK_KING; break;
				case '/': break;
				default:
					helper = fs[index] - '0';
					while (helper--) { grid[counter++] = EMPTY; }
				}
				++index;
			}
		}
		void operator=(const position& rhs) {
			for (int i = 0; i < SPACES; ++i)
				grid[i] = rhs.grid[i];
		}
		bool operator!=(const position& rhs) {
			for (int i = 0; i < SPACES; ++i)
				if(grid[i] != rhs.grid[i]) return true;
			return false;
		}
		role_enum grid[SPACES] = { EMPTY };
	}position;

	class Board
	{
	public:
		Board();
		void operator=(Board b);
		void fenSet(const char* fs);
		bool isCheckMate();
		bool isRepititionDraw()const;
		bool isPseudoRepititionDraw()const;
		bool isMaterialDraw()const;
		bool isCheck() const { return threatened[!turn][kingPos[turn]]; }
		int getCurrC() const { return myHistory[halfMoveClock].cHist; }
		int getCurrV() const { return myHistory[halfMoveClock].vHist; }
		int getCurrF() const { return myHistory[halfMoveClock].fHist; }
		U64 getCurrZ() const { return myHistory[halfMoveClock].zHist; }
		U64 getCurrP() const { return myHistory[halfMoveClock].pHist; }
		Move getCurrM() const { return myHistory[halfMoveClock].mHist; }
		bool getTurn() const { return turn; }
		int getGridAt(int pos)const { return (int)myPosition.grid[pos]; }
		int getThreatenedAt(bool team, int pos)const { return threatened[team][pos]; }
		int getAttackersAt(int x, int y, int z)const { return attackers[x][y][z]; }
		void setAttackersAt(int x, int y, int z, int val) { attackers[x][y][z] = val; }
		int getPinsAt(int pos)const { return pinnedPieces[pos]; }
		int getRolesAt(int pos)const { return roleCounts[pos]; }
		int getPinCount()const { return pinCount; }
		int getKingPosAt(bool team)const { return kingPos[team]; }
		void changeTurn() { turn = !turn; }
		void movePiece(Move nextMove);
		void unmovePiece();
		bool validateMove(const Move nextMove);
		Move createMove(int from, int to);
		unsigned genAllMoves(scoredMove* nextMove);
		unsigned genAllCapMoves(scoredMove* nextMove);
		unsigned genAllNonCapMoves(scoredMove* nextMove);
		void drawBoard();
		position myPosition;
	private:
		void pieceThreats(int from);
		void allThreats();
		unsigned genCapMovesAt(scoredMove* nextMove, int from);
		unsigned genNonCapMovesAt(scoredMove* nextMove, int from);
		unsigned genAllMovesAt(scoredMove* nextMove, int from);
		unsigned removeIllegalMoves(scoredMove* nextMove, unsigned moveCount);
		Zobrist myZobrist;
		historyInfo myHistory[512];
		int attackers[2][WIDTH][SPACES];
		int threatened[2][SPACES];
		int pinnedPieces[10];
		int roleCounts[10];
		int kingPos[2];
		bool turn;
		int pinCount;
		int halfMoveClock;
	};
}