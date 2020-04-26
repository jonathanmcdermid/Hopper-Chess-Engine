#ifndef BOARD_H
#define BOARD_H

#include "move.h"
#include "zobrist.h"

namespace Chess {
	class interface;
	class board {
	public:
		board(interface* ifx);
		void fenSet(std::string fs);
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
		move getCurrM() const { return mHist[cturn - 1]; }
		void movePiece(move m);
		void unmovePiece();
		void allThreats();
		void pieceThreats(int from);
		bool validateMove(move m);
		move createMove(int from, int to);
		int removeIllegal(move* m, int cmove);
		int genAll(move* m);
		int genAllCaps(move* m);
		int genAllNonCaps(move* m);
		int pieceCapMoves(move* m, int from);
		int pieceNonCapMoves(move* m, int from);
		int pieceMoves(move* m, int from);
		int grid[SPACES];
		int threatened[2][SPACES];
		int attackers[2][WIDTH][SPACES];
		bool turn;
		int pins[5][2];
		int cpins;
		int kpos[2];
		interface* fx;
	private:
		int roles[2][5];
		zobrist z;
		int cturn;
		int fHist[MEMORY];
		int cHist[MEMORY];
		int vHist[MEMORY];
		U64 zHist[MEMORY];
		U64 pHist[MEMORY];
		move mHist[MEMORY];
	};
}

#endif
