#ifndef MOVE_H
#define MOVE_H

#include "macros.h"
#include <iostream>

namespace Chess {
	class move {
		public:
			move(uint8_t from, uint8_t to, uint8_t flags) { m_Move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f); }
			move(uint16_t m) { m_Move = m; }
			move() { m_Move = 28672; }
			void operator=(move a) { m_Move = a.m_Move; }
			bool operator==(move a) const { return (m_Move & 0xffff) == (a.m_Move & 0xffff); }
			bool operator!=(move a) const { return (m_Move & 0xffff) != (a.m_Move & 0xffff); }
			uint8_t getTo() const { return m_Move & 0x3f; }
			uint8_t getFrom() const { return (m_Move >> 6) & 0x3f; }
			uint8_t getFlags() const { return (m_Move >> 12) & 0x0f; }
		private:
			uint16_t m_Move;
	};
}

#endif