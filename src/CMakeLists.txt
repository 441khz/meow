add_executable(main
  main.c
  sidebar.c
)

target_link_libraries(main PRIVATE
  meow
)
