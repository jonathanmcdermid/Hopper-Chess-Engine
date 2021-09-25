#include "Interface.h"
#include <string>
#include <sstream>
#include <iostream>

namespace Hopper
{
	Interface::Interface(int argc, char* argv[]) 
	{//awaits input from user or uci
		std::string input;
		while (1) 
		{
			std::getline(std::cin, input);
			if (input == "uci") 
				uci(argc, argv);
			if (input == "local")
				local();
			if (input == "self")
				self();
			if (input == "3d" || input == "3D")
			{
				std::cout << "Select opponent difficulty from 1 to 9\n";
				while (1) {
					std::getline(std::cin, input);
					if (input.at(0) > '0' && input.at(0) <= '9') {
						int time = (input.at(0) - '0') * 30000;
						ai.lim.time[WHITE] = time;
						ai.lim.time[BLACK] = time;
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
		while (is >> word) 
		{
			if (word == "wtime")		
				is >> ai.lim.time[WHITE];
			else if (word == "btime")		
				is >> ai.lim.time[BLACK];
			else if (word == "winc")		
				is >> ai.lim.inc[WHITE];
			else if (word == "binc")		
				is >> ai.lim.inc[BLACK];
			else if (word == "movestogo")	
				is >> ai.lim.movesleft;
			else if (word == "depth")		
				is >> ai.lim.depth;
			else if (word == "nodes")		
				is >> ai.lim.nodes;
			else if (word == "movetime")	
				is >> ai.lim.movetime;
			else if (word == "mate")		
				is >> ai.lim.mate;
			else if (word == "perft")		
				is >> ai.lim.perft;
			else if (word == "infinite")	
				ai.lim.infinite = true;
		}
		Interface::botMove();
	}

	void Interface::drawBoard() 
	{//prints board in cmd
		char letter;
		std::cout << "\n  a   b   c   d   e   f   g   h";
		for (int i = 0; i < WIDTH; ++i) 
		{
			std::cout << "\n---------------------------------\n|";
			for (int j = 0; j < WIDTH; ++j) 
			{
				switch (game.grid[(int) (i << 3) + j]) 
				{
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
		if (word == "startpos") 
		{
			fen = STARTFEN;
			is >> word;
		}
		else if (word == "fen") 
			while (is >> word && word != "moves") 
				fen += word + " ";
		else 
			return;
		game.fenSet((const char*)fen.c_str());
		while (is >> word) 
			if (!playerMove(word)) 
				return;
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
				game.fenSet(STARTFEN);
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
		while (1) 
		{
			std::getline(std::cin, input);
			if (input.length()) 
			{
				while (!playerMove(input)) 
					std::getline(std::cin, input);
				drawBoard();
				if (game.isCheckMate()) 
					break;
				Interface::botMove();
				drawBoard();
				if (game.isCheckMate()) 
					break;
			}
		}
		std::cout << "good game\n";
	}

	void Interface::self()
	{//for play without uci
		std::string input;
		drawBoard();
		while (1)
		{
			std::getline(std::cin, input);
			if (input.length())
			{
				while (!playerMove(input))
					std::getline(std::cin, input);
				drawBoard();
				if (game.isCheckMate())
					break;
			}
		}
		std::cout << "good game\n";
	}

	bool Interface::playerMove(std::string input) {//makes external moves
		if (input.length() == 4) 
		{
			int from = (WIDTH - (input.c_str()[1] - '0')) * WIDTH + input.c_str()[0] - 'a';
			int to	 = (WIDTH - (input.c_str()[3] - '0')) * WIDTH + input.c_str()[2] - 'a';
			if (from >= 0 && from < SPACES && to >= 0 && to < SPACES) 
			{
				m = game.createMove(from, to);
				if (m.getFlags() < NULLFLAGS) 
				{
					game.movePiece(m);
					return true;
				}
			}
		}
		else if (input.length() == 5) 
		{
			int from = (WIDTH - (input.c_str()[1] - '0')) * WIDTH + input.c_str()[0] - 'a';
			int to = (WIDTH - (input.c_str()[3] - '0')) * WIDTH + input.c_str()[2] - 'a';
			char flags = input.c_str()[4];
			if (from >= 0 && from < SPACES && to >= 0 && to < SPACES) 
			{
				m = game.createMove(from, to);
				if (m.getFlags() >= NPROMOTE) 
				{
					switch (flags) 
					{
					case 'n': 
						m = (m.getFlags() == QPROMOTEC) ? Move(m.getFrom(), m.getTo(), NPROMOTEC) : Move(m.getFrom(), m.getTo(), NPROMOTE); 
						break;
					case 'b': 
						m = (m.getFlags() == QPROMOTEC) ? Move(m.getFrom(), m.getTo(), BPROMOTEC) : Move(m.getFrom(), m.getTo(), BPROMOTE); 
						break;
					case 'r': 
						m = (m.getFlags() == QPROMOTEC) ? Move(m.getFrom(), m.getTo(), RPROMOTEC) : Move(m.getFrom(), m.getTo(), RPROMOTE); 
						break;
					case 'q': 
						m = (m.getFlags() == QPROMOTEC) ? Move(m.getFrom(), m.getTo(), QPROMOTEC) : Move(m.getFrom(), m.getTo(), QPROMOTE); 
						break;
					}
					game.movePiece(m); 
					return true;
				}
			}
		}
		else if (input == "fenset") 
		{
			std::getline(std::cin, input);
			game.fenSet(input.c_str());
			drawBoard();
		}
		else if (input == "unmove")
		{
			game.unmovePiece();
			drawBoard();
		}
		return false;
	}

	void Interface::botMove() 
	{//generates internal moves
		ai.makeMove();
		std::string message = { (char)(game.getCurrM().getFrom() % WIDTH + 'a'), (char)(WIDTH - game.getCurrM().getFrom() / WIDTH + '0'),(char)(game.getCurrM().getTo() % WIDTH + 'a'),(char)(WIDTH - game.getCurrM().getTo() / WIDTH + '0') };
		std::cout << "bestmove " << message;
		switch (game.getCurrM().getFlags()) 
		{
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