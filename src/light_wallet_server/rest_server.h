
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <memory>
#include <string>

#include "common/expect.h"
#include "light_wallet_server/db/storage.h"

namespace lws
{
class rest_server
{
    struct internal;
    std::unique_ptr<internal> impl;

public:
    explicit rest_server(db::storage disk);

    rest_server(rest_server&&) = default;
    rest_server(rest_server const&) = delete;

    ~rest_server() noexcept;

    rest_server& operator=(rest_server&&) = default;
    rest_server& operator=(rest_server const&) = delete;

    expect<void> run(boost::string_ref address, std::size_t threads);
};
}
