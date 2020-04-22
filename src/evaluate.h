#ifndef EVALUATE_H
#define EVALUATE_H

namespace Chess {
	class board;
	class evaluate {
	public:
		evaluate(board* bd){ b = bd; }
		int negaEval();
	private:
		board* b;
	};
}
#endif

