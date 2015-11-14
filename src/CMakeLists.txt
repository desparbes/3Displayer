set(
  3DISPLAYER_SRC
  main.c
  draw.c
  project.c
  coord.c
  point.c
  position.c
  color.c
  solid.c
  scene.c
  frame.c
  camera.c
  lens.c
  ext.c
  multimedia.c
  array.c
  equation.c
  light.c
  pixel.c
  object.c
  texture.c
  )

add_executable(3Displayer ${3DISPLAYER_SRC})
target_link_libraries(3Displayer dl m swap state stack readline SDL)
install(TARGETS 3Displayer DESTINATION .)
add_subdirectory(multimedia_SDL)
add_subdirectory(multimedia_ncurses)