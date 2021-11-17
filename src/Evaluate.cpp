#include <cmath>
#include <cstring>
#include <cassert>
#include "Engine.h"
#include "Evaluate.h"

namespace Hopper
{
	int mg_pawn_table[64] = {
		 0,   0,   0,   0,   0,   0,  0,   0,
		 98, 134,  61,  95,  68, 126, 34, -11,
		 -6,   7,  26,  31,  65,  56, 25, -20,
		-14,  13,   6,  21,  23,  12, 17, -23,
		-27,  -2,  -5,  12,  17,   6, 10, -25,
		-26,  -4,  -4, -10,   3,   3, 33, -12,
		-35,  -1, -20, -23, -15,  24, 38, -22,
		  0,   0,   0,   0,   0,   0,  0,   0
	};

	int eg_pawn_table[64] = {
		  0,   0,   0,   0,   0,   0,   0,   0,
		178, 173, 158, 134, 147, 132, 165, 187,
		 94, 100,  85,  67,  56,  53,  82,  84,
		 32,  24,  13,   5,  -2,   4,  17,  17,
		 13,   9,  -3,  -7,  -7,  -8,   3,  -1,
		  4,   7,  -6,   1,   0,  -5,  -1,  -8,
		 13,   8,   8,  10,  13,   0,   2,  -7,
		  0,   0,   0,   0,   0,   0,   0,   0,
	};

	int mg_knight_table[64] = {
		-205, -109, -103, 27, 87, -243, 73, -151,
		3, -12, 39, 57, 68, 100, -19, 30,
		-26, 26, 54, 65, 110, 112, 79, 64,
		10, 24, 40, 46, 38, 61, 39, 43,
		-2, 5, 25, 21, 29, 24, 41, 14,
		-13, 6, 12, 19, 24, 17, 20, -1,
		-37, -22, -6, 6, 2, 2, -17, -3,
		-120, -16, -48, -26, -13, -5, -11, -109,
	};

	int eg_knight_table[64] = {
		-21, 38, 66, 31, 21, 111, 1, -39,
		-8, 29, 38, 67, 57, 16, 31, -5,
		22, 49, 68, 68, 52, 64, 35, 7,
		21, 56, 73, 86, 90, 80, 68, 33,
		18, 57, 73, 80, 78, 77, 56, 26,
		-16, 30, 40, 63, 59, 37, 26, -15,
		0, 21, 17, 28, 35, 15, 8, -4,
		-35, -48, 7, 15, 8, -4, -35, 22,
	};

	int mg_bishop_table[64] = {
		-55, -43, -56, -54, -69, -84, 68, -21,
		-30, 0, 2, 3, 14, 38, 8, 19,
		-1, 11, 49, 39, 60, 73, 64, 38,
		-11, 23, 22, 55, 34, 46, 19, 14,
		-2, 10, 17, 20, 35, 5, 13, 12,
		0, 11, 6, 17, 5, 11, 11, 9,
		7, 9, 18, -6, 3, -1, 15, 10,
		5, 9, -20, -23, -31, -11, -17, -6,
	};

	int eg_bishop_table[64] = {
		50, 56, 48, 60, 55, 55, 26, 47,
		27, 50, 49, 50, 52, 42, 46, 17,
		33, 52, 46, 51, 47, 58, 47, 36,
		37, 51, 52, 57, 67, 49, 62, 37,
		22, 45, 55, 61, 51, 57, 41, 15,
		18, 35, 48, 45, 49, 37, 25, 13,
		6, 13, 18, 31, 25, 15, 17, -23,
		-7, 11, -8, 12, 21, 3, 21, 7,
	};

	int mg_rook_table[64] = {
		93, 98, 96, 75, 120, 125, 123, 170,
		45, 37, 64, 92, 80, 134, 82, 111,
		14, 52, 52, 76, 99, 124, 145, 76,
		5, 22, 33, 50, 37, 45, 46, 39,
		-13, -14, -6, 1, -2, -2, 19, 12,
		-23, -8, -15, -12, -13, -14, 28, 3,
		-43, -8, -7, -11, -12, 1, 11, -57,
		-7, 0, 4, 5, 3, 0, 24, -5,
	};

	int eg_rook_table[64] = {
		47, 51, 52, 61, 46, 50, 48, 33,
		56, 64, 63, 60, 64, 26, 39, 28,
		56, 46, 55, 43, 34, 36, 13, 35,
		46, 49, 52, 48, 48, 44, 38, 37,
		31, 47, 48, 45, 40, 42, 36, 23,
		10, 22, 26, 24, 25, 22, 10, 0,
		10, 4, 14, 12, 12, 7, 1, 28,
		5, 15, 23, 22, 19, 18, 10, -6,
	};

	int mg_queen_table[64] = {
		-24, 9, 47, 62, 53, 93, 104, 109,
		-18, -51, -13, -25, -4, 66, 7, 39,
		-20, -12, -15, 4, 13, 92, 96, 49,
		-1, -7, -11, -8, -7, 16, 29, 31,
		-4, -7, -2, -9, -8, 6, 16, 19,
		-10, 3, 0, -2, -4, -1, 18, 10,
		-15, 2, 8, -2, 1, 4, 9, -5,
		-1, -15, -8, 1, -7, -31, 35, -24,
	};

	int eg_queen_table[64] = {
		101, 92, 91, 89, 114, 113, 105, 64,
		54, 104, 107, 135, 169, 138, 153, 129,
		42, 62, 90, 108, 155, 126, 112, 152,
		15, 63, 78, 109, 146, 149, 129, 107,
		18, 61, 55, 99, 97, 92, 63, 81,
		6, 16, 49, 33, 35, 51, 15, 23,
		6, -4, -26, 10, 1, -27, -39, -10,
		-14, -16, -28, -29, -27, -24, -129, -10,
	};

	int mg_king_table[64] = {
		3, 103, 171, 104, -52, -73, 131, -159,
		42, 92, 133, -33, 102, 91, 35, -36,
		67, 102, 80, -3, 44, 67, 84, -50,
		3, 38, 36, 54, -17, 67, 22, -54,
		33, 75, 52, -12, 24, -2, 36, -24,
		-17, -15, -28, -21, -8, -18, 7, -17,
		25, 8, -15, -65, -36, -48, 10, 35,
		1, 41, 5, -93, -13, -79, 32, 40,
	};

	int eg_king_table[64] = {
		-78, -29, -42, -17, -17, 21, -20, -81,
		-26, 30, 15, 33, 14, 24, 50, -16,
		16, 43, 42, 46, 35, 52, 51, 21,
		19, 39, 50, 46, 58, 46, 45, 20,
		-17, 14, 35, 54, 45, 42, 20, -3,
		-8, 16, 29, 38, 34, 26, 6, -6,
		-5, 0, 13, 24, 17, 22, -1, -27,
		-49, -25, -11, -12, -57, 0, -29, -80,
	};

	int* mg_pesto_table[6] =
	{
		mg_pawn_table,
		mg_knight_table,
		mg_bishop_table,
		mg_rook_table,
		mg_queen_table,
		mg_king_table
	};

	int* eg_pesto_table[6] =
	{
		eg_pawn_table,
		eg_knight_table,
		eg_bishop_table,
		eg_rook_table,
		eg_queen_table,
		eg_king_table
	};

	int mg_table[12][64];
	int eg_table[12][64];

	int mg_value[6] = { 82, 343, 351, 480, 1139, 0, };
	int eg_value[6] = { 94, 324, 361, 645, 1110, 0, };

	int passed_rank_bonus[8] = { 0, 146, 82, 40, 20, 5, 4, 0, };

	int pawn_file_population[5] = { 0, 0, 10, 40, 50,};

	int game_phase_inc[13] = { 0,0,1,1,1,1,2,2,4,4,0,0,0 };

	int pawn_connected = 9;
	int pawn_backward = 4;
	int pawn_isolated = 11;

	int knight_outpost = 20;
	int bishop_outpost = 9;
	int rook_open_file = 10;
	int bishop_pair = 36;

	Evaluate::Evaluate() 
	{
		initEvalTables();
	}

	void Evaluate::initEvalTables()
	{
		for (int p = PAWN, pc = WHITE_PAWN; p <= KING; pc += 2, ++p) {
			for (int sq = 0; sq < SPACES; ++sq) {
				mg_table[pc][sq] = mg_value[p] + mg_pesto_table[p][sq];
				eg_table[pc][sq] = eg_value[p] + eg_pesto_table[p][sq];
				mg_table[pc + 1][sq] = mg_value[p] + mg_pesto_table[p][sq ^ 56];
				eg_table[pc + 1][sq] = eg_value[p] + eg_pesto_table[p][sq ^ 56];
			}
		}
	}

	int Evaluate::eval(position* myPosition)
	{
		int mg[2];
		int eg[2];

		mg[WHITE] = 0;
		mg[BLACK] = 0;
		eg[WHITE] = 0;
		eg[BLACK] = 0;

		int sum = 0;
		int helper;
		int pc;
		int mgPhase  = 0;
		int bCount[2] = { 0, 0 };

		for (int sq = 0; sq < SPACES; ++sq) {
			pc = myPosition->grid[sq];
			switch (pc) {
			case WHITE_PAWN:
				if (myPosition->grid[sq + BOARD_NORTH] == BLACK_PAWN) {
					switch (sq % WIDTH) {
					case 7:
					case 0:
						break;
					default:
						if (myPosition->grid[sq + BOARD_NORTHEAST] == WHITE_KNIGHT) {
							if (sq % WIDTH == 6)
								sum += knight_outpost;
							else {
								for (helper = sq + BOARD_NORTHEAST + BOARD_NORTHEAST; helper >= WIDTH; helper += BOARD_NORTH) {
									if (myPosition->grid[helper] == BLACK_PAWN)
										break;
								}
								if (helper < WIDTH)
									sum += knight_outpost;
							}
						}
						else if (myPosition->grid[sq + BOARD_NORTHEAST] == WHITE_BISHOP) {
							if (sq % WIDTH == 6)
								sum += bishop_outpost;
							else {
								for (helper = sq + BOARD_NORTHEAST + BOARD_NORTHEAST; helper >= WIDTH; helper += BOARD_NORTH) {
									if (myPosition->grid[helper] == BLACK_PAWN)
										break;
								}
								if (helper < WIDTH)
									sum += bishop_outpost;
							}
						}
						if (myPosition->grid[sq + BOARD_NORTHWEST] == WHITE_KNIGHT) {
							if (sq % WIDTH == 1)
								sum += knight_outpost;
							else {
								for (helper = sq + BOARD_NORTHWEST + BOARD_NORTHWEST; helper >= WIDTH; helper += BOARD_NORTH) {
									if (myPosition->grid[helper] == BLACK_PAWN)
										break;
								}
								if (helper < WIDTH)
									sum += knight_outpost;
							}
						}
						else if (myPosition->grid[sq + BOARD_NORTHWEST] == WHITE_BISHOP) {
							if (sq % WIDTH == 1)
								sum += bishop_outpost;
							else {
								for (helper = sq + BOARD_NORTHWEST + BOARD_NORTHWEST; helper >= WIDTH; helper += BOARD_NORTH) {
									if (myPosition->grid[helper] == BLACK_PAWN)
										break;
								}
								if (helper < WIDTH)
									sum += bishop_outpost;
							}
						}
					}
				}
				break;
			case BLACK_PAWN:
				if (myPosition->grid[sq + BOARD_SOUTH] == WHITE_PAWN) {
					switch (sq % WIDTH) {
					case 7:
					case 0:
						break;
					default:
						if (myPosition->grid[sq + BOARD_SOUTHEAST] == BLACK_KNIGHT) {
							if (sq % WIDTH == 6)
								sum -= knight_outpost;
							else {
								if (myPosition->grid[sq + BOARD_SOUTHEAST] == BLACK_KNIGHT) {
									for (helper = sq + BOARD_SOUTHEAST + BOARD_SOUTHEAST; helper < 56; helper += BOARD_SOUTH) {
										if (myPosition->grid[helper] == WHITE_PAWN)
											break;
									}
									if (helper >= 56)
										sum -= knight_outpost;
								}
							}
						}
						else if (myPosition->grid[sq + BOARD_SOUTHEAST] == BLACK_BISHOP) {
							if (sq % WIDTH == 6)
								sum -= bishop_outpost;
							else {
								if (myPosition->grid[sq + BOARD_SOUTHEAST] == BLACK_KNIGHT) {
									for (helper = sq + BOARD_SOUTHEAST + BOARD_SOUTHEAST; helper < 56; helper += BOARD_SOUTH) {
										if (myPosition->grid[helper] == WHITE_PAWN)
											break;
									}
									if (helper >= 56)
										sum -= bishop_outpost;
								}
							}
						}
						if (myPosition->grid[sq + BOARD_SOUTHWEST] == BLACK_KNIGHT) {
							if (sq % WIDTH == 1)
								sum -= knight_outpost;
							else {
								for (helper = sq + BOARD_SOUTHWEST + BOARD_SOUTHWEST; helper < 56; helper += BOARD_SOUTH) {
									if (myPosition->grid[helper] == WHITE_PAWN)
										break;
								}
								if (helper >= 56)
									sum -= knight_outpost;
							}
						}
						else if (myPosition->grid[sq + BOARD_SOUTHWEST] == BLACK_BISHOP) {
							if (sq % WIDTH == 1)
								sum -= bishop_outpost;
							else {
								for (helper = sq + BOARD_SOUTHWEST + BOARD_SOUTHWEST; helper < 56; helper += BOARD_SOUTH) {
									if (myPosition->grid[helper] == WHITE_PAWN)
										break;
								}
								if (helper >= 56)
									sum -= bishop_outpost;
							}
						}
					}
				}
				break;
			case WHITE_ROOK:
				for (helper = sq % WIDTH; helper < 56; helper += BOARD_SOUTH) {
					if (myPosition->grid[helper] == BLACK_PAWN || myPosition->grid[helper] == WHITE_PAWN)
						break;
				}
				if (helper >= 56)
					sum += rook_open_file * mgPhase / 24;
				break;
			case BLACK_ROOK:
				for (helper = sq % WIDTH; helper < 56; helper += BOARD_SOUTH) {
					if (myPosition->grid[helper] == BLACK_PAWN || myPosition->grid[helper] == WHITE_PAWN)
						break;
				}
				if (helper >= 56)
					sum -= rook_open_file * mgPhase / 24;
				break;
			case WHITE_BISHOP:
				++bCount[WHITE];
				break;
			case BLACK_BISHOP:
				++bCount[BLACK];
				break;
			case EMPTY:
				continue;
			}
			mgPhase += game_phase_inc[pc];
			mg[pc & 1] += mg_table[pc][sq];
			eg[pc & 1] += eg_table[pc][sq];
		}
		if (bCount[WHITE] > 1)
			sum += bishop_pair;
		if (bCount[BLACK] > 1)
			sum -= bishop_pair;
		sum += ((mg[WHITE] - mg[BLACK]) * mgPhase + (eg[WHITE] - eg[BLACK]) * (24 - mgPhase)) / 24;
		return sum;
	}

	int Evaluate::pawnEval(position* myPosition)
	{
		int sum = 0;
		int i, j;
		unsigned pawnCounts[2][WIDTH];
		memset(pawnCounts, 0, sizeof(pawnCounts));
		//this contradicts the idea of a pawn hashtable, but generally the state of the game shouldnt change much
		for (i = WIDTH; i < 56; ++i) {
			switch (myPosition->grid[i]) {
			case WHITE_PAWN:
				++pawnCounts[WHITE][i % WIDTH];
				switch (i % WIDTH) {
				case 0:
					for (j = i + BOARD_NORTH; j >= WIDTH; j += BOARD_NORTH) {
						if (myPosition->grid[j] == BLACK_PAWN || myPosition->grid[j + BOARD_EAST] == BLACK_PAWN)
							break;
					}
					if (j < WIDTH)
						sum += passed_rank_bonus[i / WIDTH];
					if (myPosition->grid[i + BOARD_NORTHEAST] == WHITE_PAWN) {
						sum += pawn_connected;
						if (i / WIDTH > 1 &&
							myPosition->grid[i + BOARD_NORTHEAST + BOARD_NORTH] == BLACK_PAWN &&
							myPosition->grid[i + BOARD_NORTH] != BLACK_PAWN)
							sum -= pawn_backward;
					}
					break;
				case 7:
					for (j = i + BOARD_NORTH; j >= WIDTH; j += BOARD_NORTH) {
						if (myPosition->grid[j] == BLACK_PAWN || myPosition->grid[j + BOARD_WEST] == BLACK_PAWN)
							break;
					}
					if (j < WIDTH)
						sum += passed_rank_bonus[i / WIDTH];
					if (myPosition->grid[i + BOARD_NORTHWEST] == WHITE_PAWN) {
						sum += pawn_connected;
						if (i / WIDTH > 1 &&
							myPosition->grid[i + BOARD_NORTHWEST + BOARD_NORTH] == BLACK_PAWN &&
							myPosition->grid[i + BOARD_NORTH] != BLACK_PAWN)
							sum -= pawn_backward;
					}
					break;
				default:
					for (j = i + BOARD_NORTH; j >= WIDTH; j += BOARD_NORTH) {
						if (myPosition->grid[j] == BLACK_PAWN || myPosition->grid[j + BOARD_WEST] == BLACK_PAWN || myPosition->grid[j + BOARD_EAST] == BLACK_PAWN)
							break;
					}
					if (j < WIDTH)
						sum += passed_rank_bonus[i / WIDTH];
					if (myPosition->grid[i + BOARD_NORTHWEST] == WHITE_PAWN) {
						sum += pawn_connected;
						if (i / WIDTH > 1 &&
							myPosition->grid[i + BOARD_NORTHWEST + BOARD_NORTH] == BLACK_PAWN &&
							myPosition->grid[i + BOARD_NORTH] != BLACK_PAWN)
							sum -= pawn_backward;
					}
					else if (myPosition->grid[i + BOARD_NORTHEAST] == WHITE_PAWN) {
						sum += pawn_connected;
						if (i / WIDTH > 1 &&
							myPosition->grid[i + BOARD_NORTHEAST + BOARD_NORTH] == BLACK_PAWN &&
							myPosition->grid[i + BOARD_NORTH] != BLACK_PAWN)
							sum -= pawn_backward;
					}
					break;
				}
				break;
			case BLACK_PAWN:
				++pawnCounts[BLACK][i % WIDTH];
				switch (i % WIDTH) {
				case 0:
					for (j = i + BOARD_SOUTH; j < 56; j += BOARD_SOUTH) {
						if (myPosition->grid[j] == WHITE_PAWN || myPosition->grid[j + BOARD_EAST] == WHITE_PAWN)
							break;
					}
					if (j >= 56)
						sum -= passed_rank_bonus[(i ^ 56) / WIDTH];
					if (myPosition->grid[i  + BOARD_SOUTHEAST] == BLACK_PAWN) {
						sum -= pawn_connected;
						if (i  / WIDTH < WIDTH - 1 &&
							myPosition->grid[i  + BOARD_SOUTHEAST + BOARD_SOUTH] == WHITE_PAWN &&
							myPosition->grid[i  + BOARD_SOUTH] != WHITE_PAWN)
							sum += pawn_backward;
					}
					break;
				case 7:
					for (j = i + BOARD_SOUTH; j < 56; j += BOARD_SOUTH) {
						if (myPosition->grid[j] == WHITE_PAWN || myPosition->grid[j + BOARD_WEST] == WHITE_PAWN)
							break;
					}
					if (j >= 56)
						sum -= passed_rank_bonus[(i ^ 56) / WIDTH];
					if (myPosition->grid[i  + BOARD_SOUTHWEST] == BLACK_PAWN) {
						sum -= pawn_connected;
						if (i  / WIDTH < WIDTH - 1 &&
							myPosition->grid[i  + BOARD_SOUTHWEST + BOARD_SOUTH] == WHITE_PAWN &&
							myPosition->grid[i  + BOARD_SOUTH] != WHITE_PAWN)
							sum += pawn_backward;
					}
					break;
				default:
					for (j = i + BOARD_SOUTH; j < 56; j += BOARD_SOUTH) {
						if (myPosition->grid[j] == WHITE_PAWN || myPosition->grid[j + BOARD_WEST] == WHITE_PAWN || myPosition->grid[j + BOARD_EAST] == WHITE_PAWN)
							break;
					}
					if (j >= 56) 
						sum -= passed_rank_bonus[(i ^ 56) / WIDTH];
					if (myPosition->grid[i  + BOARD_SOUTHWEST] == BLACK_PAWN) {
						sum -= pawn_connected;
						if (i  / WIDTH < WIDTH - 1 &&
							myPosition->grid[i  + BOARD_SOUTHWEST + BOARD_SOUTH] == WHITE_PAWN &&
							myPosition->grid[i  + BOARD_SOUTH] != WHITE_PAWN)
							sum += pawn_backward;
					}
					else if (myPosition->grid[i  + BOARD_SOUTHEAST] == BLACK_PAWN) {
						sum -= pawn_connected;
						if (i  / WIDTH < WIDTH - 1 &&
							myPosition->grid[i  + BOARD_SOUTHEAST + BOARD_SOUTH] == WHITE_PAWN &&
							myPosition->grid[i  + BOARD_SOUTH] != WHITE_PAWN)
							sum += pawn_backward;
					}
					break;
				}
				break;
			}
		}
		for (i = 0; i < WIDTH; ++i) {
			sum -= pawn_file_population[pawnCounts[WHITE][i]] -
				pawn_file_population[pawnCounts[BLACK][i]];
		}
		for (i = 1; i < WIDTH - 1; ++i) {
			if (pawnCounts[WHITE][i - 1] == 0 && pawnCounts[WHITE][i + 1] == 0)
				sum -= pawn_isolated * pawnCounts[WHITE][i];
			if (pawnCounts[BLACK][i - 1] == 0 && pawnCounts[BLACK][i + 1] == 0)
				sum += pawn_isolated * pawnCounts[BLACK][i];
		}
		if (pawnCounts[WHITE][1] == 0)
			sum -= pawn_isolated * pawnCounts[WHITE][0];
		if (pawnCounts[WHITE][6] == 0)
			sum -= pawn_isolated * pawnCounts[WHITE][7];
		if (pawnCounts[BLACK][1] == 0)
			sum += pawn_isolated * pawnCounts[BLACK][0];
		if (pawnCounts[BLACK][6] == 0)
			sum += pawn_isolated * pawnCounts[BLACK][7];
		return sum;
	}
}