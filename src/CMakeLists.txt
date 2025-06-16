add_executable(main
  main.c
  menu.c
)

target_include_directories(main PRIVATE
  $ENV{TIGCC}/include/c
)
