#ifndef BOARD_H
#define BOARD_H

#include "move.h"
#include "zobrist.h"

namespace Chess {
	class board {
	public:
		board() { fenSet(STARTFEN); }
		void fenSet(std::string fs);
		bool isDraw();
		bool isCheckMate();
		void drawBoard();
		bool isCheck() const { return threatened[!turn][kpos[turn]]; }
		U64 getCurrZ() const { return zHist[cturn - 1]; }
		int getCurrC() const { return cHist[cturn - 1]; }
		int getCurrV() const { return vHist[cturn - 1]; }
		int getCurrF() const { return fHist[cturn - 1]; }
		move getCurrM() const { return mHist[cturn - 1]; }
		move createMove(int from, int to);
		void movePiece(move m);
		void unmovePiece();
		bool validateMove(move m);
		int genAll(move* m);
		int genAllCaps(move* m);
		int genAllNonCaps(move* m);
		int removeIllegal(move* m, int cmove);
		void allThreats();
		void pieceThreats(int from);
		int pieceCapMoves(move* m, int from);
		int pieceNonCapMoves(move* m, int from);
		int pieceMoves(move* m, int from);
		int grid[SPACES];
		int threatened[2][SPACES];
		int attackers[2][WIDTH][SPACES];
		bool turn;
		bool endgame;
		bool zugswang;
		int pins[5][2];
		int cpins;
	private:
		int kpos[2];
		int cturn;
		int fHist[MEMORY];
		int cHist[MEMORY];
		int vHist[MEMORY];
		U64 zHist[MEMORY];
		move mHist[MEMORY];
		zobrist z;
	};
}

#endif
