#pragma once

namespace Hopper {
	class Board;
	class Evaluate {
	public:
		Evaluate(Board* bd) { b = bd; }
		int negaEval();
		int pawnEval();
		static int hypotenuse(int a, int b);
	private:
		Board* b;
	};
}

