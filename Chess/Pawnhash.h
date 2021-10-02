#pragma once

#include <string>
#include "macros.h"

namespace Hopper
{
	class Pawnhash
	{
	public:
		Pawnhash() { memset(pawnHashTable, 0, sizeof(pawnHashTable)); }
		void newEntry(int index, int hashEval) { pawnHashTable[index] = hashEval; }
		int getEntry(int index) const { return pawnHashTable[index]; }
	private:
		int pawnHashTable[HASHSIZE];
	};
}