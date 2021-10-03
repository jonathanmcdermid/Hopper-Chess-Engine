#pragma once

#include "zobrist.h"

namespace Hopper
{
	struct historyInfo {
		int fHist = 0;
		int cHist = 0;
		int vHist = 0;
		U64 zHist = 0;
		U64 pHist = 0;
		Move mHist = NULLMOVE;
		historyInfo(int f = 0, int c = 0, int v = 0, U64 z = 0, U64 p = 0, Move m = NULLMOVE) {
			fHist = f;
			cHist = c;
			vHist = v;
			zHist = z;
			pHist = p;
			mHist = m;
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
		bool isMaterialDraw();
		bool isEndgame();
		bool isCheck() const { return threatened[!turn * SPACES + kingPos[turn]]; }
		int getCurrC() const { return myHistory[halfMoveClock].cHist; }
		int getCurrV() const { return myHistory[halfMoveClock].vHist; }
		int getCurrF() const { return myHistory[halfMoveClock].fHist; }
		U64 getCurrZ() const { return myHistory[halfMoveClock].zHist; }
		U64 getCurrP() const { return myHistory[halfMoveClock].pHist; }
		Move getCurrM() const { return myHistory[halfMoveClock].mHist; }
		bool getTurn() const { return turn; }
		int getGridAt(int position)const { return (int)grid[position]; }
		int getThreatenedAt(bool team, int position)const { return threatened[team * SPACES + position]; }
		int getAttackersAt(int x, int y, int myZobrist)const { return attackers[x][y][myZobrist]; }
		void setAttackersAt(int x, int y, int myZobrist, int val) { attackers[x][y][myZobrist] = val; }
		int getPinsAt(int position)const { return pinnedPieces[position]; }
		int getRolesAt(int position)const { return roleCounts[position]; }
		int getPinCount()const { return pinCount; }
		int getKingPosAt(int position)const { return kingPos[position]; }
		void movePiece(Move nextMove);
		void unmovePiece();
		bool validateMove(const Move nextMove);
		Move createMove(int from, int to);
		int genAllMoves(Move* nextMove);
		int genAllCapMoves(Move* nextMove);
		int genAllNonCapMoves(Move* nextMove);
	private:
		void pieceThreats(int from);
		void allThreats();
		int genCapMovesAt(Move* nextMove, int from);
		int genNonCapMovesAt(Move* nextMove, int from);
		int genAllMovesAt(Move* nextMove, int from);
		int removeIllegalMoves(Move* nextMove, int moveCount);
		Zobrist myZobrist;
		historyInfo myHistory[512];
		role_enum grid[SPACES];
		int attackers[2][WIDTH][SPACES];
		int threatened[SPACES * 2];
		int pinnedPieces[10];
		int roleCounts[10];
		int kingPos[2];
		bool turn;
		int pinCount;
		int halfMoveClock;
	};
}