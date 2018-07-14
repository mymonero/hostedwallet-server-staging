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
#include "json_error.h"

namespace json
{
    struct category final : std::error_category
    {
        virtual const char* name() const noexcept override final
        {
            return "lws::error_category()";
        }

        virtual std::string message(int value) const override final
        {
            switch (json::error(value))
            {
                case error::kBufferOverflow:
                    return "Internal error - JSON parser has fixed internal buffer that was exceeeded";
                case error::kExpectedArray:
                    return "JSON parser expected array";
                case error::kExpectedBool:
                    return "JSON parser expected bool";
                case error::kExpectedDouble:
                    return "JSON parser expected double";
                case error::kExpectedFloat:
                    return "JSON parser expected float";
                case error::kExpectedObject:
                    return "JSON parser expected object";
                case error::kExpectedString:
                    return "JSON parser expected string";
                case error::kExpectedUnsigned:
                    return "JSON parser expected unsigned integer";
                case error::kInvalidHex:
                    return "JSON parser received invalid hex-ascii string";
                case error::kMissingField:
                    return "JSON parser could not find required field";
                case error::kOverflow:
                    return "JSON integer exceeded internal maximum value";
                case error::kUnexpectedField:
                    return "JSON parser encountered a field that it did not expect";
                case error::kUnderflow:
                    return "JSON integer exceeded internal minimum value";
                default:
                    break;
            }
            return "Unknown json::error_category() value";
        }

        virtual std::error_condition default_error_condition(int value) const noexcept override final
        {
            switch (json::error(value))
            {
                case error::kBufferOverflow:
                    return std::errc::no_buffer_space;
                case error::kOverflow:
                case error::kUnderflow:
                    return std::errc::result_out_of_range;
                default:
                    break; // map to unmatchable category
            }
            return std::error_condition{value, *this};
        }
    };

    std::error_category const& error_category() noexcept
    {
        static const category instance{};
        return instance;
    }
} // lws
