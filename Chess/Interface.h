#pragma once

#include "Bot.h"
#include <string>

namespace Hopper 
{
	class Interface 
	{
	public:
		Interface(int argc, char* argv[]);
		~Interface() { ; }
		void uci(int argc, char* argv[]);
		void go(std::istringstream& is);
		void position(std::istringstream& is);
	private:
		void drawBoard();
		void local();
		void self();
		bool playerMove(std::string input);
		void botMove();
		Board game;
		Bot ai = Bot(&game);
		Move m;
	};
}