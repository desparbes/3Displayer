
add_library(ncurses_backend SHARED event.c window.c)
target_link_libraries(ncurses_backend ncurses)
add_sanitizers(ncurses_backend)
install(TARGETS ncurses_backend DESTINATION ./multimedia/)
