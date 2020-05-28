#include "Evaluate.h"
#include "Board.h"
#include <cmath>

namespace Hopper 
{
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

	static int WW_QUEENBIT[SPACES] = {
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};

	static int BW_QUEENBIT[SPACES] = {
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

	int Evaluate::negaEval() 
	{//negamax evaluation using material sum of pieces and bonus boards
		if (b->insufficientMaterial()) 
			return CONTEMPT;
		int sum = 0, helper;
		int cfile[2][WIDTH];
		for (int i = 0; i < 2; ++i)
			for (int j = 0; j < WIDTH; ++j) 
				cfile[i][j] = 0;
		for (int i = 0; i < SPACES; ++i){
			if (b->grid[i] == W_PAWN) 
				++cfile[WHITE][i % WIDTH];
			else if (b->grid[i] == B_PAWN) 
				++cfile[BLACK][i % WIDTH];
		}
		bool endgame = b->isEndgame();
		for (int i = 0; i < SPACES; ++i) 
		{
			switch (b->grid[i]) 
			{
			case W_PAWN:
				sum += WPAWNBIT[i];
				if (b->grid[i + BOARD_NORTH] == B_PAWN || !cfile[BLACK][i % WIDTH]) 
				{
					if (i % WIDTH != 7 && (i % WIDTH == 6 || !cfile[BLACK][i % WIDTH + 2])) 
					{
						if (b->grid[i + BOARD_NORTHEAST] == W_KNIGHT) 
							sum += PAWN_KNIGHT_OUTPOST; 
						else if (b->grid[i + BOARD_NORTHEAST] == W_BISHOP) 
							sum += PAWN_BISHOP_OUTPOST;
					}
					if (i % WIDTH && (i % WIDTH == 1 || !cfile[BLACK][i % WIDTH - 2])) 
					{
						if (b->grid[i + BOARD_NORTHWEST] == W_KNIGHT) 
							sum += PAWN_KNIGHT_OUTPOST;
						else if (b->grid[i + BOARD_NORTHWEST] == W_BISHOP) 
							sum += PAWN_BISHOP_OUTPOST;
					}
				}
				break;
			case B_PAWN:
				sum -= BPAWNBIT[i];
				if (b->grid[i + BOARD_SOUTH] == W_PAWN || !cfile[WHITE][i % WIDTH]) 
				{
					if (i % WIDTH != 7 && (i % WIDTH == 6 || !cfile[WHITE][i % WIDTH + 2])) 
					{
						if (b->grid[i + BOARD_SOUTHEAST] == B_KNIGHT) 
							sum -= PAWN_KNIGHT_OUTPOST;
						else if (b->grid[i + BOARD_SOUTHEAST] == B_BISHOP) 
							sum -= PAWN_BISHOP_OUTPOST;
					}
					if (i % WIDTH && (i % WIDTH == 1 || !cfile[WHITE][i % WIDTH - 2])) { 
						if (b->grid[i + BOARD_SOUTHWEST] == B_KNIGHT) { sum -= PAWN_KNIGHT_OUTPOST; }
						else if (b->grid[i + BOARD_SOUTHWEST] == B_BISHOP) { sum -= PAWN_BISHOP_OUTPOST; }
					}
				}
				break;
			case W_KNIGHT:
				sum += WKNIGHTBIT[i] + hypotenuse(b->kpos[BLACK], i);
				break;
			case B_KNIGHT:
				sum -= BKNIGHTBIT[i] + hypotenuse(b->kpos[WHITE], i);
				break;
			case W_BISHOP:
				sum += WBISHOPBIT[i] + hypotenuse(b->kpos[BLACK], i) * 2;
				break;
			case B_BISHOP:
				sum -= BBISHOPBIT[i] + hypotenuse(b->kpos[WHITE], i) * 2;
				break;
			case W_ROOK:
				sum += WROOKBIT[i] + hypotenuse(b->kpos[BLACK], i) * 3;
				helper = i % WIDTH;
				if (!cfile[WHITE][helper] || !cfile[BLACK][helper]) 
				{
					if (!cfile[WHITE][helper] && !cfile[BLACK][helper]) 
						sum += BONUS_ROOK_OPEN_FILE;
					else 
					{ 
						if (cfile[BLACK][helper] && (!helper || !cfile[WHITE][helper - 1]) && (helper == 7 || !cfile[WHITE][helper + 1])) 
							sum += PAWN_ROOK_ON_PASSED;
						else if (cfile[WHITE][helper] && (!helper || !cfile[BLACK][helper - 1]) && (helper == 7 || !cfile[BLACK][helper + 1])) 
							sum += PAWN_ROOK_ON_PASSED;
						sum += BONUS_ROOK_HALF_OPEN_FILE; 
					}
					if (b->kpos[BLACK] % WIDTH == i % WIDTH) 
						sum += BONUS_ROOK_ON_KING_FILE;
				}
				break;
			case B_ROOK:
				sum -= BROOKBIT[i] + hypotenuse(b->kpos[WHITE], i) * 3;
				helper = i % WIDTH;
				if (!cfile[WHITE][helper] || !cfile[BLACK][helper]) 
				{
					if (!cfile[WHITE][helper] && !cfile[BLACK][helper]) 
						sum -= BONUS_ROOK_OPEN_FILE;
					else { 
						if (cfile[BLACK][helper] && (!helper || !cfile[WHITE][helper - 1]) && (helper == 7 || !cfile[WHITE][helper + 1])) 
							sum -= PAWN_ROOK_ON_PASSED;
						else if (cfile[WHITE][helper] && (!helper || !cfile[BLACK][helper - 1]) && (helper == 7 || !cfile[BLACK][helper + 1])) 
							sum -= PAWN_ROOK_ON_PASSED;
						sum -= BONUS_ROOK_HALF_OPEN_FILE; 
					}
					if (b->kpos[WHITE] % WIDTH == i % WIDTH) 
						sum -= BONUS_ROOK_ON_KING_FILE;
				}
				break;
			case W_QUEEN:
				sum += WW_QUEENBIT[i] + hypotenuse(b->kpos[BLACK], i) * 4;
				for (helper = 0; helper < b->threatened[WHITE][i]; ++helper) 
				{
					if (b->grid[b->attackers[WHITE][helper][i]] == W_ROOK || b->grid[b->attackers[WHITE][helper][i]] == W_BISHOP) 
					{
						sum += BONUS_QUEEN_SUPPORT;
						break;
					}
				}
				break;
			case B_QUEEN:
				sum -= BW_QUEENBIT[i] + hypotenuse(b->kpos[WHITE], i) * 4;
				for (helper = 0; helper < b->threatened[BLACK][i]; ++helper) 
				{
					if (b->grid[b->attackers[BLACK][helper][i]] == B_ROOK || b->grid[b->attackers[BLACK][helper][i]] == B_BISHOP) 
					{
						sum -= BONUS_QUEEN_SUPPORT;
						break;
					}
				}
				break;
			case W_KING:
				sum += (endgame) ? WENDKINGBIT[i] : WKINGBIT[i];
				break;
			case B_KING:
				sum -= (endgame) ? BENDKINGBIT[i] : BKINGBIT[i];
				break;
			}
		}
		if (b->roles[WHITE][BINDEX] > 1) 
			sum += BONUS_BISHOP_PAIR;
		if (b->roles[BLACK][BINDEX] > 1) 
			sum -= BONUS_BISHOP_PAIR;
		return (b->turn) ? b->getCurrV() + sum : -b->getCurrV() - sum;
	}

	int Evaluate::hypotenuse(int a, int b) 
	{
		int xc = WIDTH - abs(a % WIDTH - b % WIDTH);
		int yc = WIDTH - abs(a / WIDTH - b / WIDTH);
		int c = xc * xc + yc * yc;
		c = (int) std::sqrt(c);
		return c;
	}

	int Evaluate::pawnEval() 
	{
		int sum = 0;
		int cfile[2][WIDTH];
		int rank[2][WIDTH][3];
		int helper;
		for (int i = 0; i < 2; ++i) 
		{
			for (int j = 0; j < WIDTH; ++j) 
			{
				cfile[i][j] = 0;
				for (int k = 0; k < 3; ++k) 
					rank[i][j][k] = 0;
			}
		}
		for (int i = 0; i < SPACES; ++i) 
		{
			if (b->grid[i] == W_PAWN) 
				rank[WHITE][i % WIDTH][cfile[WHITE][i % WIDTH]++] = i / WIDTH;
			else if (b->grid[i] == B_PAWN) 
				rank[BLACK][i % WIDTH][cfile[BLACK][i % WIDTH]++] = i / WIDTH;
		}
		for (int file = 0; file < WIDTH; ++file) 
		{
			for (int index = 0; index < cfile[WHITE][file]; ++index) 
			{
				switch (index) 
				{
				case 1:
					sum += PAWN_DOUBLED;
					break;
				case 2:
					sum += PAWN_TRIPLED;
					break;
				}
				switch (file) {
				case 0:
					if (!cfile[WHITE][1]) 
						sum += PAWN_ISOLATED * cfile[WHITE][0];
					if ((!cfile[BLACK][0] || rank[BLACK][0][0] > rank[WHITE][0][index]) && (!cfile[BLACK][1] || rank[BLACK][1][0] > rank[WHITE][0][index])) 
						sum += PAWN_PASSED * (WIDTH - rank[WHITE][0][index]) * (WIDTH - rank[WHITE][0][index]);
					for (helper = 0; helper < cfile[WHITE][1]; ++helper) 
					{
						if (abs(rank[WHITE][0][index] - rank[WHITE][1][helper]) < 2) 
						{
							if (!abs(rank[WHITE][0][index] - rank[WHITE][1][helper])) 
								sum += PAWN_PHALANX;
							if (rank[WHITE][0][index] - rank[WHITE][1][helper] == 1 && b->grid[rank[WHITE][1][helper] * WIDTH + BOARD_NORTH] == B_PAWN) 
								sum += PAWN_BACKWARD;
							sum += PAWN_CONNECTED;
							break;
						}
					}
					break;
				case 7:
					if (!cfile[WHITE][6]) 
						sum += PAWN_ISOLATED * cfile[WHITE][7];
					if ((!cfile[BLACK][7] || rank[BLACK][7][0] > rank[WHITE][7][index]) && (!cfile[BLACK][6] || rank[BLACK][6][0] > rank[WHITE][7][index])) 
						sum += PAWN_PASSED * (WIDTH - rank[WHITE][7][index]) * (WIDTH - rank[WHITE][7][index]);
					for (helper = 0; helper < cfile[WHITE][6]; ++helper) 
					{
						if (abs(rank[WHITE][7][index] - rank[WHITE][6][helper]) < 2) 
						{
							if (!abs(rank[WHITE][7][index] - rank[WHITE][6][helper])) 
								sum += PAWN_PHALANX;
							if (rank[WHITE][7][index] - rank[WHITE][6][helper] == 1 && b->grid[7 + rank[WHITE][6][helper] * WIDTH + BOARD_NORTH] == B_PAWN) 
								sum += PAWN_BACKWARD;
							sum += PAWN_CONNECTED;
							break;
						}
					}
					break;
				default:
					if (!cfile[WHITE][file - 1] && !cfile[WHITE][file + 1]) 
						sum += PAWN_ISOLATED * cfile[WHITE][file];
					if ((!cfile[BLACK][file] || rank[BLACK][file][0] > rank[WHITE][file][index]) && (!cfile[BLACK][file + 1] || rank[BLACK][file + 1][0] > rank[WHITE][0][index]) && (!cfile[BLACK][file - 1] || rank[BLACK][file - 1][0] > rank[WHITE][0][index])) 
						sum += PAWN_PASSED * (WIDTH - rank[WHITE][file][index]) * (WIDTH - rank[WHITE][file][index]);
					for (helper = 0; helper < cfile[WHITE][file + 1]; ++helper) 
					{
						if (abs(rank[WHITE][file][index] - rank[WHITE][file + 1][helper]) < 2) 
						{
							if (!abs(rank[WHITE][file][index] - rank[WHITE][file + 1][helper])) 
								sum += PAWN_PHALANX;
							if (rank[WHITE][file][index] - rank[WHITE][file + 1][helper] == 1 && b->grid[file + rank[WHITE][file + 1][helper] * WIDTH + BOARD_NORTH] == B_PAWN) 
								sum += PAWN_BACKWARD;
							sum += PAWN_CONNECTED;
							break;
						}
					}
					for (helper = 0; helper < cfile[WHITE][file - 1]; ++helper) 
					{
						if (abs(rank[WHITE][file][index] - rank[WHITE][file - 1][helper]) < 2) 
						{
							if (!abs(rank[WHITE][file][index] - rank[WHITE][file - 1][helper])) 
								sum += PAWN_PHALANX;
							if (rank[WHITE][file][index] - rank[WHITE][file - 1][helper] == 1 && b->grid[file + rank[WHITE][file - 1][helper] * WIDTH + BOARD_NORTH] == B_PAWN) 
								sum += PAWN_BACKWARD;
							sum += PAWN_CONNECTED;
							break;
						}
					}
				}
			}
		}
		for (int file = 0; file < WIDTH; ++file) 
		{
			for (int index = 0; index < cfile[BLACK][file]; ++index) 
			{
				switch (index) 
				{
				case 1:
					sum -= PAWN_DOUBLED;
					break;
				case 2:
					sum -= PAWN_TRIPLED;
					break;
				}
				switch (file) 
				{
				case 0:
					if (!cfile[BLACK][1]) 
						sum -= PAWN_ISOLATED * cfile[BLACK][0];
					if ((!cfile[WHITE][0] || rank[WHITE][0][0] > rank[BLACK][0][index]) && (!cfile[WHITE][1] || rank[WHITE][1][0] > rank[BLACK][0][index])) 
						sum -= PAWN_PASSED * (WIDTH - rank[BLACK][0][index]) * (WIDTH - rank[BLACK][0][index]);
					for (helper = 0; helper < cfile[BLACK][1]; ++helper) 
					{
						if (abs(rank[BLACK][0][index] - rank[BLACK][1][helper]) < 2) 
						{
							if (!abs(rank[BLACK][0][index] - rank[BLACK][1][helper])) 
								sum -= PAWN_PHALANX;
							if (rank[BLACK][0][index] - rank[BLACK][1][helper] == 1 && b->grid[rank[BLACK][1][helper] * WIDTH + BOARD_SOUTH] == W_PAWN) 
								sum -= PAWN_BACKWARD;
							sum -= PAWN_CONNECTED;
							break;
						}
					}
					break;
				case 7:
					if (!cfile[BLACK][6]) 
						sum -= PAWN_ISOLATED * cfile[BLACK][7];
					if ((!cfile[WHITE][7] || rank[WHITE][7][0] > rank[BLACK][7][index]) && (!cfile[WHITE][6] || rank[WHITE][6][0] > rank[BLACK][7][index])) 
						sum -= PAWN_PASSED * (WIDTH - rank[BLACK][7][index]) * (WIDTH - rank[BLACK][7][index]);
					for (helper = 0; helper < cfile[BLACK][6]; ++helper) 
					{
						if (abs(rank[BLACK][7][index] - rank[BLACK][6][helper]) < 2) 
						{
							if (!abs(rank[BLACK][7][index] - rank[BLACK][6][helper])) 
								sum -= PAWN_PHALANX;
							if (rank[BLACK][7][index] - rank[BLACK][6][helper] == 1 && b->grid[7 + rank[BLACK][6][helper] * WIDTH + BOARD_SOUTH] == W_PAWN) 
								sum -= PAWN_BACKWARD;
							sum -= PAWN_CONNECTED;
							break;
						}
					}
					break;
				default:
					if (!cfile[BLACK][file - 1] && !cfile[BLACK][file + 1]) 
						sum -= PAWN_ISOLATED * cfile[BLACK][file];
					if ((!cfile[WHITE][file] || rank[WHITE][file][0] > rank[BLACK][file][index]) && (!cfile[WHITE][file + 1] || rank[WHITE][file + 1][0] > rank[BLACK][0][index]) && (!cfile[WHITE][file - 1] || rank[WHITE][file - 1][0] > rank[BLACK][0][index])) 
						sum -= PAWN_PASSED * (WIDTH - rank[BLACK][file][index]) * (WIDTH - rank[BLACK][file][index]);
					for (helper = 0; helper < cfile[BLACK][file + 1]; ++helper) 
					{
						if (abs(rank[BLACK][file][index] - rank[BLACK][file + 1][helper]) < 2) 
						{
							if (!abs(rank[BLACK][file][index] - rank[BLACK][file + 1][helper])) 
								sum -= PAWN_PHALANX;
							if (rank[BLACK][file][index] - rank[BLACK][file + 1][helper] == 1 && b->grid[file + rank[BLACK][file + 1][helper] * WIDTH + BOARD_SOUTH] == W_PAWN) 
								sum -= PAWN_BACKWARD;
							sum -= PAWN_CONNECTED;
							break;
						}
					}
					for (helper = 0; helper < cfile[BLACK][file - 1]; ++helper) 
					{
						if (abs(rank[BLACK][file][index] - rank[BLACK][file - 1][helper]) < 2) 
						{
							if (!abs(rank[BLACK][file][index] - rank[BLACK][file - 1][helper])) 
								sum -= PAWN_PHALANX;
							if (rank[BLACK][file][index] - rank[BLACK][file - 1][helper] == 1 && b->grid[file + rank[BLACK][file - 1][helper] * WIDTH + BOARD_SOUTH] == W_PAWN) 
								sum -= PAWN_BACKWARD;
							sum -= PAWN_CONNECTED;
							break;
						}
					}
				}
			}
		}
		return sum;
	}
}