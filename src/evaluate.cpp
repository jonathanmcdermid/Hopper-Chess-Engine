#include "evaluate.h"
#include "board.h"
#include "interface.h"

namespace Chess {
	static int WPAWNBIT[SPACES] = {
		 0,  0,  0,  0,  0,  0,  0,  0,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		 5,  5, 10, 25, 25, 10,  5,  5,
		 0,  0,  0, 20, 20,  0,  0,  0,
		 5, -5,-10,  0,  0,-10, -5,  5,
		 5, 10, 10,-20,-20, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	static int BPAWNBIT[SPACES] = {
		 0,  0,  0,  0,  0,  0,  0,  0,
		 5, 10, 10,-20,-20, 10, 10,  5,
		 5, -5,-10,  0,  0,-10, -5,  5,
		 0,  0,  0, 20, 20,  0,  0,  0,
		 5,  5, 10, 25, 25, 10,  5,  5,
		10, 10, 20, 30, 30, 20, 10, 10,
		50, 50, 50, 50, 50, 50, 50, 50,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	static int WKNIGHTBIT[SPACES] = {
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	};

	static int BKNIGHTBIT[SPACES] = {
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	};

	static int WBISHOPBIT[SPACES] = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20,
	};

	static int BBISHOPBIT[SPACES] = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	};

	static int WROOKBIT[SPACES] = {
		 0,  0,  0,  0,  0,  0,  0,  0,
		 5, 10, 10, 10, 10, 10, 10,  5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		 0,  0,  0,  5,  5,  0,  0,  0
	};

	static int BROOKBIT[SPACES] = {
		 0,  0,  0,  5,  5,  0,  0,  0,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		 5, 10, 10, 10, 10, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	static int WQUEENBIT[SPACES] = {
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};

	static int BQUEENBIT[SPACES] = {
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-10,  5,  5,  5,  5,  5,  0,-10,
		  0,  0,  5,  5,  5,  5,  0, -5,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};

	static int WKINGBIT[SPACES] = {
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		 20, 20,  0,  0,  0,  0, 20, 20,
		 20, 30, 10,  0,  0, 10, 30, 20
	};

	static int BKINGBIT[SPACES] = {
		 20, 30, 10,  0,  0, 10, 30, 20,
		 20, 20,  0,  0,  0,  0, 20, 20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30
	};

	static int WENDKINGBIT[SPACES] = {
		-50,-40,-30,-20,-20,-30,-40,-50,
		-30,-20,-10,  0,  0,-10,-20,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-30,  0,  0,  0,  0,-30,-30,
		-50,-30,-30,-30,-30,-30,-30,-50
	};

	static int BENDKINGBIT[SPACES] = {
		-50,-30,-30,-30,-30,-30,-30,-50,
		-30,-30,  0,  0,  0,  0,-30,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-20,-10,  0,  0,-10,-20,-30,
		-50,-40,-30,-20,-20,-30,-40,-50
	};

	int evaluate::negaEval() {//negamax evaluation using material sum of pieces and bonus boards
		if (b->insufficientMaterial()) { return CONTEMPT; }
		int sum = 0;
		bool endgame = b->isEndgame();
		for (int i = 0; i < SPACES; ++i) {
			switch (b->grid[i]) {
			case EMPTY:	
				break;
			case PAWN: 
				sum += WPAWNBIT[i]; 
				break;
			case -PAWN:	
				sum -= BPAWNBIT[i];	
				break;
			case KNIGHT: 
				sum += WKNIGHTBIT[i]; 
				break;
			case -KNIGHT: 
				sum -= BKNIGHTBIT[i]; 
				break;
			case BISHOP: 
				sum += WBISHOPBIT[i]; 
				break;
			case -BISHOP: 
				sum -= BBISHOPBIT[i]; 
				break;
			case ROOK: 
				sum += WROOKBIT[i]; 
				break;
			case -ROOK:	
				sum -= BROOKBIT[i];	
				break;
			case QUEEN:	
				sum += WQUEENBIT[i]; 
				break;
			case -QUEEN: 
				sum -= BQUEENBIT[i]; 
				break;
			case KING: 
				sum += (endgame) ? WENDKINGBIT[i] : WKINGBIT[i]; 
				break;
			case -KING:	
				sum += (endgame) ? BENDKINGBIT[i] : BKINGBIT[i]; 
				break;
			}
		}
		if (b->roles[WHITE][BINDEX] > 1) { sum += BISHOPPAIR; }
		if (b->roles[BLACK][BINDEX] > 1) { sum -= BISHOPPAIR; }
		return (b->turn) ? b->getCurrV() + sum : -b->getCurrV() - sum;
	}

	int evaluate::pawnEval() {
		int sum = 0;
		int cfile[2][WIDTH];
		int rank[2][WIDTH][3];
		int helper;
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < WIDTH; ++j) {
				cfile[i][j] = 0;
				for (int k = 0; k < 3; ++k) {
					rank[i][j][k] = 0;
				}
			}
		}
		for (int i = 0; i < SPACES; ++i) {
			if (b->grid[i] == PAWN) { 
				rank[WHITE][i % WIDTH][cfile[WHITE][i % WIDTH]++] = i / WIDTH;
			}
			else if(b->grid[i] == -PAWN) { 
				rank[BLACK][i % WIDTH][cfile[BLACK][i % WIDTH]++] = i / WIDTH; 
			}
		}
		for (int file = 0; file < WIDTH; ++file) {
			if (cfile[WHITE][file] > 1) {
				if (cfile[WHITE][file] == 2) { sum += DOUBLED; }
				else { TRIPLED; }
			}
			for (int index = 0; index < cfile[WHITE][file]; ++index) {
				switch (file) {
				case 0:
					if (!cfile[WHITE][file + 1]) { sum += ISOLATED * cfile[WHITE][file]; }
					if (!cfile[BLACK][file] && !cfile[BLACK][file + 1]) { sum += PASSED * (WIDTH - rank[WHITE][file][index]) * (WIDTH - rank[WHITE][file][index]); }
					for (helper = 0; helper < cfile[WHITE][file + 1]; ++helper) {
						if (abs(rank[WHITE][file][index] - rank[WHITE][file + 1][helper]) < 2) {
							//if (!abs(rank[i][j][k] - rank[i][j + 1][helper])) { sum += (i) ? PHALANX : -PHALANX; }
							sum += CONNECTED;
						}
					}
					break;
				case 7:
					if (!cfile[WHITE][file - 1]) { sum += ISOLATED * cfile[WHITE][file]; }
					if (!cfile[BLACK][file] && !cfile[BLACK][file - 1]) { sum += PASSED * (WIDTH - rank[WHITE][file][helper]) * (WIDTH - rank[WHITE][file][helper]); }
				default:
					if (!cfile[WHITE][file - 1] && !cfile[WHITE][file + 1]) { sum += ISOLATED * cfile[WHITE][file]; }
					if (!cfile[BLACK][file] && !cfile[BLACK][file - 1] && !cfile[BLACK][file + 1]) { sum += PASSED * (WIDTH - rank[WHITE][file][index]) * (WIDTH - rank[WHITE][file][index]); }
					for (helper = 0; helper < cfile[WHITE][file + 1]; ++helper) {
						if (abs(rank[WHITE][file][index] - rank[WHITE][file + 1][helper]) < 2) {
							//if (!abs(rank[i][j][k] - rank[i][j + 1][helper])) { sum += (i) ? PHALANX : -PHALANX; }
							sum += CONNECTED;
						}
					}
					break;
				}
			}
		}
		for (int file = 0; file < WIDTH; ++file) {
			if (cfile[BLACK][file] > 1) {
				if (cfile[BLACK][file] == 2) { sum -= DOUBLED; }
				else { sum -= TRIPLED; }
			}
			for (int index = 0; index < cfile[BLACK][file]; ++index) {
				switch (file) {
				case 0:
					if (!cfile[BLACK][file + 1]) { sum -= ISOLATED * cfile[BLACK][file]; }
					if (!cfile[WHITE][file] && !cfile[WHITE][file + 1]) { sum -= PASSED * rank[BLACK][file][index] * rank[BLACK][file][index]; }
					for (helper = 0; helper < cfile[BLACK][file + 1]; ++helper) {
						if (abs(rank[BLACK][file][index] - rank[BLACK][file + 1][helper]) < 2) {
							//if (!abs(rank[i][j][k] - rank[i][j + 1][helper])) { sum += (i) ? PHALANX : -PHALANX; }
							sum -= CONNECTED;
						}
					}
					break;
				case 7:
					if (!cfile[BLACK][file - 1]) { sum -= ISOLATED * cfile[BLACK][file]; }
					if (!cfile[WHITE][file] && !cfile[WHITE][file - 1]) { sum -= PASSED * rank[BLACK][file][index] * rank[BLACK][file][index]; }
					break;
				default:
					if (!cfile[BLACK][file - 1] && !cfile[BLACK][file + 1]) { sum -= ISOLATED * cfile[BLACK][file]; }
					if (!cfile[WHITE][file] && !cfile[WHITE][file - 1] && !cfile[WHITE][file + 1]) {sum -= PASSED * rank[BLACK][file][index] * rank[BLACK][file][index];}
					for (helper = 0; helper < cfile[BLACK][file + 1]; ++helper) {
						if (abs(rank[BLACK][file][index] - rank[BLACK][file + 1][helper]) < 2) {
							//if (!abs(rank[i][j][k] - rank[i][j + 1][helper])) { sum += (i) ? PHALANX : -PHALANX; }
							sum -= CONNECTED;
						}
					}
					break;
				}
			}
		}
		return sum;
	}
}