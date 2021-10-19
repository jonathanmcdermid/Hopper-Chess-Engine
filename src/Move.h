#pragma once

#include "macros.h"

namespace Hopper
{
	typedef unsigned long long U64;
	class Move
	{
	public:
		Move(int from, int to, int flags) { myMove = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f); }
		Move(int n) { myMove = n; }
		Move() { myMove = NULLMOVE; }
		void operator=(int a) { myMove = a; }
		bool operator==(int a) const { return myMove == a; }
		bool operator!=(int a) const { return myMove != a; }
		void operator=(Move a) { myMove = a.myMove; }
		bool operator==(Move a) const { return (myMove & 0xffff) == (a.myMove & 0xffff); }
		bool operator!=(Move a) const { return (myMove & 0xffff) != (a.myMove & 0xffff); }
		bool isCap() const { return myMove != 28672 && ((myMove >> 12) & 0x0f) & 1 << 2; }
		int getTo() const { return myMove & 0x3f; }
		int getFrom() const { return (myMove >> 6) & 0x3f; }
		int getFlags() const { return (myMove >> 12) & 0x0f; }
	private:
		int myMove;
	};
}