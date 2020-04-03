#ifndef BOARD_H
#define BOARD_H

#include "move.h"
#include "zobrist.h"

namespace Chess {
	class board {
	public:
		board();
		void fenSet(std::string fs);
		bool twofoldRep() const { return (cturn > 3 && zHist[cturn - 4] == currZ) ? true : false; }
		bool checkMate() { return (checkTeam(!turn) || !cmove) ? true : false; }
		bool checkTeam(bool team) const { return (team) ? threatened[BLACK][kpos[WHITE]] : threatened[WHITE][kpos[BLACK]]; }
		move createMove(int from, int to);
		void drawBoard();
		bool movePiece(move m);
		void unmovePiece();
		void generateMoves();
		void pieceMoves(int from);
		int grid[SPACES];
		int threatened[2][SPACES];
		bool turn;
		bool endgame;
		int cmove;
		int currC;
		int currV;
		U64 currZ;
		move currM;
		move possiblemoves[MEMORY];
	private:
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
