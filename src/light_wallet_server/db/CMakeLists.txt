set(lws-db_sources storage.cpp string.cpp)
set(lws-db_headers data.h fwd.h storage.h string.h)

monero_add_library(light_wallet_server-db ${lws-db_sources} ${lws-db_headers})
target_link_libraries(light_wallet_server-db
    cryptonote_core
    lmdb-lib
    light_wallet_server-lib
    serialization-new
)
