#ifndef ZOBRIST_H
#define ZOBRIST_H

namespace Chess {
	class board;
	class zobrist {
	public:
		zobrist();
		unsigned long long newKey(board* b);
		unsigned long long pieces[6][2][SPACES] = { 0 };
		unsigned long long castle[2][2] = { 0 };
		unsigned long long enpassant[SPACES] = { 0 };
		unsigned long long side = 0;
	};
}

#endif

