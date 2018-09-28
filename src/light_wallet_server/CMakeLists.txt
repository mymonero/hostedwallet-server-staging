

set(lws_lib_sources account.cpp config.cpp error.cpp json.cpp)
set(lws_lib_headers account.h config.h error.h fwd.h)

monero_add_library(light_wallet_server-lib ${lws_lib_sources} ${lws_lib_headers})
target_link_libraries(light_wallet_server-lib serialization-new)

add_subdirectory(db)

monero_add_executable(light_wallet_server server_main.cpp rates.cpp rest_server.cpp rpc.cpp scanner.cpp)
target_link_libraries(light_wallet_server
  PRIVATE
    cryptonote_core
    cncrypto
    common
    daemon_messages
    device
    epee
    light_wallet_server-db
    light_wallet_server-lib
    lmdb-lib
    daemon_messages
    ${Boost_CHRONO_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTRA_LIBRARIES}
    ${ZMQ_LIB}
)
set_property(TARGET light_wallet_server PROPERTY OUTPUT_NAME "monero-light-wallet-server")

monero_add_executable(light_wallet_server_admin admin_main.cpp)
target_link_libraries(light_wallet_server_admin
  PRIVATE
    cryptonote_core
    cncrypto
    common
    light_wallet_server-db
    light_wallet_server-lib
    serialization-new
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${EXTRA_LIBRARIES}
)
set_property(TARGET light_wallet_server_admin PROPERTY OUTPUT_NAME "monero-light-wallet-server-admin")

install(TARGETS light_wallet_server DESTINATION bin)
install(TARGETS light_wallet_server_admin DESTINATION bin)
