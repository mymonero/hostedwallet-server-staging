
set(lmdb_sources database.cpp error.cpp table.cpp value_stream.cpp)
set(lmdb_headers database.h error.h key_stream.h table.h transaction.h util.h value_stream.h)

monero_add_library(lmdb-lib ${lmdb_sources} ${lmdb_headers})
