#include "player.h"

#include <vlc/vlc.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

static libvlc_instance_t *libvlc;
static libvlc_media_player_t *mp;
static libvlc_state_t state;

#define VIDEOFILE "./test.mp4" // The movie to read

// WIDTH, HEIGHT
#define W 800
#define H 800

GLuint textureId; // Texture ID

// SDL Structure
struct sdlStructure {
    SDL_Surface *sdlSurface;
    SDL_mutex *sdlMutex;
};

struct sdlStructure sdlStruct;

// Lock func
static void * lockfct(void *data, void **p_pixels) {
   SDL_LockMutex(sdlStruct.sdlMutex);
   SDL_LockSurface(sdlStruct.sdlSurface);
   *p_pixels = sdlStruct.sdlSurface->pixels;
   return NULL;
}

// Unlock func
static void unlockfct(void *data, void *id, void * const *p_pixels) {
   SDL_UnlockSurface(sdlStruct.sdlSurface);
   SDL_UnlockMutex(sdlStruct.sdlMutex);
}

int player_init() {
    // Init SDL_surface
    sdlStruct.sdlSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, W, H, 16, 0xf800, 0x07e0, 0x001f, 0);
    sdlStruct.sdlMutex = SDL_CreateMutex();
    
    
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

    m = libvlc_media_new_path(libvlc, VIDEOFILE);
    mp = libvlc_media_player_new_from_media(m);
    libvlc_media_release(m);
    libvlc_release(libvlc);
    
    
    libvlc_video_set_callbacks(mp, lockfct, unlockfct, NULL, &sdlStruct);
    libvlc_video_set_format(mp, "RV16", W, H, W*2);

    // Start reading the video
    libvlc_media_player_play(mp);
    state = libvlc_media_player_get_state(mp);

    return 0;
}

int player_next_frame(GLuint _texture) {
    // Render to render: target http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/  
    void * rawData = (void *) malloc(W * H * 4);
    Uint8 * pixelSource;
    Uint8 * pixelDestination = (Uint8 *) rawData;
    Uint32 pix;

    for (unsigned int i = H; i > 0; i--) {
        for (unsigned int j = 0; j < W; j++) {
            pixelSource = (Uint8 *) sdlStruct.sdlSurface->pixels + (i-1) * sdlStruct.sdlSurface->pitch + j * 2;
            pix = *(Uint16 *) pixelSource;
            SDL_GetRGBA(pix, sdlStruct.sdlSurface->format, &(pixelDestination[0]), &(pixelDestination[1]), &(pixelDestination[2]), &(pixelDestination[3]));
            pixelDestination += 4;
        }
    }

    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawData);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(rawData);

    // Update VLC state
    state = libvlc_media_player_get_state(mp);
    
    return 0;
}

int player_close() {
    // Stop stream and clean up libVLC.
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(libvlc);
    return 0;
}