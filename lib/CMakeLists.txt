add_library(meow OBJECT
  src/menu.c
  src/map.c
)

target_include_directories(meow PUBLIC
  $ENV{TIGCC}/include/c
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
