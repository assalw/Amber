#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "player.h"

static SDL_GLContext gl_context;
static SDL_Window *window;

int render_init(){ 
    window = SDL_CreateWindow("Amber", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    gl_context = SDL_GL_CreateContext(window);
    
    glewInit();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // Print OpenGL version
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s: %s", "OpenGL Version:",  glGetString(GL_VERSION));
    
    player_init();
    return 0;
}

int render_handle_command(){
    return 0;
}

int render() {
    player_next_frame();
    SDL_GL_SwapWindow(window);
    return 0;
}

int render_close(){
    player_close();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    return 0;
}