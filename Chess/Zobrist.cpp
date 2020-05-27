#include <random>
#include "Board.h"
#include "Zobrist.h"

namespace Hopper {
	Zobrist::Zobrist() {//generates pseudo random template
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<std::uintmax_t> dis;
		side = dis(gen);
		for (int i = 0; i < 6; ++i) {
			for (int j = 0; j < 2; ++j) {
				for (int k = 0; k < SPACES; ++k) { pieces[i][j][k] = dis(gen); }
			}
		}
		for (int i = 0; i < SPACES; ++i) { enpassant[i] = dis(gen); }
		castle[WHITE][0] = dis(gen);
		castle[WHITE][1] = dis(gen);
		castle[BLACK][0] = dis(gen);
		castle[BLACK][1] = dis(gen);
	}

	U64 Zobrist::newKey(Board* b) {//XORs random template with board state and returns zobrist key
		U64 key = 0;
		for (int i = 0; i < SPACES; ++i) {
			if (b->grid[i] > 0) { key ^= pieces[ b->grid[i] % 10][WHITE][i]; }
			else if (b->grid[i] < 0) { key ^= pieces[-b->grid[i] % 10][BLACK][i]; }
		}
		if (b->getCurrC() & 1 << 2) {
			if (b->getCurrC() & 1 << 0) { key ^= castle[WHITE][0]; }
			if (b->getCurrC() & 1 << 1) { key ^= castle[WHITE][1]; }
		}
		if (b->getCurrC() & 1 << 4) {
			if (b->getCurrC() & 1 << 3) { key ^= castle[BLACK][0]; }
			if (b->getCurrC() & 1 << 5) { key ^= castle[BLACK][1]; }
		}
		if (b->getCurrM().getFlags() == DOUBLEPUSH) { key ^= enpassant[b->getCurrM().getTo()]; }
		if (b->turn) { key ^= side; }
		return key;
	}

	U64 Zobrist::newPawnKey(Board* b) {
		U64 key = 0;
		for (int i = 0; i < SPACES; i++) {
			if (b->grid[i] == W_PAWN) { key ^= pieces[PINDEX][WHITE][i]; }
			else if (b->grid[i] == B_PAWN) { key ^= pieces[PINDEX][BLACK][i]; }
		}
		return key;
	}
}