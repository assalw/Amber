#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vlc/vlc.h>

static libvlc_instance_t *libvlc;
static libvlc_media_player_t *mp;

int setup_vlc() {
    libvlc_media_t *m;
    
    char const *vlc_argv[] = {
        "--no-audio",
        "--no-xlib",
    };
    
    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
  
    libvlc = libvlc_new(vlc_argc, vlc_argv);
    if(libvlc == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER,"LibVLC initialization failure.\n");
        return -1;
    }

    m = libvlc_media_new_path(libvlc, "");
    mp = libvlc_media_player_new_from_media(m);
    libvlc_media_release(m);
    
    return 0;
}

int close_vlc() {
    // Stop stream and clean up libVLC.
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(libvlc);
    return 0;
}

int render_init(){
    setup_vlc();
    return 0;
}

int render_handle_command(){
    
    // OpenGL Test
    glClearColor( (float)rand() / (float)RAND_MAX , 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    
    // Square
    glBegin(GL_QUADS);
        glTexCoord2d(0, 1);
        glVertex2f(-0.75, 0.75);
        glTexCoord2d(1, 1);
        glVertex2f(0.75, 0.75);
        glTexCoord2d(1, 0);
        glVertex2f(0.75, -0.75);
        glTexCoord2d(0, 0);
        glVertex2f(-0.75, -0.75);
    glEnd();
    
    return 0;
}

int render(SDL_Window *window) {
    SDL_GL_SwapWindow(window);
    return 0;
}

int render_close(){
    close_vlc();
    return 0;
}