#pragma once

#include <sstream>
#include "Engine.h"

namespace Hopper
{
	class Interface
	{
	public:
		Interface(int argc, char* argv[]);
		void uci(int argc, char* argv[]);
		void go(std::istringstream& is);
		void position(std::istringstream& is);
	private:
		void drawBoard();
		void local();
		void self();
		bool playerMove(std::string input);
		void botMove();
		Board myBoard;
		Engine myEngine = Engine(&myBoard);
		Move nextMove;
	};
}