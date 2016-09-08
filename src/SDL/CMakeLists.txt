


add_library(SDL_backend SHARED event.c window.c)
target_link_libraries(SDL_backend ${SDL2_LIBRARY})
add_sanitizers(SDL_backend)

install(TARGETS SDL_backend DESTINATION ./multimedia/)