#include <string>
#include "interface.h"

namespace Chess{

	void interface::drawBoard() {//prints board in cmd
		char letter;
		std::cout << "\n  a   b   c   d   e   f   g   h";
		for (uint8_t i = 0; i < WIDTH; ++i) {
			std::cout << "\n---------------------------------\n|";
			for (uint8_t j = 0; j < WIDTH; ++j) {
				switch (game.getGrid(i * WIDTH + j)) {
				case PAWN: { letter = 'P'; break; }
				case ROOK: { letter = 'R'; break; }
				case KNIGHT: { letter = 'N'; break; }
				case BISHOP: { letter = 'B'; break; }
				case QUEEN: { letter = 'Q'; break; }
				case KING: { letter = 'K'; break; }
				case -PAWN: { letter = 'p'; break; }
				case -ROOK: { letter = 'r'; break; }
				case -KNIGHT: { letter = 'n'; break; }
				case -BISHOP: { letter = 'b'; break; }
				case -QUEEN: { letter = 'q'; break; }
				case -KING:	{ letter = 'k'; break; }
				default: { letter = ' '; }
				}
				std::cout << " " << letter << " |";
			}
			std::cout << " " << WIDTH - i;
		}
		std::cout << "\n---------------------------------\n";
	}

	interface::interface(){//awaits input from user or uci
		std::string input;
		while (1) {
			std::getline(std::cin, input);
			if (input == "uci") { uci(); }
			if (input == "local") { local(); }
			if (input == "quit") { exit(1); }
		}
	}

	void interface::local() {//for play without uci
		game = board();
		ai = bot::bot();
		std::string input;
		interface::drawBoard();
		while (1) {
			std::getline(std::cin, input);
			if (input.length()) {
				while (!playerMove(input)) { std::getline(std::cin, input); }
				interface::drawBoard();
				if (game.checkMate()) { break; }
				interface::botMove();
				interface::drawBoard();
				if (game.checkMate()) { break; }
			}
		}
		std::cout << "good game\n";
	}

	std::string interface::nextWord(std::string s, uint8_t* index) {//temporary fix to find next word from inut stream
		while (s.c_str()[*index] == ' ') { (*index)++; }
		uint8_t temp = *index;
		while (s.c_str()[*index] != '\0' && s.c_str()[*index] != '\n') {
			(*index)++;
			if (s.c_str()[*index] == ' ') { break; }
		}
		uint8_t temp2 = *index - temp;
		std::string w1 = s.substr(temp, temp2);
		return w1;
	}

	void interface::uci() {//communicates with uci
		std::cout << "id name chessbrainlet 1.0\nid author Jonathan M\n";
		std::cout << "uciok\n";
		while (1) {
			std::string input;
			std::string w1;
			uint8_t index = 0;
			std::getline(std::cin, input);
			w1 = nextWord(input, &index);
			if (w1 == "ucinewgame") { std::cout << "readyok\n"; }
			else if (w1 == "isready") { std::cout << "readyok\n"; }
			else if (w1 == "position") {
				w1 = nextWord(input, &index);
				if (w1 == "startpos") {
					game = board();
					ai = bot::bot();
				}
				w1 = nextWord(input, &index);
				if (w1 == "moves") {
					while (w1 != "\0") {
						w1 = nextWord(input, &index);
						playerMove(w1);
					}
				}
			}
			else if (w1 == "go") { 
				w1 = nextWord(input, &index);
				if (w1 == "wtime") {
					w1 = nextWord(input, &index);
					ai.lim.time[WHITE] = stoi(w1);
					w1 = nextWord(input, &index);
					if (w1 == "btime") {
						w1 = nextWord(input, &index);
						ai.lim.time[BLACK] = stoi(w1);
					}
				}
				botMove(); 
			}
			else if (input == "print") { drawBoard(); }
			else if (input == "quit") { exit(1); }
		}
	}


	bool interface::playerMove(std::string input) {//makes external moves
		if (input.length() == 4) {
			uint8_t from = (WIDTH - (input.c_str()[1] - '0')) * WIDTH + input.c_str()[0] - 'a';
			uint8_t to	 = (WIDTH - (input.c_str()[3] - '0')) * WIDTH + input.c_str()[2] - 'a';
			if (from >= 0 && from < SPACES && to >= 0 && to < SPACES) {
				m = game.createMove(from, to);
				if (m.getFlags() != FAIL && m.getFlags() < PROMOTE) {
					if (game.movePiece(m)) { return true; }
				}
			}
		}
		else if (input.length() == 5) {
			uint8_t from = (WIDTH - (input.c_str()[1] - '0')) * WIDTH + input.c_str()[0] - 'a';
			uint8_t to = (WIDTH - (input.c_str()[3] - '0')) * WIDTH + input.c_str()[2] - 'a';
			char flags = input.c_str()[4];
			if (from >= 0 && from < SPACES && to >= 0 && to < SPACES) {
				m = game.createMove(from, to);
				if (m.getFlags() >= PROMOTE) {
					switch (flags) {
					case 'n': { m = (m.getFlags() == QPROMOTEC) ? move(m.getFrom(), m.getTo(), NPROMOTEC) : move(m.getFrom(), m.getTo(), NPROMOTE); break; }
					case 'b': { m = (m.getFlags() == QPROMOTEC) ? move(m.getFrom(), m.getTo(), BPROMOTEC) : move(m.getFrom(), m.getTo(), BPROMOTE); break; }
					case 'r': { m = (m.getFlags() == QPROMOTEC) ? move(m.getFrom(), m.getTo(), RPROMOTEC) : move(m.getFrom(), m.getTo(), RPROMOTE); break; }
					case 'q': { m = (m.getFlags() == QPROMOTEC) ? move(m.getFrom(), m.getTo(), QPROMOTEC) : move(m.getFrom(), m.getTo(), QPROMOTE); break; }
					}
					if (game.movePiece(m)) { return true; }
				}
			}
		}
		return false;
	}

	void interface::botMove() {//generates internal moves
		m = ai.getMove(&game);
		game.movePiece(m);
		char message[] = { (int) WIDTH - m.getFrom() / WIDTH + '0', (int) m.getFrom() % WIDTH + 'a', (int) WIDTH - m.getTo() / WIDTH + '0', (int) m.getTo() % WIDTH + 'a' };
		std::cout << "bestmove " << message[1] << message[0] << message[3] << message[2];
		if (game.getmHist(game.getCturn()).getFlags() >= PROMOTE) { std::cout << "q"; }
		std::cout << "\n";
	}
}
