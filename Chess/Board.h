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
		void fenSet(const char* fs);
		bool isCheckMate();
		bool isRepititionDraw();
		bool isMaterialDraw();
		bool isEndgame();
		bool isCheck() const { return threatened[!turn * SPACES + kpos[turn]]; }
		int getCurrC() const { return hist.back().cHist; }
		int getCurrV() const { return hist.back().vHist; }
		int getCurrF() const { return hist.back().fHist; }
		U64 getCurrZ() const { return hist.back().zHist; }
		U64 getCurrP() const { return hist.back().pHist; }
		Move getCurrM() const { return hist.back().mHist; }
		bool getTurn() const { return turn; }
		int gridAt(int x)const { return (int) grid[x]; }
		int threatenedAt(bool team, int x)const { return threatened[team * SPACES + x]; }
		int getAttackers(int x, int y, int z)const { return attackers[x][y][z]; }
		void setAttackers(int x, int y, int z, int val) { attackers[x][y][z] = val; }
		int pinsAt(int x)const { return pins[x]; }
		int rolesAt(int x)const { return roles[x]; }
		int cpinsAt()const { return cpins; }
		int kposAt(int x)const { return kpos[x]; }
		void movePiece(Move m);
		void unmovePiece();
		bool validateMove(const Move m);
		Move createMove(int from, int to);
		int genAll(Move* m);
		int genAllCaps(Move* m);
		int genAllNonCaps(Move* m);
	private:
		void pieceThreats(int from);
		void allThreats();
		int pieceCapMoves(Move* m, int from);
		int pieceNonCapMoves(Move* m, int from);
		int pieceMoves(Move* m, int from);
		int removeIllegal(Move* m, int cmove);
		Zobrist z;
		std::vector<historyInfo> hist;
		enum::role_enum grid[SPACES];
		int attackers[2][WIDTH][SPACES];
		int threatened[SPACES * 2];
		int pins[10];
		int roles[10];
		int kpos[2];
		bool turn;
		int cpins;
	};
}