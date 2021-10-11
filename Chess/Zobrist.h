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
		U64 piecesAt(unsigned p, unsigned z)const { return zobristPieces[p][z]; }
		U64 castleAt(unsigned x)const { return zobristCastle[x]; }
		U64 enPassantAt(unsigned x)const { return zobristEnPassantFlag[x]; }
		U64 sideAt()const { return zobristTurn; }
	private:
		U64 zobristCastle[4];
		U64 zobristEnPassantFlag[SPACES];
		U64 zobristTurn;
		U64 zobristPieces[12][SPACES];
	};
}