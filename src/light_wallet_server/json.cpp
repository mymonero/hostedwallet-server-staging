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
#include "json.h"

#include <boost/optional/optional.hpp>

#include "light_wallet_server/db/data.h"
#include "light_wallet_server/db/string.h"
#include "serialization/new/json_input.h"
#include "serialization/new/json_output.h"

namespace lws
{
namespace json
{
    namespace
    {
        template<typename S, typename T>
        expect<void> address_format(S&& stream, T& value)
        {
            static constexpr const auto fmt = ::json::object(
                ::json::field("spend_public", ::json::hex_string),
                ::json::field("view_public", ::json::hex_string)
            );
            return fmt(
                std::forward<S>(stream), value.spend_public, value.view_public
            );
        }

        template<typename S, typename T1, typename T2>
        expect<void> account_format(S&& stream, T1& value, T2& key)
        {
            static constexpr const auto fmt = ::json::object(
                ::json::field("id", ::json::uint32),
                ::json::field("address", json::account_address),
                ::json::optional_field("view_key", ::json::hex_string),
                ::json::field("scan_height", ::json::uint64),
                ::json::field("start_height", ::json::uint64),
                ::json::field("access_time", ::json::uint32),
                ::json::field("creation_time", ::json::uint32)
            );
            return fmt(
                std::forward<S>(stream), 
                value.id,
                value.address,
                key,
                value.scan_height,
                value.start_height,
                value.access,
                value.creation
            );
        }

        template<typename S, typename T>
        expect<void> block_info_format(S&& stream, T& value)
        {
            static constexpr const auto fmt = ::json::object(
                ::json::field("height", ::json::uint64),
                ::json::field("hash", ::json::hex_string)
            );
            return fmt(std::forward<S>(stream), value.id, value.hash);
        }

        template<typename S, typename T>
        expect<void> spend_format(S&& stream, T& value)
        {
            static constexpr const auto fmt = ::json::object(
                ::json::field("key_image", ::json::hex_string),
                ::json::field("mixin_count", ::json::uint32)
            );
            return fmt(std::forward<S>(stream), value.image, value.mixin_count);
        }

        template<typename S, typename T1, typename T2>
        expect<void> request_format(S&& stream, T1& value, T2& key)
        {
            static constexpr const auto fmt = ::json::object(
                ::json::field("address", json::account_address),
                ::json::optional_field("view_key", ::json::hex_string),
                ::json::field("start_height", ::json::uint64)
            );
            return fmt(
                std::forward<S>(stream),
                value.address, key, value.start_height
            );
        }
    } // anonymous

    expect<void> status_::operator()(std::ostream& dest, db::account_status src) const
    {
        char const* const value = db::status_string(src);
        MONERO_PRECOND(value != nullptr);
        return ::json::string(dest, value);
    }

    expect<void> account_address_::operator()(rapidjson::Value const& src, db::account_address& dest) const
    {
        return address_format(src, dest);
    }
    expect<void> account_address_::operator()(std::ostream& dest, db::account_address const& src) const
    {
        return address_format(dest, src);
    }

    expect<void> account_::operator()(rapidjson::Value const& src, db::account& dest) const
    {
        boost::optional<db::view_key> key;
        MONERO_CHECK(account_format(src, dest, key));
       // if (show_sensitive && !key)
//            return {lws::error::kInvalidKey};
        if (key)
            dest.key = *key;
        return success();
    }
    expect<void> account_::operator()(std::ostream& dest, db::account const& src) const
    {

        db::view_key const* const key =
            show_sensitive ? std::addressof(src.key) : nullptr;
        return account_format(dest, src, key);
    }

    expect<void> block_info_::operator()(rapidjson::Value const& src, db::block_info& dest) const
    {
        return block_info_format(src, dest);
    }
    expect<void> block_info_::operator()(std::ostream& dest, db::block_info const& src) const
    {
        return block_info_format(dest, src);
    }

    expect<void> output_::operator()(std::ostream& dest, db::output const& src) const
    {
        static constexpr const auto fmt = ::json::object(
            ::json::field("id", ::json::uint64),
            ::json::field("block", ::json::uint64),
            ::json::field("index", ::json::uint32),
            ::json::field("amount", ::json::uint64),
            ::json::field("timestamp", ::json::uint64),
            ::json::field("tx_hash", ::json::hex_string),
            ::json::field("tx_prefix_hash", ::json::hex_string),
            ::json::field("tx_public", ::json::hex_string),
            ::json::optional_field("rct_mask", ::json::hex_string),
            ::json::optional_field("payment_id", ::json::hex_string),
            ::json::field("unlock_time", ::json::uint64),
            ::json::field("mixin_count", ::json::uint32),
            ::json::field("coinbase", ::json::boolean)
        );

        const std::pair<db::extra, std::uint8_t> unpacked =
            db::unpack(src.extra);

        const bool coinbase =
            (lmdb::to_native(unpacked.first) & lmdb::to_native(db::extra::kCoinbase));
        const bool rct =
            (lmdb::to_native(unpacked.first) & lmdb::to_native(db::extra::kRingct));

        const auto rct_mask = rct ? std::addressof(src.ringct.mask) : nullptr;

        epee::span<const std::uint8_t> payment_bytes{};
        if (unpacked.second == 32)
            payment_bytes = epee::as_byte_span(src.payment_id.long_);
        else if (unpacked.second == 8)
            payment_bytes = epee::as_byte_span(src.payment_id.short_);

        const auto payment_id = payment_bytes.empty() ?
            nullptr : std::addressof(payment_bytes);

        return fmt(
            dest,
            src.id,
            src.height,
            src.index,
            src.amount,
            src.timestamp,
            src.tx_hash,
            src.tx_prefix_hash,
            src.tx_public,
            rct_mask,
            payment_id,
            src.unlock_time,
            src.mixin_count,
            coinbase
        );
    }

    expect<void> spend_::operator()(rapidjson::Value const& src, db::spend& dest) const
    {
        return spend_format(src, dest);
    }
    expect<void> spend_::operator()(std::ostream& dest, db::spend const& src) const
    {
        return spend_format(dest, src);
    }

    expect<void> request_info_::operator()(rapidjson::Value const& src, db::request_info& dest) const
    {
        boost::optional<db::view_key> key;
        MONERO_CHECK(request_format(src, dest, key));
//        if (show_sensitive && !key)
//            return {lws::error::kInvalidKey};
        if (key)
            dest.key = *key;
        return success();
    }
    expect<void> request_info_::operator()(std::ostream& dest, db::request_info const& src) const
    {
        db::view_key const* const key =
            show_sensitive ? std::addressof(src.key) : nullptr;
        return request_format(dest, src, key);
    }
} // json
} // lws
