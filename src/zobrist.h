#ifndef ZOBRIST_H
#define ZOBRIST_H

namespace Chess {
	class board;
	class zobrist {
	public:
		zobrist();
		U64 newKey(board* b);
		U64 pieces[6][2][SPACES] = { 0 };
		U64 castle[2][2] = { 0 };
		U64 enpassant[SPACES] = { 0 };
		U64 side = 0;
	};
}

#endif

