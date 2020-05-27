#pragma once

#include "zobrist.h"

namespace Hopper {
	class Board {
	public:
		Board();
		void fenSet(const char* fs);
		bool isCheckMate();
		bool isDraw();
		bool insufficientMaterial();
		bool isEndgame();
		bool isCheck() const { return threatened[!turn][kpos[turn]]; }
		int getCurrC() const { return cHist[cturn - 1]; }
		int getCurrV() const { return vHist[cturn - 1]; }
		int getCurrF() const { return fHist[cturn - 1]; }
		U64 getCurrZ() const { return zHist[cturn - 1]; }
		U64 getCurrP() const { return pHist[cturn - 1]; }
		Move getCurrM() const { return mHist[cturn - 1]; }
		void movePiece(Move m);
		void unmovePiece();
		void allThreats();
		void pieceThreats(int from);
		bool validateMove(Move m);
		Move createMove(int from, int to);
		int removeIllegal(Move* m, int cmove);
		int genAll(Move* m);
		int genAllCaps(Move* m);
		int genAllNonCaps(Move* m);
		int pieceCapMoves(Move* m, int from);
		int pieceNonCapMoves(Move* m, int from);
		int pieceMoves(Move* m, int from);
		enum::role_enum grid[SPACES];
		enum::side_enum turn;
		int threatened[2][SPACES];
		int attackers[2][WIDTH][SPACES];
		int pins[5][2];
		int roles[2][5];
		int cpins;
		int kpos[2];
	private:
		Zobrist z;
		int cturn;
		int fHist[MEMORY];
		int cHist[MEMORY];
		int vHist[MEMORY];
		U64 zHist[MEMORY];
		U64 pHist[MEMORY];
		Move mHist[MEMORY];
	};
}
