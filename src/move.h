#ifndef MOVE_H
#define MOVE_H

#include "macros.h"
#include <iostream>

namespace Chess {
	typedef unsigned long long U64;
	class move {
		public:
			move(int from, int to, int flags) { m = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f); }
			move(int n) { m = n; }
			move() { m = 28672; }
			void operator=(move a) { m = a.m; }
			bool operator==(move a) const { return (m & 0xffff) == (a.m & 0xffff); }
			bool operator!=(move a) const { return (m & 0xffff) != (a.m & 0xffff); }
			int getTo() const { return m & 0x3f; }
			int getFrom() const { return (m >> 6) & 0x3f; }
			int getFlags() const { return (m >> 12) & 0x0f; }
		private:
			int m;
	};
}

#endif