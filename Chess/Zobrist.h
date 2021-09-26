#pragma once

#include "move.h"

namespace Hopper 
{
	class Board;
	class Zobrist
	{
	public:
		Zobrist();
		U64 newKey(Board* b);
		U64 newPawnKey(Board* b);
		U64 piecesAt(int x, int y, int z)const { return pieces[x + 6 * (y + z * 2)]; }
		U64 castleAt(int x)const { return castle[x]; }
		U64 enPassantAt(int x)const { return enPassant[x]; }
		U64 sideAt()const { return side; }
	private:
		U64 castle[4];
		U64 enPassant[SPACES];
		U64 side;
		U64 pieces[6 * 2 * SPACES];
	};
}