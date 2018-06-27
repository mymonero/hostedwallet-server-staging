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
#include "error.h"

namespace lws
{
    struct category final : std::error_category
    {
        virtual const char* name() const noexcept override final
        {
            return "lws::error_category()";
        }

        virtual std::string message(int value) const override final
        {
            switch (lws::error(value))
            {
                case error::kAbortScan:
                    return "Thread needs to abort its scanning process";
                case error::kAccountExists:
                    return "Account with specified address already exists";
                case error::kBadAddress:
                    return "Invalid base58 public address - wrong --network ?";
                case error::kBadViewKey:
                    return "Address/viewkey mismatch";
                case error::kBadBlockchain:
                    return "Unable to sync blockchain - wrong --network ?";
                case error::kBlockchainReorg:
                    return "A blockchain reorg has been detected";
                case error::kCreateQueueMax:
                    return "Exceeded maxmimum number of pending account requests";
                case error::kDaemonConnectionFailure:
                    return "Connection failed with daemon";
                case error::kDuplicateRequest:
                    return "A request of this type for this address has already been made";
                case error::kExceededBlockchainBuffer:
                    return "Exceeded internal buffer for blockchain hashes";
                case error::kNoSuchAccount:
                    return "No account with the specified address exists";
                case error::kSystemClockInvalidRange:
                    return "System clock is out of range for account storage format";
                default:
                    break;
            }
            return "Unknown lws::error_category() value";
        }

        virtual std::error_condition default_error_condition(int value) const noexcept override final
        {
            switch (lws::error(value))
            {
                case error::kBadAddress:
                case error::kBadViewKey:
                    return std::errc::bad_address;
                case error::kDaemonConnectionFailure:
                    return std::errc::connection_refused;
                case error::kExceededBlockchainBuffer:
                    return std::errc::no_buffer_space;
                case error::kSystemClockInvalidRange:
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
