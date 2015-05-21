set(
  3DISPLAYER_SRC
  main.c
  draw.c
  project.c
  basic.c
  solid.c
  data.c
)

add_executable(3Displayer ${3DISPLAYER_SRC})
target_link_libraries(3Displayer m SDL)
install(TARGETS 3Displayer DESTINATION .)