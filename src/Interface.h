#pragma once

#include <sstream>
#include <vector>
#include "Engine.h"

namespace Hopper
{
	class Interface
	{
	public:
		Interface(int argc, char* argv[]);
	private:
		void go(std::istringstream& is);
		void position(std::istringstream& is);
		void tuner();
		void local();
		void self();
		bool playerMove(std::string input);
		void botMove();
		void setOption(std::istringstream& is);
		Board myBoard;
		Engine myEngine;
		std::vector<Thread> myThreads;
		Move nextMove;
	};
}