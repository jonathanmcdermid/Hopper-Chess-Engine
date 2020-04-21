#ifndef BOARD_H
#define BOARD_H

#include "move.h"
#include "zobrist.h"

namespace Chess {
	class board {
	public:
		board();
		void fenSet(std::string fs);
		bool isDraw();
		bool isCheckMate();
		bool isCheck() const { return threatened[!turn][kpos[turn]]; }
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
		int currC;
		int currV;
		U64 currZ;
		move currM;
		int pins[5][2];
		int cpins;
		int fmr;
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
