#ifndef MOVE_H
#define MOVE_H

#include "macros.h"
#include <iostream>

namespace Chess {
	class move {
		public:
			move(uint8_t from, uint8_t to, uint8_t flags) { m = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f); }
			move(uint16_t m) { m = m; }
			move() { m = 28672; }
			void operator=(move a) { m = a.m; }
			bool operator==(move a) const { return (m & 0xffff) == (a.m & 0xffff); }
			bool operator!=(move a) const { return (m & 0xffff) != (a.m & 0xffff); }
			uint8_t getTo() const { return m & 0x3f; }
			uint8_t getFrom() const { return (m >> 6) & 0x3f; }
			uint8_t getFlags() const { return (m >> 12) & 0x0f; }
		private:
			uint16_t m;
	};
}

#endif