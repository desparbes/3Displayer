add_library(multimedia_ncurses SHARED event.c display.c)
target_link_libraries(multimedia_ncurses ncurses)
install(TARGETS multimedia_ncurses DESTINATION ./multimedia/)