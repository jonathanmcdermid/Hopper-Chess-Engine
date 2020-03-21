#ifndef BOARD_H
#define BOARD_H

#include "move.h"
#include "zobrist.h"

namespace Chess {
	class board {
	public:
		board();
		void fenSet(std::string fs);
		bool checkMate();
		move createMove(uint8_t from, uint8_t to);
		bool movePiece(move m);
		void unmovePiece();
		int16_t negaEval();
		uint8_t moveTotal(uint8_t from);
		uint8_t getCturn() { return cturn; }
		int16_t getGrid(uint8_t i) { return grid[i]; }
		int16_t getvHist(uint8_t i) { return vHist[cturn - i - 1]; }
		move getmHist(uint8_t i) { return mHist[cturn - i - 1]; }
		bool getCheck() { return check; }
		bool getTurn() { return turn; }
		uint64_t getzHist(uint8_t i) { return zHist[cturn - i]; }
	private:
		zobrist z;
		uint8_t cturn;
		int16_t grid[SPACES] = { 0 };
		int16_t vHist[MEMORY] = { 0 };
		uint64_t zHist[MEMORY] = { 0 };
		move mHist[MEMORY] = { move() };
		bool check;
		bool turn;
		bool checkTurn();
	};
}

#endif
