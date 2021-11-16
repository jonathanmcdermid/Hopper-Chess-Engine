#pragma once

namespace Hopper
{
	class Board;

	extern int mg_pawn_table[64];
	extern int eg_pawn_table[64];
	extern int mg_knight_table[64];
	extern int eg_knight_table[64];
	extern int mg_bishop_table[64];
	extern int eg_bishop_table[64];
	extern int mg_rook_table[64];
	extern int eg_rook_table[64];
	extern int mg_queen_table[64];
	extern int eg_queen_table[64];
	extern int mg_king_table[64];
	extern int eg_king_table[64];

	extern int* mg_pesto_table[6];

	extern int* eg_pesto_table[6];

	extern int mg_table[12][64];
	extern int eg_table[12][64];

	extern int mg_value[6];
	extern int eg_value[6];

	extern int passed_rank_bonus[8];

	extern int pawn_file_population[5];

	extern int game_phase_inc[13];

	extern int pawn_isolated;
	extern int pawn_connected;
	extern int pawn_backward;



	extern int knight_outpost;
	extern int bishop_outpost;
	extern int bishop_pair;
	extern int rook_open_file;
	extern int queen_support;

	class Evaluate
	{
	public:
		Evaluate();
		void initEvalTables();
		int eval(position* myPosition);
		int pawnEval(position* myPosition);
	private:
	};
}