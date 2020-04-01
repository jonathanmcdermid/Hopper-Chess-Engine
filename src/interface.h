#include "board.h"
#include "bot.h"

#ifndef INTERFACE_H
#define INTERFACE_H

namespace Chess {
	class interface {
	public:
		interface(int argc, char* argv[]);
		void uci(int argc, char* argv[]);
		void go(std::istringstream& is);
		void position(std::istringstream& is);
		void local();
		bool playerMove(std::string input);
		void botMove();
		void getChoice();
		void drawBoard();
		void promote();
		void checkBoth();
	private:
		board game = board();
		bot ai;
		move m = move();
	};
}

#endif