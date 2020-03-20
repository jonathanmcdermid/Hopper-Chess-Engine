#include <random>
#include "board.h"
#include "zobrist.h"

namespace Chess {

	zobrist::zobrist() {//generates pseudo random template
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<std::uintmax_t> dis;
		side = dis(gen);
		for (uint8_t i = 0; i < 6; ++i) {
			for (uint8_t j = 0; j < 2; ++j) {
				for (uint8_t k = 0; k < SPACES; ++k) { pieces[i][j][k] = dis(gen); }
			}
		}
		for (uint8_t i = 0; i < SPACES; ++i) { enpassant[i] = dis(gen); }
		for (uint8_t i = 0; i < 2; ++i) {
			blackcastle[i] = dis(gen);
			whitecastle[i] = dis(gen);
		}
	}

	uint64_t zobrist::newKey(board* b) {//XORs random template with board state and returns zobrist key
		uint64_t key = 0;
		for (uint8_t i = 0; i < SPACES; ++i) {
			if		(b->getGrid(i) > 0) { key ^= pieces[ b->getGrid(i) % 100][WHITE][i]; }
			else if (b->getGrid(i) < 0) { key ^= pieces[-b->getGrid(i) % 100][BLACK][i]; }
		}
		move m = move();
		uint8_t count = b->getCturn();
		bool castles[] = { true,true,true,true };
		for (uint8_t i = 0; i < count; ++i) {
			m = b->getmHist(i);
			if (m.getTo() == SPACES - 1		|| m.getFrom() == SPACES - 1) { castles[1] = false; }
			if (m.getTo() == SPACES - WIDTH || m.getFrom() == SPACES - WIDTH) { castles[0] = false; }
			if (m.getTo() == SPACES - 3		|| m.getFrom() == SPACES - 3) {
				castles[0] = false;
				castles[1] = false;
			}
			if (m.getTo() == WIDTH - 1 || m.getFrom() == WIDTH - 1) { castles[3] = false; }
			if (m.getTo() == 0 || m.getFrom() == 0) { castles[2] = false; }
			if (m.getTo() == 4 || m.getFrom() == 4) {
				castles[2] = false;
				castles[3] = false;
			}
		}
		if (!castles[0]) { key ^= whitecastle[0]; }
		if (!castles[1]) { key ^= whitecastle[1]; }
		if (!castles[2]) { key ^= blackcastle[0]; }
		if (!castles[3]) { key ^= blackcastle[1]; }
		if (m.getFlags() == DOUBLEPUSH) key ^= enpassant[m.getTo()];
		if (b->getTurn()) key ^= side;
		return key;
	}
}