#pragma once

#include "move.h"

namespace Hopper {
	class Board;
	class Zobrist {
	public:
		Zobrist();
		U64 newKey(Board* b);
		U64 newPawnKey(Board* b);
		U64 pieces[6][2][SPACES] = { 0 };
		U64 castle[2][2] = { 0 };
		U64 enpassant[SPACES] = { 0 };
		U64 side = 0;
	};
}