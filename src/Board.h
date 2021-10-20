#pragma once

#include "Zobrist.h"
#include "Move.h"

namespace Hopper
{
	struct historyInfo {
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
	};

	class Board
	{
	public:
		Board();
		void fenSet(const char* fs);
		bool isCheckMate();
		bool isRepititionDraw();
		bool isPseudoRepititionDraw();
		bool isMaterialDraw()const;
		unsigned getGamePhase()const;
		bool isCheck() const { return threatened[!turn][kingPos[turn]]; }
		int getCurrC() const { return myHistory[halfMoveClock].cHist; }
		int getCurrV() const { return myHistory[halfMoveClock].vHist; }
		int getCurrF() const { return myHistory[halfMoveClock].fHist; }
		U64 getCurrZ() const { return myHistory[halfMoveClock].zHist; }
		U64 getCurrP() const { return myHistory[halfMoveClock].pHist; }
		Move getCurrM() const { return myHistory[halfMoveClock].mHist; }
		bool getTurn() const { return turn; }
		int getGridAt(int position)const { return (int)grid[position]; }
		int getThreatenedAt(bool team, int position)const { return threatened[team][position]; }
		int getAttackersAt(int x, int y, int z)const { return attackers[x][y][z]; }
		void setAttackersAt(int x, int y, int z, int val) { attackers[x][y][z] = val; }
		int getPinsAt(int position)const { return pinnedPieces[position]; }
		int getRolesAt(int position)const { return roleCounts[position]; }
		int getPinCount()const { return pinCount; }
		int getKingPosAt(bool team)const { return kingPos[team]; }
		void changeTurn() { turn = !turn; }
		void movePiece(Move nextMove);
		void unmovePiece();
		bool validateMove(const Move nextMove);
		Move createMove(int from, int to);
		unsigned genAllMoves(Move* nextMove);
		unsigned genAllCapMoves(Move* nextMove);
		unsigned genAllNonCapMoves(Move* nextMove);
		void drawBoard();
	private:
		void pieceThreats(int from);
		void allThreats();
		unsigned genCapMovesAt(Move* nextMove, int from);
		unsigned genNonCapMovesAt(Move* nextMove, int from);
		unsigned genAllMovesAt(Move* nextMove, int from);
		unsigned removeIllegalMoves(Move* nextMove, unsigned moveCount);
		Zobrist myZobrist;
		historyInfo myHistory[512];
		role_enum grid[SPACES];
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