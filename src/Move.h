#pragma once

#include "Macros.h"

namespace Hopper
{
	typedef unsigned long long U64;

	class Move
	{
	public:
		Move(int from, int to, int flags) { rawMove = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f); }
		Move(int n) { rawMove = n; }
		Move() { rawMove = NULLMOVE; }
		void operator=(int a) { rawMove = a; }
		bool operator==(int a) const { return rawMove == a; }
		bool operator!=(int a) const { return rawMove != a; }
		void operator=(Move a) { rawMove = a.rawMove; }
		bool operator==(Move a) const { return (rawMove & 0xffff) == (a.rawMove & 0xffff); }
		bool operator!=(Move a) const { return (rawMove & 0xffff) != (a.rawMove & 0xffff); }
		bool isCap() const { return rawMove != 28672 && ((rawMove >> 12) & 0x0f) & 1 << 2; }
		int getTo() const { return rawMove & 0x3f; }
		int getFrom() const { return (rawMove >> 6) & 0x3f; }
		int getFlags() const { return (rawMove >> 12) & 0x0f; }
	private:
		int rawMove;
	};

	typedef struct scoredMove {
		scoredMove(unsigned u = 0, Move k = NULLMOVE) {
			score = u;
			myMove = k;
		}
		void operator=(const scoredMove& rhs) {
			score = rhs.score;
			myMove = rhs.myMove;
		}
		bool operator!=(const scoredMove& rhs) {
			return score != rhs.score || myMove != rhs.myMove;
		}
		int score = 0;
		Move myMove = NULLMOVE;
	}scoredMove;

	inline static bool smFlagsComp(scoredMove const& lhs, scoredMove const& rhs) {
		return lhs.myMove.getFlags() > rhs.myMove.getFlags();
	}

	inline static bool smScoreComp(scoredMove const& lhs, scoredMove const& rhs) {
		return lhs.score > rhs.score;
	}
}