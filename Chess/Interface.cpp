#include "Interface.h"
#include <string>
#include <sstream>
#include <iostream>

namespace Hopper
{
	Interface::Interface(int argc, char* argv[])
	{//awaits input from user or uci
		std::string input;
		while (1) {
			std::getline(std::cin, input);
			if (input == "uci")
				uci(argc, argv);
			if (input == "local")
				local();
			if (input == "self")
				self();
			if (input == "3d" || input == "3D") {
				std::cout << "Select opponent difficulty from 1 to 9\n";
				while (1) {
					std::getline(std::cin, input);
					if (input.at(0) > '0' && input.at(0) <= '9') {
						int time = (input.at(0) - '0') * 30000;
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

	void Interface::drawBoard()
	{//prints board in cmd
		char letter;
		std::cout << "\n  a   b   c   d   e   f   g   h";
		for (int i = 0; i < WIDTH; ++i) {
			std::cout << "\n---------------------------------\n|";
			for (int j = 0; j < WIDTH; ++j) {
				switch (myBoard.getGridAt(i * WIDTH + j)) {
				case W_PAWN:
					letter = 'P'; break;
				case W_ROOK:
					letter = 'R'; break;
				case W_KNIGHT:
					letter = 'N'; break;
				case W_BISHOP:
					letter = 'B'; break;
				case W_QUEEN:
					letter = 'Q'; break;
				case W_KING:
					letter = 'K'; break;
				case B_PAWN:
					letter = 'p'; break;
				case B_ROOK:
					letter = 'r'; break;
				case B_KNIGHT:
					letter = 'n'; break;
				case B_BISHOP:
					letter = 'b'; break;
				case B_QUEEN:
					letter = 'q'; break;
				case B_KING:
					letter = 'k'; break;
				default:
					letter = ' ';
				}
				std::cout << " " << letter << " |";
			}
			std::cout << " " << WIDTH - i;
		}
		std::cout << "\n---------------------------------\n";
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
		while (is >> word) {
			if (!playerMove(word))
				return;
		}
	}

	void Interface::uci(int argc, char* argv[])
	{//uci communication loop, some options non functioning
		std::string word, cmd;
		std::cout << "id name Hopper Engine \nid author Jonathan M\nuciok\n";
		for (int i = 1; i < argc; ++i)
			cmd += std::string(argv[i]) + " ";
		do {
			if (argc == 1 && !getline(std::cin, cmd))
				cmd = "quit";
			std::istringstream is(cmd);
			word.clear();
			is >> std::skipws >> word;
			if (word == "uci")
				std::cout << "id name chessbrainlet 1.0\nid author Jonathan M\nuciok\n";
			else if (word == "go")
				go(is);
			else if (word == "position")
				position(is);
			else if (word == "ucinewgame")
				myBoard.fenSet(STARTFEN);
			else if (word == "isready")
				std::cout << "readyok\n";
			else if (word == "print")
				drawBoard();
		} while (word != "quit" && argc == 1);
	}

	void Interface::local()
	{//for play without uci
		std::string input;
		drawBoard();
		while (1) {
			std::getline(std::cin, input);
			if (input.length()) {
				while (!playerMove(input))
					std::getline(std::cin, input);
				drawBoard();
				if (myBoard.isCheckMate())
					break;
				Interface::botMove();
				drawBoard();
				if (myBoard.isCheckMate())
					break;
			}
		}
		std::cout << "good game\n";
	}

	void Interface::self()
	{//for play without uci
		std::string input;
		drawBoard();
		while (1) {
			std::getline(std::cin, input);
			if (input.length()) {
				while (!playerMove(input))
					std::getline(std::cin, input);
				drawBoard();
				if (myBoard.isCheckMate() || myBoard.isMaterialDraw() || myBoard.isRepititionDraw())
					break;
			}
		}
		std::cout << "good game\n";
	}

	bool Interface::playerMove(std::string input) {//makes external moves
		if (input.length() == 4) {
			int from = (WIDTH - (input.c_str()[1] - '0')) * WIDTH + input.c_str()[0] - 'a';
			int to = (WIDTH - (input.c_str()[3] - '0')) * WIDTH + input.c_str()[2] - 'a';
			if (from >= 0 && from < SPACES && to >= 0 && to < SPACES) {
				nextMove = myBoard.createMove(from, to);
				if (nextMove.getFlags() < NULLFLAGS) {
					myBoard.movePiece(nextMove);
					return true;
				}
			}
		}
		else if (input.length() == 5)
		{
			int from = (WIDTH - (input.c_str()[1] - '0')) * WIDTH + input.c_str()[0] - 'a';
			int to = (WIDTH - (input.c_str()[3] - '0')) * WIDTH + input.c_str()[2] - 'a';
			char flags = input.c_str()[4];
			if (from >= 0 && from < SPACES && to >= 0 && to < SPACES) {
				nextMove = myBoard.createMove(from, to);
				if (nextMove.getFlags() >= NPROMOTE) {
					switch (flags) {
					case 'n':
						nextMove = (nextMove.getFlags() == QPROMOTEC) ? Move(nextMove.getFrom(), nextMove.getTo(), NPROMOTEC) : Move(nextMove.getFrom(), nextMove.getTo(), NPROMOTE);
						break;
					case 'b':
						nextMove = (nextMove.getFlags() == QPROMOTEC) ? Move(nextMove.getFrom(), nextMove.getTo(), BPROMOTEC) : Move(nextMove.getFrom(), nextMove.getTo(), BPROMOTE);
						break;
					case 'r':
						nextMove = (nextMove.getFlags() == QPROMOTEC) ? Move(nextMove.getFrom(), nextMove.getTo(), RPROMOTEC) : Move(nextMove.getFrom(), nextMove.getTo(), RPROMOTE);
						break;
					case 'q':
						nextMove = (nextMove.getFlags() == QPROMOTEC) ? Move(nextMove.getFrom(), nextMove.getTo(), QPROMOTEC) : Move(nextMove.getFrom(), nextMove.getTo(), QPROMOTE);
						break;
					}
					myBoard.movePiece(nextMove);
					return true;
				}
			}
		}
		else if (input == "fenset") {
			std::getline(std::cin, input);
			myBoard.fenSet(input.c_str());
			drawBoard();
		}
		else if (input == "unmove") {
			myBoard.unmovePiece();
			drawBoard();
		}
		return false;
	}

	void Interface::botMove()
	{//generates internal moves
		myEngine.makeMove();
		std::string message = { (char)(myBoard.getCurrM().getFrom() % WIDTH + 'a'), (char)(WIDTH - myBoard.getCurrM().getFrom() / WIDTH + '0'),(char)(myBoard.getCurrM().getTo() % WIDTH + 'a'),(char)(WIDTH - myBoard.getCurrM().getTo() / WIDTH + '0') };
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