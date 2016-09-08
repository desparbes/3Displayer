set(
  3DISPLAYER_SRC
  main.c
  draw.c
  project.c
  point.c
  position.c
  solid.c
  scene.c
  frame.c
  camera.c
  lens.c
  array.c
  equation.c
  light.c
  pixel.c
  object.c
  texture.c
  build.c
  parametric.c
  buffer.c
  hypergrid.c
  window.c
  config.c
  )

add_executable(3Displayer ${3DISPLAYER_SRC})
target_link_libraries(3Displayer
  m swap state stack readline
  ${GLIB_LIBRARIES}
  ${GLIB_GMODULE_LIBRARIES}
  ${GLIB_GIO_LIBRARIES}
  ${GLIB_GOBJECT_LIBRARIES}
  ${SDL2_LIBRARY})

add_sanitizers(3Displayer)

install(TARGETS 3Displayer DESTINATION .)
add_subdirectory(SDL)
add_subdirectory(ncurses)
