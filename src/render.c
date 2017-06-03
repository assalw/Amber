#include <SDL2/SDL.h>
#include <GL/gl.h>

int render_handle_command(){
    return 0;
}

int render(SDL_Window *window, SDL_GLContext *gl_context) {
    SDL_GL_MakeCurrent(window, gl_context);
    
    // OpenGL Test
    glClearColor( (float)rand() / (float)RAND_MAX , 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    
    SDL_GL_SwapWindow(window);
    
    return 0;
}