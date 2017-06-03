#include <SDL2/SDL.h>
#include <GL/gl.h>

void render_handle_commands(SDL_Window *window, SDL_GLContext *gl_context) {
    SDL_GL_MakeCurrent(window, gl_context);
    
    // OpenGL Test
    glClearColor( (float)rand() / (float)RAND_MAX , 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    
    SDL_GL_SwapWindow(window);
}