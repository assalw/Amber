#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vlc/vlc.h>

int render_init(){
    
    // Initialize VLC
    libvlc_instance_t *libvlc;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;
    char const *vlc_argv[] = {

        "--no-audio", // Don't play audio.
        "--no-xlib", // Don't use Xlib.

        // Apply a video filter.
        //"--video-filter", "sepia",
        //"--sepia-intensity=200"
    };
    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
  
    libvlc = libvlc_new(vlc_argc, vlc_argv);
    if(NULL == libvlc) {
        printf("LibVLC initialization failure.\n");
        return EXIT_FAILURE;
    }

    
    
    return 0;
}

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