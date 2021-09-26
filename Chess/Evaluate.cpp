#include "Bot.h"
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

	int Bot::negaEval() 
	{//negamax evaluation using material sum of pieces and bonus boards
		if (b->isRepititionDraw() || b->isMaterialDraw()) 
			return CONTEMPT;
		bool endgame = b->isEndgame();
		int sum = 0, helper;
		int cfile[WIDTH * 2];
		for (int i = 0; i < WIDTH * 2; ++i)
			cfile[i] = 0;
		for (int i = 0; i < SPACES; ++i){
			if (b->grid[i] == W_PAWN) 
				++cfile[i % WIDTH + WIDTH];
			else if (b->grid[i] == B_PAWN) 
				++cfile[i % WIDTH];
		}
		for (int i = 0; i < SPACES; ++i) 
		{
			switch (b->grid[i]) 
			{
			case W_PAWN:
				sum += WPAWNBIT[i];
				if (b->grid[i + BOARD_NORTH] == B_PAWN || !cfile[i % WIDTH]) 
				{
					if (i % WIDTH != 7 && (i % WIDTH == 6 || !cfile[i % WIDTH + 2])) 
					{
						if (b->grid[i + BOARD_NORTHEAST] == W_KNIGHT) 
							sum += PAWN_KNIGHT_OUTPOST; 
						else if (b->grid[i + BOARD_NORTHEAST] == W_BISHOP) 
							sum += PAWN_BISHOP_OUTPOST;
					}
					if (i % WIDTH && (i % WIDTH == 1 || !cfile[i % WIDTH - 2])) 
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
				if (b->grid[i + BOARD_SOUTH] == W_PAWN || !cfile[i % WIDTH + WIDTH]) 
				{
					if (i % WIDTH != 7 && (i % WIDTH == 6 || !cfile[i % WIDTH + 10])) 
					{
						if (b->grid[i + BOARD_SOUTHEAST] == B_KNIGHT) 
							sum -= PAWN_KNIGHT_OUTPOST;
						else if (b->grid[i + BOARD_SOUTHEAST] == B_BISHOP) 
							sum -= PAWN_BISHOP_OUTPOST;
					}
					if (i % WIDTH && (i % WIDTH == 1 || !cfile[i % WIDTH + 6])) { 
						if (b->grid[i + BOARD_SOUTHWEST] == B_KNIGHT)
							sum -= PAWN_KNIGHT_OUTPOST;
						else if (b->grid[i + BOARD_SOUTHWEST] == B_BISHOP)
							sum -= PAWN_BISHOP_OUTPOST;
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
				if (!cfile[helper + WIDTH] || !cfile[helper]) 
				{
					if (!cfile[helper + WIDTH] && !cfile[helper]) 
						sum += BONUS_ROOK_OPEN_FILE;
					else 
					{ 
						if (cfile[helper] && (!helper || !cfile[helper + 7]) && (helper == 7 || !cfile[helper + 9])) 
							sum += PAWN_ROOK_ON_PASSED;
						else if (cfile[helper + WIDTH] && (!helper || !cfile[helper - 1]) && (helper == 7 || !cfile[helper + 1])) 
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
				if (!cfile[helper + WIDTH] || !cfile[helper]) 
				{
					if (!cfile[helper + WIDTH] && !cfile[helper]) 
						sum -= BONUS_ROOK_OPEN_FILE;
					else { 
						if (cfile[helper] && (!helper || !cfile[helper + 7]) && (helper == 7 || !cfile[helper + 9])) 
							sum -= PAWN_ROOK_ON_PASSED;
						else if (cfile[helper + WIDTH] && (!helper || !cfile[helper - 1]) && (helper == 7 || !cfile[helper + 1])) 
							sum -= PAWN_ROOK_ON_PASSED;
						sum -= BONUS_ROOK_HALF_OPEN_FILE; 
					}
					if (b->kpos[WHITE] % WIDTH == i % WIDTH) 
						sum -= BONUS_ROOK_ON_KING_FILE;
				}
				break;
			case W_QUEEN:
				sum += WW_QUEENBIT[i] + hypotenuse(b->kpos[BLACK], i) * 4;
				for (helper = 0; helper < b->threatened[SPACES + i]; ++helper) 
				{
					if (b->grid[b->attackers[WIDTH + helper][i]] == W_ROOK || b->grid[b->attackers[WIDTH + helper][i]] == W_BISHOP) 
					{
						sum += BONUS_QUEEN_SUPPORT;
						break;
					}
				}
				break;
			case B_QUEEN:
				sum -= BW_QUEENBIT[i] + hypotenuse(b->kpos[WHITE], i) * 4;
				for (helper = 0; helper < b->threatened[i]; ++helper) 
				{
					if (b->grid[b->attackers[helper][i]] == B_ROOK || b->grid[b->attackers[helper][i]] == B_BISHOP) 
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
		if (b->roles[KINDEX + BINDEX] > 1) 
			sum += BONUS_BISHOP_PAIR;
		if (b->roles[BINDEX] > 1) 
			sum -= BONUS_BISHOP_PAIR;
		return (b->turn) ? b->getCurrV() + sum : -b->getCurrV() - sum;
	}

	int Bot::hypotenuse(int a, int b) 
	{
		int xc = WIDTH - abs(a % WIDTH - b % WIDTH);
		int yc = WIDTH - abs(a / WIDTH - b / WIDTH);
		int c = xc * xc + yc * yc;
		c = (int) std::sqrt(c);
		return c;
	}

	int Bot::pawnEval() 
	{
		int sum = 0;
		int cfile[WIDTH * 2];
		int rank[WIDTH * 2][3];
		int helper;
		for (int i = 0; i < WIDTH * 2; ++i)
		{
			cfile[i] = 0;
			for (int j = 0; j < 3; ++j)
				rank[i][j] = 0;
		}
		for (int i = 0; i < SPACES; ++i) 
		{
			if (b->grid[i] == W_PAWN) 
				rank[i % WIDTH + WIDTH][cfile[i % WIDTH + WIDTH]++] = i / WIDTH;
			else if (b->grid[i] == B_PAWN) 
				rank[i % WIDTH][cfile[i % WIDTH]++] = i / WIDTH;
		}
		for (int file = 0; file < WIDTH; ++file) 
		{
			for (int index = 0; index < cfile[file + WIDTH]; ++index) 
			{
				switch (index) 
				{
				case 2:
					sum += PAWN_DOUBLED;
					break;
				case 3:
					sum += PAWN_TRIPLED;
					break;
				}
				switch (file) {
				case 0:
					if (!cfile[9]) 
						sum += PAWN_ISOLATED * cfile[WIDTH];
					if ((!cfile[0] || rank[0][0] > rank[WIDTH][index]) && (!cfile[1] || rank[1][0] > rank[WIDTH][index])) 
						sum += PAWN_PASSED * (WIDTH - rank[WIDTH][index]) * (WIDTH - rank[WIDTH][index]);
					for (helper = 0; helper < cfile[WIDTH]; ++helper) 
					{
						if (abs(rank[WIDTH][index] - rank[WIDTH][helper]) < 2) 
						{
							if (!abs(rank[WIDTH][index] - rank[9][helper])) 
								sum += PAWN_PHALANX;
							if (rank[WIDTH][index] - rank[9][helper] == 1 && b->grid[rank[9][helper] * WIDTH + BOARD_NORTH] == B_PAWN) 
								sum += PAWN_BACKWARD;
							sum += PAWN_CONNECTED;
							break;
						}
					}
					break;
				case 7:
					if (!cfile[14]) 
						sum += PAWN_ISOLATED * cfile[15];
					if ((!cfile[7] || rank[7][0] > rank[15][index]) && (!cfile[6] || rank[6][0] > rank[15][index])) 
						sum += PAWN_PASSED * (WIDTH - rank[15][index]) * (WIDTH - rank[15][index]);
					for (helper = 0; helper < cfile[14]; ++helper) 
					{
						if (abs(rank[15][index] - rank[14][helper]) < 2) 
						{
							if (!abs(rank[15][index] - rank[14][helper])) 
								sum += PAWN_PHALANX;
							if (rank[15][index] - rank[14][helper] == 1 && b->grid[7 + rank[14][helper] * WIDTH + BOARD_NORTH] == B_PAWN) 
								sum += PAWN_BACKWARD;
							sum += PAWN_CONNECTED;
							break;
						}
					}
					break;
				default:
					if (!cfile[file + 7] && !cfile[file + 9]) 
						sum += PAWN_ISOLATED * cfile[file + WIDTH];
					if ((!cfile[file] || rank[file][0] > rank[file + WIDTH][index]) && (!cfile[file + 1] || rank[file + 1][0] > rank[WIDTH][index]) && (!cfile[file - 1] || rank[file - 1][0] > rank[WIDTH][index])) 
						sum += PAWN_PASSED * (WIDTH - rank[file + WIDTH][index]) * (WIDTH - rank[file + WIDTH][index]);
					for (helper = 0; helper < cfile[file + 9]; ++helper) 
					{
						if (abs(rank[file + WIDTH][index] - rank[file + 9][helper]) < 2) 
						{
							if (!abs(rank[file + WIDTH][index] - rank[file + 9][helper])) 
								sum += PAWN_PHALANX;
							if (rank[file + WIDTH][index] - rank[file + 9][helper] == 1 && b->grid[file + rank[file + 9][helper] * WIDTH + BOARD_NORTH] == B_PAWN) 
								sum += PAWN_BACKWARD;
							sum += PAWN_CONNECTED;
							break;
						}
					}
					for (helper = 0; helper < cfile[file + 7]; ++helper) 
					{
						if (abs(rank[file + WIDTH][index] - rank[file + 7][helper]) < 2) 
						{
							if (!abs(rank[file + WIDTH][index] - rank[file + 7][helper])) 
								sum += PAWN_PHALANX;
							if (rank[file + WIDTH][index] - rank[file + 7][helper] == 1 && b->grid[file + rank[file + 7][helper] * WIDTH + BOARD_NORTH] == B_PAWN) 
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
			for (int index = 0; index < cfile[file]; ++index) 
			{
				switch (index) 
				{
				case 2:
					sum -= PAWN_DOUBLED;
					break;
				case 3:
					sum -= PAWN_TRIPLED;
					break;
				}
				switch (file) 
				{
				case 0:
					if (!cfile[1]) 
						sum -= PAWN_ISOLATED * cfile[0];
					if ((!cfile[WIDTH] || rank[WIDTH][0] > rank[0][index]) && (!cfile[9] || rank[9][0] > rank[0][index])) 
						sum -= PAWN_PASSED * (WIDTH - rank[0][index]) * (WIDTH - rank[0][index]);
					for (helper = 0; helper < cfile[1]; ++helper) 
					{
						if (abs(rank[0][index] - rank[1][helper]) < 2) 
						{
							if (!abs(rank[0][index] - rank[1][helper])) 
								sum -= PAWN_PHALANX;
							if (rank[0][index] - rank[1][helper] == 1 && b->grid[rank[1][helper] * WIDTH + BOARD_SOUTH] == W_PAWN) 
								sum -= PAWN_BACKWARD;
							sum -= PAWN_CONNECTED;
							break;
						}
					}
					break;
				case 7:
					if (!cfile[6]) 
						sum -= PAWN_ISOLATED * cfile[7];
					if ((!cfile[15] || rank[15][0] > rank[7][index]) && (!cfile[14] || rank[14][0] > rank[7][index])) 
						sum -= PAWN_PASSED * (WIDTH - rank[7][index]) * (WIDTH - rank[7][index]);
					for (helper = 0; helper < cfile[6]; ++helper) 
					{
						if (abs(rank[7][index] - rank[6][helper]) < 2) 
						{
							if (!abs(rank[7][index] - rank[6][helper])) 
								sum -= PAWN_PHALANX;
							if (rank[7][index] - rank[6][helper] == 1 && b->grid[7 + rank[6][helper] * WIDTH + BOARD_SOUTH] == W_PAWN) 
								sum -= PAWN_BACKWARD;
							sum -= PAWN_CONNECTED;
							break;
						}
					}
					break;
				default:
					if (!cfile[file - 1] && !cfile[file + 1]) 
						sum -= PAWN_ISOLATED * cfile[file];
					if ((!cfile[file + WIDTH] || rank[file + WIDTH][0] > rank[file][index]) && (!cfile[file + 9] || rank[file + 9][0] > rank[0][index]) && (!cfile[file + 7] || rank[file + 7][0] > rank[0][index])) 
						sum -= PAWN_PASSED * (WIDTH - rank[file][index]) * (WIDTH - rank[file][index]);
					for (helper = 0; helper < cfile[file + 1]; ++helper) 
					{
						if (abs(rank[file][index] - rank[file + 1][helper]) < 2) 
						{
							if (!abs(rank[file][index] - rank[file + 1][helper])) 
								sum -= PAWN_PHALANX;
							if (rank[file][index] - rank[file + 1][helper] == 1 && b->grid[file + rank[file + 1][helper] * WIDTH + BOARD_SOUTH] == W_PAWN) 
								sum -= PAWN_BACKWARD;
							sum -= PAWN_CONNECTED;
							break;
						}
					}
					for (helper = 0; helper < cfile[file - 1]; ++helper) 
					{
						if (abs(rank[file][index] - rank[file - 1][helper]) < 2) 
						{
							if (!abs(rank[file][index] - rank[file - 1][helper])) 
								sum -= PAWN_PHALANX;
							if (rank[file][index] - rank[file - 1][helper] == 1 && b->grid[file + rank[file - 1][helper] * WIDTH + BOARD_SOUTH] == W_PAWN) 
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