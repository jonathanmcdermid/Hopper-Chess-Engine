#include <random>
#include "board.h"
#include "zobrist.h"

namespace Chess {

	zobrist::zobrist() {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<std::uintmax_t> dis;
		side = dis(gen);
		for (uint8_t i = 0; i < 6; i++) {
			for (uint8_t j = 0; j < 2; j++) {
				for (uint8_t k = 0; k < SPACES; k++) {
					pieces[i][j][k] = dis(gen);
				}
			}
		}
		for (uint8_t i = 0; i < SPACES; i++) {
			enpassant[i] = dis(gen);
		}
		for (uint8_t i = 0; i < 2; i++) {
			lCastle[i] = dis(gen);
			cCastle[i] = dis(gen);
		}
	}

	uint64_t zobrist::newKey(board* b) {
		uint64_t key = 0;
		for (uint8_t i = 0; i < SPACES; i++) {
			if		(b->getGrid(i) > 0) { key ^= pieces[ b->getGrid(i) % 100][1][i]; }
			else if (b->getGrid(i) < 0) { key ^= pieces[-b->getGrid(i) % 100][0][i]; }
		}
		move m = move();
		uint8_t count = b->getCturn();
		bool temp[] = { true,true,true,true };
		for (uint8_t i = 0; i < count; i++) {
			m = b->getmHist(i);
			if (m.getTo() == SPACES - 1		|| m.getFrom() == SPACES - 1) { temp[1] = false; }
			if (m.getTo() == SPACES - WIDTH || m.getFrom() == SPACES - WIDTH) { temp[0] = false; }
			if (m.getTo() == SPACES - 3		|| m.getFrom() == SPACES - 3) {
				temp[0] = false;
				temp[1] = false;
			}
			if (m.getTo() == WIDTH - 1 || m.getFrom() == WIDTH - 1) { temp[3] = false; }
			if (m.getTo() == 0 || m.getFrom() == 0) { temp[2] = false; }
			if (m.getTo() == 4 || m.getFrom() == 4) {
				temp[2] = false;
				temp[3] = false;
			}
		}
		if (!temp[0]) { key ^= cCastle[0]; }
		if (!temp[1]) { key ^= cCastle[1]; }
		if (!temp[2]) { key ^= lCastle[0]; }
		if (!temp[3]) { key ^= lCastle[1]; }
		if (m.getFlags() == DOUBLEPUSH) key ^= enpassant[m.getTo()];
		if (b->getTurn()) key ^= side;
		return key;
	}
}