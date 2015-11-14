add_library(multimedia_SDL SHARED event.c display.c)
target_link_libraries(multimedia_SDL SDL)
install(TARGETS multimedia_SDL DESTINATION ./multimedia/)