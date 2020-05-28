#pragma once

#include "macros.h"

namespace Hopper 
{
	typedef unsigned long long U64;
	class Move 
	{
		public:
			Move(int from, int to, int flags) { m = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f); }
			Move(int n) { m = n; }
			Move() { m = NULLMOVE; }
			~Move() { ; }
			void operator=(Move a) { m = a.m; }
			bool operator==(Move a) const { return (m & 0xffff) == (a.m & 0xffff); }
			bool operator!=(Move a) const { return (m & 0xffff) != (a.m & 0xffff); }
			bool isCap() const { return m != 28672 && ((m >> 12) & 0x0f) & 1 << 2; }
			int getTo() const { return m & 0x3f; }
			int getFrom() const { return (m >> 6) & 0x3f; }
			int getFlags() const { return (m >> 12) & 0x0f; }
		private:
			int m;
	};
}