#pragma once

namespace Hopper 
{
	class Pawnhash 
	{
	public:
		Pawnhash() { for (int i = 0; i < HASHSIZE; ++i) { pawnHashTable[i] = 0; } }
		void newEntry(int index, int eval) { pawnHashTable[index] = eval; }
		int getEntry(int index) const { return pawnHashTable[index]; }
	private:
		int pawnHashTable[HASHSIZE];
	};
}