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
		turn = dis(gen);
		for (unsigned i = 0; i < 6 * 2 * SPACES; ++i)
			pieces[i] = dis(gen);
		for (unsigned i = 0; i < SPACES; ++i)
			enPassant[i] = dis(gen);
		for (unsigned i = 0; i < 4; ++i)
			castle[i] = dis(gen);
	}

	U64 Zobrist::newKey(Board* b)
	{//XORs random template with board state and returns zobrist key
		U64 key = 0;
		for (unsigned i = 0; i < SPACES; ++i) {
			if (b->getGridAt(i))
				key ^= piecesAt(abs(b->getGridAt(i)) % 10, b->getGridAt(i) > 0, i);
		}
		for (unsigned i = 0; i < 4; ++i) {
			if (b->getCurrC() & 1 << i)
				key ^= castle[i];
		}
		if (b->getCurrM().getFlags() == DOUBLEPUSH)
			key ^= enPassant[b->getCurrM().getTo()];
		if (b->getTurn())
			key ^= turn;
		return key;
	}

	U64 Zobrist::newPawnKey(Board* b)
	{
		U64 key = 0;
		for (unsigned i = 0; i < SPACES; i++) {
			if (b->getGridAt(i) == W_PAWN)
				key ^= piecesAt(PINDEX, WHITE, i);
			else if (b->getGridAt(i) == B_PAWN)
				key ^= piecesAt(PINDEX, BLACK, i);
		}
		return key;
	}
}