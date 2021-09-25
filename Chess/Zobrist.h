#pragma once

#include "move.h"

namespace Hopper 
{
	class Board;
	class Zobrist 
	{
	public:
		Zobrist();
		~Zobrist() { ; }
		U64 newKey(Board* b);
		U64 newPawnKey(Board* b);
		U64 piecesAt(int x, int y, int z)const { return pieces[x + 6 * (y + z * 2)]; }
		U64 castle[4];
		U64 enpassant[SPACES];
		U64 side;
		U64 pieces[6 * 2 * SPACES];
	};
}