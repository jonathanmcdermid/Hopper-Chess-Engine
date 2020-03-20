#ifndef ZOBRIST_H
#define ZOBRIST_H

namespace Chess {
	class board;
	class zobrist {
	public:
		zobrist();
		uint64_t newKey(board* b);
		uint64_t pieces[6][2][SPACES];
	private:
		uint64_t whitecastle[2];
		uint64_t blackcastle[2];
		uint64_t enpassant[SPACES];
		uint64_t side;
	};
}

#endif

