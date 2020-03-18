#include "board.h"
#include "bot.h"

#ifndef INTERFACE_H
#define INTERFACE_H

namespace Chess {
	class interface {
	public:
		interface();
		static std::string nextWord(std::string s, uint8_t* index);
		void local();
		bool playerMove(std::string input);
		void botMove();
		void getChoice();
		void drawBoard();
		void promote();
		void checkBoth();
		void uci();
	private:
		board game;
		bot ai;
		move m;
	};
}

#endif