// Copyright (c) 2018, The Monero Project
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#pragma once

#include <boost/range/iterator_range.hpp>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <lmdb.h>
#include <utility>

#include "lmdb/value_stream.h"
#include "span.h"

namespace lmdb
{
    template<typename K, typename V>
    class key_iterator
    {
        MDB_cursor* cur;
        epee::span<const std::uint8_t> key;
        
        void increment()
        {
            if (cur)
                key = lmdb::stream::get(*cur, MDB_NEXT_NODUP, sizeof(K), sizeof(V)).first;
        }

    public:
        using value_type = std::pair<K, boost::iterator_range<value_iterator<V>>>;
        using reference = value_type;
        using pointer = void;
        using difference_type = std::size_t;
        using iterator_category = std::input_iterator_tag;

        //! Construct an "end" iterator.
        key_iterator() noexcept
          : cur(nullptr), key()
        {}

        key_iterator(MDB_cursor* cur)
          : cur(cur), key()
        {
            if (cur)
                key = lmdb::stream::get(*cur, MDB_GET_CURRENT, sizeof(K), sizeof(V)).first;
        }

        //! \return True if `this` is one-past the last key.
        bool is_end() const noexcept { return key.empty(); }

        //! \return True iff `rhs` is referencing `this` key.
        bool equal(key_iterator const& rhs) const noexcept
        {
            return
                (key.empty() && rhs.key.empty()) ||
                key.data() == rhs.key.data();
        }

        /*!
            Moves iterator to next key or end.

            \note Invalidates all prior copies of this iterator, and all
                `value_iterator`s returned by `make_value_iterator` and
                `make_value_range`.

            \return `*this`.
        */
        key_iterator& operator++()
        {
            increment();
            return *this;
        }

        /*!
            Moves iterator to next key or end.

            \note Invalidates all prior copies of this iterator, and all
                `value_iterator`s returned by `make_value_iterator` and
                `make_value_range`.

            \return Ignore return value, it is an old copy.
        */
        key_iterator operator++(int)
        {
            key_iterator out{*this};
            increment();
            return out;
        }

        //! \pre `!is_end()` \return {current key, current value range}
        value_type operator*() const
        {
            return {get_key(), make_value_range()};
        }

        //! \pre `!is_end()` \return Current key
        K get_key() const noexcept
        {
            assert(!is_end());
            K out;
            std::memcpy(std::addressof(out), key.data(), sizeof(out));
            return out;
        }

        /*!
            Get a specific field within `F`. Default behavior is to return
            the entirety of `F`, despite the filtering logic of `operator*`.

            \return A different field, `G`, at `uoffset` within `F`.
        */
        template<typename T = V, typename F = T, std::size_t offset = 0>
        value_iterator<T, F, offset> make_value_iterator() const
        {
            static_assert(std::is_same<T, V>(), "bad MONERO_FIELD usage?");
            return {cur};
        }

        template<typename T = V, typename F = T, std::size_t offset = 0>
        boost::iterator_range<value_iterator<T, F, offset>> make_value_range() const
        {
            return {make_value_iterator<T, F, offset>(), value_iterator<T, F, offset>{}};
        }
    };

    //! C++ wrapper for a LMDB read-only cursor on a fixed-sized value `T`.
    template<typename K, typename V, typename D>
    class key_stream
    {
        std::unique_ptr<MDB_cursor, D> cur;
    public:
        explicit key_stream(std::unique_ptr<MDB_cursor, D> cur)
          : cur(std::move(cur))
        {}

        key_stream(key_stream&&) = default;
        key_stream(key_stream const&) = delete;
        ~key_stream() = default;
        key_stream& operator=(key_stream&&) = default;
        key_stream& operator=(key_stream const&) = delete;

        std::unique_ptr<MDB_cursor, D> give_cursor() noexcept
        {
            return {std::move(cur)};
        }

        /*!
            Place the stream back at the first key/value. Newly created
            iterators will start at the first value again.

            \note Invalidates all current iterators, including those created
                with `make_iterator` or `make_range`. Also invalidates all
                `value_iterator`s created with a `key_iterator`.
        */
        void reset()
        {
            if (cur)
                lmdb::stream::get(*cur, MDB_FIRST, 0, 0);
        }

        //! \return An iterator over the remaining set of keys.
        key_iterator<K, V> make_iterator() const
        {
            return {cur.get()};
        }

        /*!
            \throw std::system_error if LMDB has unexpected errors.
            \return An InputIterator range over values at cursor position.
        */
        boost::iterator_range<key_iterator<K, V>> make_range() const
        {
            return {make_iterator(), key_iterator<K, V>{}};
        }
    };

    template<typename K, typename V>
    inline
    bool operator==(key_iterator<K, V> const& lhs, key_iterator<K, V> const& rhs) noexcept
    {
        return lhs.equal(rhs);
    }

    template<typename K, typename V>
    inline
    bool operator!=(key_iterator<K, V> const& lhs, key_iterator<K, V> const& rhs) noexcept
    {
        return !lhs.equal(rhs);
    }
} // lmdb

