#ifndef EVALUATE_H
#define EVALUATE_H
namespace Chess {
	class board;
	class evaluate{
	public:
		evaluate() { b = nullptr; }
		evaluate(board* bp) { b = bp; }
		int negaEval();
	private:
		board* b;
	};
}
#endif

