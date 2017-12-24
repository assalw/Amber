#include "render.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "player.h"
#include "png.h"

static SDL_GLContext _gl_context;
static SDL_Window *_window;

static GLuint _program, _texture, _vertex_array, _data_buffer, _color_buffer;
static GLint _texture_location;
static int _update_buffers, _render_channels;
static float *_data;
static unsigned char *_color_data;

static int compile_and_check_shader(GLuint shader, const char *shader_file){
    FILE *fd = fopen(shader_file, "rb");
    if(!fd){
        fprintf(stderr, "Could not open file %s\n", shader_file);
        return -1;
    }
    fseek(fd, 0, SEEK_END);

    size_t bytes = ftell(fd);
    rewind(fd);

    char *shader_source = (char*)malloc(bytes + 1);
    if(bytes != fread(shader_source, sizeof(char), bytes, fd)){
        fprintf(stderr, "Error reading file %s\n", shader_file);
        free(shader_source);
        return -1;
    }
    shader_source[bytes] = '\0';
    fclose(fd);

    glShaderSource(shader, 1, (const char **)&shader_source, NULL);
    glCompileShader(shader);

    int shader_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status) {
        int shader_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &shader_log_length);

        char *shader_log = (char*)malloc(shader_log_length + 1);
        glGetShaderInfoLog(shader, shader_log_length, NULL, shader_log);

        fprintf(stderr, "Compilation error in OpenGL shader:\n%s\nSource:\n%s\n", shader_log, shader_source);
        free(shader_source);
        free(shader_log);
        return -1;
    }

    free(shader_source);

    return 0;
}

static int update_buffers(){
    glBindVertexArray(_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, _data_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _render_channels * 5 * sizeof(GLfloat), _data);

    glBindBuffer(GL_ARRAY_BUFFER, _color_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _render_channels * 4 * sizeof(GLubyte), _color_data);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, _data_buffer);
    // X & Y
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    // Width & Height
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(float)));
    // Rotation
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, _color_buffer);
    // R, G, B & A
    glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 4 * sizeof(GLubyte), (GLvoid*)0);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    return 0;
}

static int init_shaders(const char *vertex_file, const char *fragment_file){
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    compile_and_check_shader(vertex_shader, vertex_file);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    compile_and_check_shader(fragment_shader, fragment_file);

    _program = glCreateProgram();
    glAttachShader(_program, vertex_shader);
    glAttachShader(_program, fragment_shader);
    glLinkProgram(_program);

    _texture_location = glGetUniformLocation(_program, "texture_unit");
    if(_texture_location < 0){
        fprintf(stderr, "Could not get OpenGL uniform location\n");
        return -1;
    }

    return 0;
}

static int init_buffers(){
    glGenVertexArrays(1, &_vertex_array);
    glBindVertexArray(_vertex_array);

    glGenBuffers(1, &_data_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _data_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_CHANNELS * 5 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    glGenBuffers(1, &_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_CHANNELS * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

int render_init(int width, int height, const char *vertex_file, const char *fragment_file){ 
    _render_channels = 0;

    _data = (float*)malloc(MAX_CHANNELS * 5 * sizeof(float));
    _color_data = (unsigned char*)malloc(MAX_CHANNELS * 4 * sizeof(unsigned char));

    _window = SDL_CreateWindow("Amber", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
    _gl_context = SDL_GL_CreateContext(_window);

    glewInit();

    // Print OpenGL version
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s: %s", "OpenGL Version:",  glGetString(GL_VERSION));

    init_shaders(vertex_file, fragment_file);
    init_buffers();

    render_init_texture();

    player_init();

    _update_buffers = 1;

    return 0;
}

int render_handle_command(){
    return 0;
}

int render(int width, int height, double transition){
    if(_update_buffers){
        update_buffers();
        _update_buffers = 0;
    }

    //player_next_frame(_texture);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glBindVertexArray(_vertex_array);
    glUseProgram(_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glUniform1i(_texture_location, 0);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _render_channels);

    SDL_GL_SwapWindow(_window);

    return 0;
}

int render_close(){
    player_close();
    SDL_GL_DeleteContext(_gl_context);
    SDL_DestroyWindow(_window);

    return 0;
}

int render_add_channel(float x, float y, float width, float height, float rotation){
    int current = _render_channels;
    if(current == MAX_CHANNELS - 1){
        return -1;
    }
    _render_channels++;

    size_t color_index = current * 4;
    _color_data[color_index + 0] = 255;
    _color_data[color_index + 1] = 0;
    _color_data[color_index + 2] = 0;
    _color_data[color_index + 3] = 255;

    size_t data_index = current * 5;
    _data[data_index + 0] = x;
    _data[data_index + 1] = y;
    _data[data_index + 2] = width;
    _data[data_index + 3] = height;
    _data[data_index + 4] = rotation;

    _update_buffers = 1;

    return 0;
}

int render_load_png(const char *file){
    int png = loadPng(file, file);
    if(png < 0){
        fprintf(stderr, "Loading image \"%s\" failed\n", file);
        return png;
    }

    int width = getPngWidth(png);
    int height = getPngHeight(png);

    render_load_texture(getPngData(png), width, height);

    return png;
}

void render_load_texture(void * rawdata, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawdata);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void render_init_texture() {
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}