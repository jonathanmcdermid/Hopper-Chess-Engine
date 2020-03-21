#ifndef ZOBRIST_H
#define ZOBRIST_H

namespace Chess {
	class board;
	class zobrist {
	public:
		zobrist();
		uint64_t newKey(board* b);
		uint64_t pieces[6][2][SPACES] = { 0 };
	private:
		uint64_t whitecastle[2] = { 0 };
		uint64_t blackcastle[2] = { 0 };
		uint64_t enpassant[SPACES] = { 0 };
		uint64_t side = 0;
	};
}

#endif

