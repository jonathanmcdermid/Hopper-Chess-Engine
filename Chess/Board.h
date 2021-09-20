#pragma once

#include "zobrist.h"
#include <vector>

namespace Hopper 
{
	struct historyInfo {
		int fHist = 0;
		int cHist = 0;
		int vHist = 0;
		U64 zHist = 0;
		U64 pHist = 0;
		Move mHist = NULLMOVE;
		historyInfo(int f, int c, int v, U64 z, U64 p, Move m) {
			fHist = f;
			cHist = c;
			vHist = v;
			zHist = z;
			pHist = p;
			mHist = m;
		}
	};

	class Board 
	{
	public:
		Board();
		~Board() { ; }
		void fenSet(const char* fs);
		bool isCheckMate();
		bool isRepititionDraw();
		bool isMaterialDraw();
		bool isEndgame();
		bool isCheck() const { return threatened[!turn][kpos[turn]]; }
		int getCurrC() const { return hist.back().cHist; }
		int getCurrV() const { return hist.back().vHist; }
		int getCurrF() const { return hist.back().fHist; }
		U64 getCurrZ() const { return hist.back().zHist; }
		U64 getCurrP() const { return hist.back().pHist; }
		Move getCurrM() const { return hist.back().mHist; }
		void movePiece(Move m);
		void unmovePiece();
		void allThreats();
		void pieceThreats(int from);
		bool validateMove(Move m);
		Move createMove(int from, int to);
		int removeIllegal(Move* m, int cmove);
		int genAll(Move* m);
		int genAllCaps(Move* m);
		int genAllNonCaps(Move* m);
		int pieceCapMoves(Move* m, int from);
		int pieceNonCapMoves(Move* m, int from);
		int pieceMoves(Move* m, int from);
		enum::role_enum grid[SPACES];
		enum::side_enum turn;
		int threatened[2][SPACES] = { 0 };
		int attackers[2][WIDTH][SPACES] = { 0 };
		int pins[5][2] = { 0 };
		int roles[2][5] = { 0 };
		int cpins = 0;
		int kpos[2] = { 0 };
	private:
		Zobrist z;
		std::vector<historyInfo> hist;
	};
}
