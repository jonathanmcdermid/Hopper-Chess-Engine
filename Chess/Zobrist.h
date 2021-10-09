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
		U64 piecesAt(unsigned x, unsigned y, unsigned myZobrist)const { return pieces[x + 6 * (y + myZobrist * 2)]; }
		U64 castleAt(unsigned x)const { return castle[x]; }
		U64 enPassantAt(unsigned x)const { return enPassant[x]; }
		U64 sideAt()const { return turn; }
	private:
		U64 castle[4];
		U64 enPassant[SPACES];
		U64 turn;
		U64 pieces[6 * 2 * SPACES];
	};
}