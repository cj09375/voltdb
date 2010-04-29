/* This file is part of VoltDB.
 * Copyright (C) 2008-2010 VoltDB L.L.C.
 *
 * VoltDB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VoltDB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VoltDB.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef STREAMBLOCK_H_
#define STREAMBLOCK_H_

#include "common/FatalException.hpp"

#include <cassert>
#include <cstring>
#include <stdint.h>

namespace voltdb
{
    /**
     * A single data block with some buffer semantics.
     */
    class StreamBlock {
    public:
        StreamBlock(char* data, size_t capacity, size_t uso)
            : m_data(data), m_capacity(capacity), m_offset(0), m_uso(uso)
        {
        }

        ~StreamBlock()
        {
            delete[] m_data;
        }

        const char* const dataPtr() const {
            return m_data;
        }

        /**
         * Returns the universal stream offset of the block not
         * including any of the octets in this block.
         */
        const size_t uso() const {
            return m_uso;
        }

        /**
         * Returns the additional offset from uso() to count all the
         * octets in this block.  uso() + offset() will compute the
         * universal stream offset for the entire block.
         */
        const size_t offset() const {
            return m_offset;
        }

    private:
        char* mutableDataPtr() {
            return m_data;
        }

        void consumed(size_t consumed) {
            m_offset += consumed;
            assert (m_offset < m_capacity);
        }

        void truncateTo(size_t mark) {
            // just move offset. pretty easy.
            if (((m_uso + m_offset) >= mark ) && (m_uso <= mark)) {
                m_offset = mark - m_uso;
            }
            else {
                throwFatalException("Attempted ELT block truncation past start of block."
                                    "\n m_uso(%jd), m_offset(%jd), mark(%jd)\n",
                                    (intmax_t)m_uso, (intmax_t)m_offset, (intmax_t)mark);
            }
        }

        char *m_data;
        const size_t m_capacity;
        size_t m_offset;  // position for next write.
        size_t m_uso;     // universal stream offset of m_offset 0.

        friend class TupleStreamWrapper;
    };
}

#endif
