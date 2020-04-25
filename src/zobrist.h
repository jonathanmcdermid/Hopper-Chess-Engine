#ifndef ZOBRIST_H
#define ZOBRIST_H

namespace Chess {
	class board;
	class zobrist {
	public:
		zobrist();
		U64 newKey(board* b);
		U64 newPawnKey(board* b);
		U64 pieces[6][2][SPACES];
		U64 castle[2][2];
		U64 enpassant[SPACES];
		U64 side = 0;
	};
}

#endif

