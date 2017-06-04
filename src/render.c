#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <vlc/vlc.h>

static SDL_GLContext gl_context;
static SDL_Window *window;
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

int close_vlc() {
    // Stop stream and clean up libVLC.
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(libvlc);
    return 0;
}

int render_init(){
    window = SDL_CreateWindow("Amber", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    gl_context = SDL_GL_CreateContext(window);
    
    glewInit();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    
    // Init Texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Init SDL_surface
    sdlStruct.sdlSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, W, H, 16, 0xf800, 0x07e0, 0x001f, 0);
    sdlStruct.sdlMutex = SDL_CreateMutex();
    
    setup_vlc();
    return 0;
}

int render_handle_command(){
    

    
    return 0;
}

// Load a texture
void loadTexture() {

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

   // Building the texture
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindTexture(GL_TEXTURE_2D, textureId);
   glTexImage2D(GL_TEXTURE_2D, 0, 4, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, (Uint8 *) rawData);

   free(rawData);
}

int render() {
    
    
    loadTexture(); // Loading the texture
    
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
    
    SDL_GL_SwapWindow(window);
    
    
    state = libvlc_media_player_get_state(mp);
    return 0;
}

int render_close(){
    close_vlc();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    return 0;
}