#include <random>
#include "Board.h"
#include "Zobrist.h"

namespace Hopper
{
	Zobrist::Zobrist()
	{//generates pseudo random template
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<std::uintmax_t> dis;
		zobristTurn = dis(gen);
		for (unsigned i = 0; i < 12; ++i)
			for(unsigned j = 0; j < SPACES; ++j)
				zobristPieces[i * SPACES + j] = dis(gen);
		for (unsigned i = 0; i < SPACES; ++i)
			zobristEnPassantFlag[i] = dis(gen);
		for (unsigned i = 0; i < 4; ++i)
			zobristCastle[i] = dis(gen);
	}

	U64 Zobrist::newKey(Board* b)
	{//XORs random template with board state and returns zobrist key
		U64 key = 0;
		for (unsigned i = 0; i < SPACES; ++i) {
			if (b->getGridAt(i) != EMPTY)
				key ^= piecesAt(b->getGridAt(i), i);
		}
		for (unsigned i = 0; i < 4; ++i) {
			if (b->getCurrC() & 1 << i)
				key ^= zobristCastle[i];
		}
		if (b->getCurrM().getFlags() == DOUBLEPUSH)
			key ^= zobristEnPassantFlag[b->getCurrM().getTo()];
		if (b->getTurn() == BLACK)
			key ^= zobristTurn;
		return key;
	}

	U64 Zobrist::newPawnKey(Board* b)
	{
		U64 key = 0;
		for (unsigned i = 0; i < SPACES; i++) {
			if (b->getGridAt(i) == WHITE_PAWN)
				key ^= piecesAt(WHITE_PAWN, i);
			else if (b->getGridAt(i) == BLACK_PAWN)
				key ^= piecesAt(BLACK_PAWN, i);
		}
		return key;
	}
}