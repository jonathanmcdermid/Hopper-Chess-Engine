#ifndef BOARD_H
#define BOARD_H

#include "move.h"
#include "zobrist.h"

namespace Chess {
	class board {
	public:
		board();
		void fenSet(std::string fs);
		unsigned long long getzHist(int i) const { return (i > cturn) ? 0 : zHist[cturn - i]; }
		move getmHist(int i) const { return (i > cturn) ? move() : mHist[i]; }
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
		unsigned long long currZ;
		move currM;
		move possiblemoves[MEMORY];
	private:
		int kpos[2];
		int cturn;
		unsigned long long zHist[MEMORY];
		int cHist[MEMORY];
		int vHist[MEMORY];
		move mHist[MEMORY];
		zobrist z;
	};
}

#endif
