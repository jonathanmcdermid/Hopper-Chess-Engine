#include "board.h"
#include "bot.h"

#ifndef INTERFACE_H
#define INTERFACE_H

namespace Chess {
	class interface {
	public:
		interface(int argc, char* argv[]);
		void drawBoard();
		void uci(int argc, char* argv[]);
		void go(std::istringstream& is);
		void position(std::istringstream& is);
		void local();
		bool playerMove(std::string input);
		void botMove();
		void getChoice();
		void promote();
		void checkBoth();
	private:
		board game = board(this);
		bot ai = bot(this, &game);
		move m;
	};
}

#endif