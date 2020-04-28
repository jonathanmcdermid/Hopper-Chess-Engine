#ifndef EVALUATE_H
#define EVALUATE_H

#include "move.h"

namespace Chess {
	class board;
	class evaluate {
	public:
		evaluate(board* bd) { b = bd; }
		int negaEval();
		int pawnEval();
	private:
		board* b;
	};
}
#endif

