#include <cstring>
#include <iostream>
#include <cassert>
#include "Interface.h"
#include "tuner/tuner.h"

namespace Hopper
{
	Interface::Interface(int argc, char* argv[])
	{//awaits input from user or uci
		myThreads.push_back(Thread(&myBoard, 2));
		myThreads.push_back(Thread(&myBoard, 2));
		std::string input;
		std::cout << "Hopper Engine v1.8 by Jonathan McDermid\n";
		while (true) {
			std::getline(std::cin, input);
			if (input == "uci") {
				uci(argc, argv);
				input = "quit";
			}
			if (input == "local")
				local();
			if (input == "tuner")
				tuner();
			if (input == "self")
				self();
			if (input == "perft")
				myEngine.perftControl();
			if (input == "3d" || input == "3D") {
				std::cout << "Select opponent difficulty from 1 to 9\n";
				while (1) {
					std::getline(std::cin, input);
					if (input.at(0) > '0' && input.at(0) <= '9') {
						unsigned time = (input.at(0) - '0') * 30000;
						myEngine.myLimits.time[WHITE] = time;
						myEngine.myLimits.time[BLACK] = time;
						return;
					}
				}
			}
			if (input == "quit")
				exit(1);
		}
	}

	void Interface::go(std::istringstream& is)
	{//most options are not implemented yet
		std::string word;
		while (is >> word) {
			if (word == "wtime")
				is >> myEngine.myLimits.time[WHITE];
			else if (word == "btime")
				is >> myEngine.myLimits.time[BLACK];
			else if (word == "winc")
				is >> myEngine.myLimits.inc[WHITE];
			else if (word == "binc")
				is >> myEngine.myLimits.inc[BLACK];
			else if (word == "movestogo")
				is >> myEngine.myLimits.movesleft;
			else if (word == "depth")
				is >> myEngine.myLimits.depth;
			else if (word == "nodes")
				is >> myEngine.myLimits.nodes;
			else if (word == "movetime")
				is >> myEngine.myLimits.movetime;
			else if (word == "mate")
				is >> myEngine.myLimits.mate;
			else if (word == "perft")
				is >> myEngine.myLimits.perft;
			else if (word == "infinite")
				myEngine.myLimits.infinite = true;
		}
		Interface::botMove();
	}

	void Interface::position(std::istringstream& is)
	{
		std::string word, fen;
		is >> word;
		if (word == "startpos") {
			fen = STARTFEN;
			is >> word;
		}
		else if (word == "fen")
			while (is >> word && word != "moves")
				fen += word + " ";
		else
			return;
		myBoard.fenSet((const char*)fen.c_str());
		for (int i = 0; i < myThreads.size(); ++i)
			myThreads[i].myBoard.fenSet((const char*)fen.c_str());
		while (is >> word) {
			if (!playerMove(word)) {
				std::cout << word << " is not a valid move\n";
				exit(1);
			}
		}
	}

	void Interface::uci(int argc, char* argv[])
	{//uci communication loop, some options non functioning
		std::string word, cmd;
		std::cout << "id name Hopper Engine v1.8 \nid author Jonathan McDermid\n";
		std::cout << "option name Threads type spin default 1 min 1 max 10\n";
		std::cout << "uciok\n";
		for (int i = 1; i < argc; ++i)
			cmd += std::string(argv[i]) + " ";
		do {
			if (argc == 1 && !getline(std::cin, cmd))
				cmd = "quit";
			std::istringstream is(cmd);
			word.clear();
			is >> std::skipws >> word;
			if (word == "uci")
				std::cout << "id name Hopper Engine v1.8 \nid author Jonathan McDermid\nuciok\n";
			else if (word == "setoption")
				setOption(is);
			else if (word == "go")
				go(is);
			else if (word == "position")
				position(is);
			else if (word == "ucinewgame") {
				myBoard.fenSet(STARTFEN);
				for (int i = 0; i < myThreads.size(); ++i)
					myThreads[i].myBoard.fenSet(STARTFEN);
				myEngine.flushTT();
			}
			else if (word == "isready")
				std::cout << "readyok\n";
			else if (word == "print")
				myBoard.drawBoard();
		} while (word != "quit" && argc == 1);
	}

	void Interface::setOption(std::istringstream& is)
	{
		std::string word;
		int value;
		while (is >> word) {
			if (word == "Hash") {
				is >> word;
				is >> myEngine.myLimits.hashbytes;
			}
			else if (word == "Threads") {
				is >> word;
				is >> value;
				myThreads.resize(0);
				for (int i = 0; i < value; ++i)
					myThreads.push_back(Thread(&myBoard, value));
				std::cout << "info string set Threads to " << value << "\n";
			}
		}
	}

	void Interface::local()
	{//for play without uci
		std::string input;
		myBoard.drawBoard();
		while (1) {
			std::getline(std::cin, input);
			if (input.length()) {
				while (playerMove(input) == false)
					std::getline(std::cin, input);
				myBoard.drawBoard();
				if (myBoard.isCheckMate())
					break;
				Interface::botMove();
				myBoard.drawBoard();
				if (myBoard.isCheckMate())
					break;
			}
		}
		std::cout << "good game\n";
	}

	void Interface::tuner()
	{
		tune();
	}

	void Interface::self()
	{//for play without uci
		std::string input;
		myBoard.drawBoard();
		while (1) {
			std::getline(std::cin, input);
			if (input.length()) {
				while (!playerMove(input))
					std::getline(std::cin, input);
				myBoard.drawBoard();
				if (myBoard.isCheckMate() || myBoard.isMaterialDraw() || myBoard.isRepititionDraw())
					break;
			}
		}
		std::cout << "good game\n";
	}

	bool Interface::playerMove(std::string input) {//makes external moves
		if (input.length() == 4) {
			unsigned from = (WIDTH - (input.c_str()[1] - '0')) * WIDTH + input.c_str()[0] - 'a';
			unsigned to = (WIDTH - (input.c_str()[3] - '0')) * WIDTH + input.c_str()[2] - 'a';
			if (from >= 0 && from < SPACES && to >= 0 && to < SPACES) {
				nextMove = myBoard.createMove(from, to);
				if (nextMove.getFlags() < NULLFLAGS) {
					myBoard.movePiece(nextMove);
					for (int i = 0; i < myThreads.size(); ++i)
						myThreads[i].myBoard.movePiece(nextMove);
					return true;
				}
			}
		}
		else if (input.length() == 5)
		{
			unsigned from = (WIDTH - (input.c_str()[1] - '0')) * WIDTH + input.c_str()[0] - 'a';
			unsigned to = (WIDTH - (input.c_str()[3] - '0')) * WIDTH + input.c_str()[2] - 'a';
			char flags = input.c_str()[4];
			if (from >= 0 && from < SPACES && to >= 0 && to < SPACES) {
				nextMove = myBoard.createMove(from, to);
				if (nextMove.getFlags() >= NPROMOTE) {
					switch (flags) {
					case 'n':
						nextMove = (nextMove.getFlags() == QPROMOTEC) ? 
							Move(nextMove.getFrom(), nextMove.getTo(), NPROMOTEC) : 
							Move(nextMove.getFrom(), nextMove.getTo(), NPROMOTE);
						break;
					case 'b':
						nextMove = (nextMove.getFlags() == QPROMOTEC) ? 
							Move(nextMove.getFrom(), nextMove.getTo(), BPROMOTEC) : 
							Move(nextMove.getFrom(), nextMove.getTo(), BPROMOTE);
						break;
					case 'r':
						nextMove = (nextMove.getFlags() == QPROMOTEC) ? 
							Move(nextMove.getFrom(), nextMove.getTo(), RPROMOTEC) : 
							Move(nextMove.getFrom(), nextMove.getTo(), RPROMOTE);
						break;
					case 'q':
						nextMove = (nextMove.getFlags() == QPROMOTEC) ? 
							Move(nextMove.getFrom(), nextMove.getTo(), QPROMOTEC) : 
							Move(nextMove.getFrom(), nextMove.getTo(), QPROMOTE);
						break;
					}
					for (int i = 0; i < myThreads.size(); ++i)
						myThreads[i].myBoard.movePiece(nextMove);
					myBoard.movePiece(nextMove);
					return true;
				}
			}
		}
		else if (input == "fenset") {
			std::getline(std::cin, input);
			myBoard.fenSet(input.c_str());
			for (int i = 0; i < myThreads.size(); ++i)
				myThreads[i].myBoard.fenSet(input.c_str());
			myBoard.drawBoard();
		}
		else if (input == "unmove") {
			myBoard.unmovePiece();
			for (int i = 0; i < myThreads.size(); ++i)
				myThreads[i].myBoard.unmovePiece();
			myBoard.drawBoard();
		}
		return false;
	}

	void Interface::botMove()
	{//generates internal moves
		Move bestMove = myEngine.getBestMove(&myThreads[0]);
		myBoard.movePiece(bestMove);
		for (int i = 0; i < myThreads.size(); ++i)
			myThreads[i].myBoard.movePiece(bestMove);
		std::string message = { 
			(char)(myBoard.getCurrM().getFrom() % WIDTH + 'a'), 
			(char)(WIDTH - myBoard.getCurrM().getFrom() / WIDTH + '0'),
			(char)(myBoard.getCurrM().getTo() % WIDTH + 'a'),
			(char)(WIDTH - myBoard.getCurrM().getTo() / WIDTH + '0') 
		};
		std::cout << "bestmove " << message;
		switch (myBoard.getCurrM().getFlags()) {
		case NPROMOTE:
		case NPROMOTEC:
			std::cout << "n\n";
			break;
		case BPROMOTE:
		case BPROMOTEC:
			std::cout << "b\n";
			break;
		case RPROMOTE:
		case RPROMOTEC:
			std::cout << "r\n";
			break;
		case QPROMOTE:
		case QPROMOTEC:
			std::cout << "q\n";
			break;
		default:
			std::cout << "\n";
		}
	}
}