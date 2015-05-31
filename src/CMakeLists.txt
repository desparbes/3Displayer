set(
  3DISPLAYER_SRC
  event.c
  main.c
  draw.c
  project.c
  coord.c
  point.c
  texture.c
  color.c
  solid.c
  scene.c
  frame.c
  equation.c
)

add_executable(3Displayer ${3DISPLAYER_SRC})
target_link_libraries(3Displayer m SDL swap state stack)
install(TARGETS 3Displayer DESTINATION .)