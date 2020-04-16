#ifndef BOARD_H
#define BOARD_H

#include "move.h"
#include "zobrist.h"

namespace Chess {
	class board {
	public:
		board();
		void fenSet(std::string fs);
		bool twofoldRep() const { return zHist[cturn - 4] == currZ; }
		bool checkMate();
		bool checkTurn() const { return cpos[0] != NOCHECK; }
		move createMove(int from, int to);
		void drawBoard();
		void movePiece(move m);
		void unmovePiece();
		bool validateMove(move m);
		int genAll(move* m);
		int genAllCaps(move* m);
		int genAllNonCaps(move* m);
		int removeIllegal(move* m, int cmove);
		void allThreats();
		void pieceThreats(int from);
		int pieceMoves(move* m, int from);
		int pieceCapMoves(move* m, int from);
		int pieceNonCapMoves(move* m, int from);
		int grid[SPACES];
		int threatened[2][SPACES];
		bool turn;
		bool endgame;
		int currC;
		int currV;
		U64 currZ;
		move currM;
		int pins[5][2];
		int cpins;
	private:
		int cpos[2];
		int kpos[2];
		int cturn;
		U64 zHist[MEMORY];
		int cHist[MEMORY];
		int vHist[MEMORY];
		move mHist[MEMORY];
		zobrist z;
	};
}

#endif
