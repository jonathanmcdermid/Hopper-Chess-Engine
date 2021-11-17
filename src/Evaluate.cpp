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
		-167, -89, -34, -49,  61, -97, -15, -107,
		 -73, -41,  72,  36,  23,  62,   7,  -17,
		 -47,  60,  37,  65,  84, 129,  73,   44,
		  -9,  17,  19,  53,  37,  69,  18,   22,
		 -13,   4,  16,  13,  28,  19,  21,   -8,
		 -23,  -9,  12,  10,  19,  17,  25,  -16,
		 -29, -53, -12,  -3,  -1,  18, -14,  -19,
		-105, -21, -58, -33, -17, -28, -19,  -23
	};

	int eg_knight_table[64] = {
		-58, -38, -13, -28, -31, -27, -63, -99,
		-25,  -8, -25,  -2,  -9, -25, -24, -52,
		-24, -20,  10,   9,  -1,  -9, -19, -41,
		-17,   3,  22,  22,  22,  11,   8, -18,
		-18,  -6,  16,  25,  16,  17,   4, -18,
		-23,  -3,  -1,  15,  10,  -3, -20, -22,
		-42, -20, -10,  -5,  -2, -20, -23, -44,
		-29, -51, -23, -15, -22, -18, -50, -64
	};

	int mg_bishop_table[64] = {
		-29,   4, -82, -37, -25, -42,   7,  -8,
		-26,  16, -18, -13,  30,  59,  18, -47,
		-16,  37,  43,  40,  35,  50,  37,  -2,
		 -4,   5,  19,  50,  37,  37,   7,  -2,
		 -6,  13,  13,  26,  34,  12,  10,   4,
		  0,  15,  15,  15,  14,  27,  18,  10,
		  4,  15,  16,   0,   7,  21,  33,   1,
		-33,  -3, -14, -21, -13, -12, -39, -21
	};

	int eg_bishop_table[64] = {
		-14, -21, -11,  -8, -7,  -9, -17, -24,
		 -8,  -4,   7, -12, -3, -13,  -4, -14,
		  2,  -8,   0,  -1, -2,   6,   0,   4,
		 -3,   9,  12,   9, 14,  10,   3,   2,
		 -6,   3,  13,  19,  7,  10,  -3,  -9,
		-12,  -3,   8,  10, 13,   3,  -7, -15,
		-14, -18,  -7,  -1,  4,  -9, -15, -27,
		-23,  -9, -23,  -5, -9, -16,  -5, -17
	};

	int mg_rook_table[64] = {
		 32,  42,  32,  51, 63,  9,  31,  43,
		 27,  32,  58,  62, 80, 67,  26,  44,
		 -5,  19,  26,  36, 17, 45,  61,  16,
		-24, -11,   7,  26, 24, 35,  -8, -20,
		-36, -26, -12,  -1,  9, -7,   6, -23,
		-45, -25, -16, -17,  3,  0,  -5, -33,
		-44, -16, -20,  -9, -1, 11,  -6, -71,
		-19, -13,   1,  17, 16,  7, -37, -26
	};

	int eg_rook_table[64] = {
		13, 10, 18, 15, 12,  12,   8,   5,
		11, 13, 13, 11, -3,   3,   8,   3,
		 7,  7,  7,  5,  4,  -3,  -5,  -3,
		 4,  3, 13,  1,  2,   1,  -1,   2,
		 3,  5,  8,  4, -5,  -6,  -8, -11,
		-4,  0, -5, -1, -7, -12,  -8, -16,
		-6, -6,  0,  2, -9,  -9, -11,  -3,
		-9,  2,  3, -1, -5, -13,   4, -20
	};

	int mg_queen_table[64] = {
		-28,   0,  29,  12,  59,  44,  43,  45,
		-24, -39,  -5,   1, -16,  57,  28,  54,
		-13, -17,   7,   8,  29,  56,  47,  57,
		-27, -27, -16, -16,  -1,  17,  -2,   1,
		 -9, -26,  -9, -10,  -2,  -4,   3,  -3,
		-14,   2, -11,  -2,  -5,   2,  14,   5,
		-35,  -8,  11,   2,   8,  15,  -3,   1,
		 -1, -18,  -9,  10, -15, -25, -31, -50
	};

	int eg_queen_table[64] = {
		 -9,  22,  22,  27,  27,  19,  10,  20,
		-17,  20,  32,  41,  58,  25,  30,   0,
		-20,   6,   9,  49,  47,  35,  19,   9,
		  3,  22,  24,  45,  57,  40,  57,  36,
		-18,  28,  19,  47,  31,  34,  39,  23,
		-16, -27,  15,   6,   9,  17,  10,   5,
		-22, -23, -30, -16, -16, -23, -36, -32,
		-33, -28, -22, -43,  -5, -32, -20, -41
	};

	int mg_king_table[64] = {
		-65,  23,  16, -15, -56, -34,   2,  13,
		 29,  -1, -20,  -7,  -8,  -4, -38, -29,
		 -9,  24,   2, -16, -20,   6,  22, -22,
		-17, -20, -12, -27, -30, -25, -14, -36,
		-49,  -1, -27, -39, -46, -44, -33, -51,
		-14, -14, -22, -46, -44, -30, -15, -27,
		  1,   7,  -8, -64, -43, -16,   9,   8,
		-15,  36,  12, -54,   8, -28,  24,  14
	};

	int eg_king_table[64] = {
		-74, -35, -18, -18, -11,  15,   4, -17,
		-12,  17,  14,  17,  17,  38,  23,  11,
		 10,  17,  23,  15,  20,  45,  44,  13,
		 -8,  22,  24,  27,  26,  33,  26,   3,
		-18,  -4,  21,  24,  27,  23,   9, -11,
		-19,  -3,  11,  21,  23,  16,   7,  -9,
		-27, -11,   4,  13,  14,   4,  -5, -17,
		-53, -34, -21, -11, -28, -14, -24, -43
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

	int mg_value[6] = { 82, 337, 365, 477, 1025,  0 };
	int eg_value[6] = { 94, 281, 297, 512,  936,  0 };

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