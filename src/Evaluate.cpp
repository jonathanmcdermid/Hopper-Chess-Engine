#include <cmath>
#include <cstring>
#include <cassert>
#include "Engine.h"
#include "Evaluate.h"

namespace Hopper
{
	int mg_pawn_table[64] = {
		68, 64, 11, 65, 28, 86, -116, -71,
		-6, 7, 26, 11, 25, 86, 25, 10,
		-4, 3, 6, 21, 33, 42, 17, -3,
		-17, -12, -5, 2, 17, 16, 10, -5,
		-26, -24, -14, -10, 13, 13, 23, -2,
		-25, -21, -20, -23, -15, 14, 28, -12,
		0, 0, 0, 0, 0, 0, 0, 0,
	};

	int eg_pawn_table[64] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		48, 33, 38, -26, -23, -28, 45, 47,
		74, 60, 35, 17, 6, 13, 42, 44,
		42, 34, 23, 5, 8, 14, 27, 27,
		33, 29, 17, 13, 13, 12, 13, 19,
		24, 17, 24, 21, 20, 15, 9, 12,
		33, 18, 28, 30, 33, 30, 12, 13,
		0, 0, 0, 0, 0, 0, 0, 0,
	};

	int mg_knight_table[64] = {
		-197, -109, -104, 41, 91, -257, 85, -147,
		17, -11, 52, 46, 63, 92, -33, 43,
		-17, 30, 57, 75, 104, 99, 93, 74,
		21, 27, 49, 53, 37, 59, 38, 32,
		-3, 4, 26, 23, 38, 29, 51, 12,
		-13, 1, 12, 20, 29, 17, 25, 4,
		-49, -33, -2, 7, -1, -2, -4, 1,
		-125, -21, -38, -23, -17, -8, -9, -103,
	};

	int eg_knight_table[64] = {
		-28, 52, 57, 22, 9, 103, -13, -29,
		-5, 22, 25, 58, 51, 15, 36, 8,
		26, 40, 60, 69, 49, 61, 21, -1,
		23, 53, 62, 72, 82, 81, 68, 42,
		22, 44, 66, 75, 66, 67, 54, 22,
		-23, 17, 39, 65, 50, 37, 20, -12,
		8, 30, 10, 25, 48, 20, -3, -4,
		-29, -41, 17, 15, 8, 2, -40, 36,
	};

	int mg_bishop_table[64] = {
		-69, -46, -42, -47, -75, -92, 67, -18,
		-16, 6, 12, 17, 10, 39, -2, 33,
		4, 17, 53, 40, 65, 60, 77, 28,
		-14, 25, 19, 60, 37, 47, 17, 8,
		-6, 3, 13, 26, 44, 2, 0, 14,
		0, 5, 5, 15, 4, 7, 8, 20,
		-6, 5, 16, -10, -3, -9, 23, 11,
		7, 7, -24, -21, -33, -12, -19, -11,
	};

	int eg_bishop_table[64] = {
		36, 49, 39, 52, 53, 41, 13, 46,
		32, 36, 37, 38, 47, 37, 36, 26,
		32, 42, 40, 39, 38, 56, 40, 44,
		27, 39, 52, 49, 54, 40, 63, 42,
		24, 43, 53, 49, 37, 50, 37, 1,
		8, 27, 38, 40, 53, 33, 23, 5,
		16, 12, 23, 29, 24, 1, 15, -17,
		-3, 1, -3, 15, 11, 4, 35, -7,
	};

	int mg_rook_table[64] = {
		92, 102, 92, 61, 133, 119, 111, 183,
		57, 42, 68, 92, 70, 127, 76, 104,
		15, 49, 56, 76, 87, 115, 141, 86,
		16, 19, 37, 46, 44, 45, 32, 30,
		-26, -16, -12, -1, -1, 3, 16, 17,
		-25, -5, -16, -7, -17, -10, 35, 17,
		-44, -6, -10, -9, -11, 1, 14, -71,
		-9, -3, 1, 7, 6, -3, 23, -16,
	};

	int eg_rook_table[64] = {
		43, 40, 38, 55, 32, 42, 38, 25,
		51, 53, 53, 51, 57, 13, 28, 23,
		47, 37, 47, 35, 34, 37, 15, 27,
		44, 43, 43, 41, 42, 41, 39, 32,
		33, 45, 38, 44, 45, 34, 32, 19,
		-4, 10, 25, 19, 23, 18, 12, -6,
		4, -6, 10, 2, 11, 1, -1, 27,
		11, 22, 23, 19, 5, 17, 14, 0,
	};

	int mg_queen_table[64] = {
		-28, 10, 49, 62, 59, 84, 93, 95,
		-4, -39, -15, -39, -16, 57, 8, 34,
		-13, -7, -3, -2, -1, 96, 97, 37,
		3, -7, -6, -6, -1, 17, 28, 31,
		-9, -6, 1, -10, -12, 6, 23, 17,
		-14, 2, -1, -2, -5, -8, 24, 5,
		-25, 2, 11, 2, -2, -5, 7, -19,
		-1, -18, -9, 0, -15, -25, 49, -20,
	};

	int eg_queen_table[64] = {
		91, 82, 82, 77, 117, 109, 100, 50,
		53, 100, 102, 121, 158, 145, 140, 140,
		40, 56, 99, 109, 157, 115, 109, 149,
		3, 52, 84, 95, 137, 140, 117, 96,
		22, 68, 49, 87, 101, 84, 59, 73,
		4, 3, 45, 36, 39, 57, 10, 35,
		8, -13, -40, 4, 4, -13, -26, -2,
		-23, -8, -22, -23, -15, -32, -130, -1,
	};

	int mg_king_table[64] = {
		-5, 113, 176, 105, -66, -74, 142, -167,
		39, 99, 130, -47, 112, 86, 42, -39,
		81, 104, 72, -16, 30, 56, 82, -52,
		3, 30, 28, 63, -30, 65, 26, -46,
		41, 89, 43, -19, 24, -14, 47, -11,
		-24, -24, -42, -26, -4, -20, 15, -7,
		11, 17, -8, -54, -23, -36, 19, 28,
		5, 46, 2, -84, -12, -68, 24, 34,
	};

	int eg_king_table[64] = {
		-64, -15, -28, -18, -31, 35, -6, -67,
		-22, 27, 24, 27, 17, 18, 53, -9,
		30, 37, 43, 35, 30, 55, 44, 23,
		22, 42, 44, 47, 56, 43, 46, 23,
		-18, 6, 31, 54, 47, 43, 19, -1,
		-19, 17, 31, 41, 33, 26, 7, -9,
		3, -11, 4, 13, 14, 14, -5, -27,
		-63, -24, -1, -11, -58, -4, -24, -73,
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

	int mg_value[6] = { 62, 347, 365, 487, 1125, 0, };
	int eg_value[6] = { 94, 311, 347, 632, 1096, 0, };

	int passed_rank_bonus[8] = { 0, 146, 88, 44, 22, 16, 8, 0, };

	int pawn_file_population[5] = { 0, 0, 12, 54, 18,};

	int game_phase_inc[13] = { 0,0,1,1,1,1,2,2,4,4,0,0,0 };

	int pawn_connected = 5;
	int pawn_backward = 0;
	int pawn_isolated = 16;

	int knight_outpost = 20;
	int bishop_outpost = 10;
	int rook_open_file = 10;
	int bishop_pair = 32;

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