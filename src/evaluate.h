#ifndef EVALUATE_H
#define EVALUATE_H

#include "move.h"

namespace Hopper {
	class board;
	class evaluate {
	public:
		evaluate(board* bd) { b = bd; }
		int negaEval();
		int pawnEval();
		static int hypotenuse(int a, int b);
		int kingsafety(bool team);
	private:
		board* b;
	};
}
#endif

