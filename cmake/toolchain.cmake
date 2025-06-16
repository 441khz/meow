# set(CMAKE_SYSTEM_NAME ti68k) # <-- this will produce some useless warnings and files
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER $ENV{TIGCC}/bin/tigcc)

# tigcc doesn't follow c89 (main is not the entry point), so suppress sanity checks
set(CMAKE_C_COMPILER_WORKS 1)
